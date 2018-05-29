/* file: concat_layer_backward_impl.i */
/*******************************************************************************
* Copyright 2014-2018 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/*
//++
//  Implementation of concat algorithm
//--
*/

#ifndef __CONCAT_LAYER_BACKWARD_IMPL_I__
#define __CONCAT_LAYER_BACKWARD_IMPL_I__

#include "service_micro_table.h"

#include "service_mkl_tensor.h"

using namespace daal::internal;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace concat
{
namespace backward
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
Status ConcatKernel<algorithmFPType, method, cpu>::compute(Tensor *inputTensor, const NumericTable *forwardOutputTable, const concat::Parameter *parameter,
                                                         Tensor *resultTensors[])
{
    const size_t concatDimension = parameter->concatDimension;
    const size_t nOutputs = forwardOutputTable->getNumberOfColumns();

    const size_t nDims = inputTensor->getNumberOfDimensions();

    MklTensor<algorithmFPType> *inputMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(inputTensor);

    bool canUseMklTensor = (inputMklTensor != 0 && concatDimension == 1 && (nDims == 4 || nDims == 5));

    for (size_t i = 0; i < nOutputs && canUseMklTensor; i++)
    {
        MklTensor<algorithmFPType> *resultMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(resultTensors[i]);
        if (!inputMklTensor)
        {
            canUseMklTensor = false;
        }
    }

    {
        ReadRows<int, cpu> inputDims(*const_cast<NumericTable *>(forwardOutputTable), 0, 1);
        DAAL_CHECK_BLOCK_STATUS(inputDims);
        const int *auxDims = inputDims.get();
        if (canUseMklTensor)
        {
            algorithmFPType* splitRes[dnnResourceNumber] = {0};

            dnnLayout_t inputLayout = (dnnLayout_t)inputMklTensor->getDnnLayout();
            dnnError_t err;

            if (splitPrim == NULL)
            {
                daal::internal::TArray<size_t, cpu> dstChannelSize(nOutputs);
                DAAL_CHECK_MALLOC(dstChannelSize.get());

                int *auxDims;
                BlockMicroTable<int, readOnly, cpu> inputDims(forwardOutputTable);
                inputDims.getBlockOfRows(0, 1, &auxDims);

                for (size_t i = 0; i < nOutputs; i++)
                {
                    dstChannelSize[i] = auxDims[i];
                }

                err = dnn::xSplitCreate(&splitPrim, nOutputs, inputLayout, dstChannelSize.get()); ON_ERR(err);
            }

            splitRes[dnnResourceSrc] = inputMklTensor->getDnnArray();

            for (size_t i = 0; i < nOutputs; i++)
            {
                MklTensor<algorithmFPType> *resultMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(resultTensors[i]);
                dnnLayout_t resultLayout;
                err = dnn::xLayoutCreateFromPrimitive(&resultLayout, splitPrim, (dnnResourceType_t)(dnnResourceMultipleDst + i)); ON_ERR(err);

                resultMklTensor->setDnnLayout(resultLayout);
                splitRes[dnnResourceMultipleDst + i] = resultMklTensor->getDnnArray();
            }

            err = dnn::xExecute(splitPrim, (void**)splitRes); ON_ERR(err);
        }
        else
        {
            const Collection<size_t> &dims = inputTensor->getDimensions();
            const size_t nInputRows = dims[0];

            ReadSubtensor<algorithmFPType, cpu, Tensor> inputSubtensor(inputTensor, 0, 0, 0, nInputRows);
            DAAL_CHECK_BLOCK_STATUS(inputSubtensor);
            const algorithmFPType *inputArray = inputSubtensor.get();

            int *auxDims;
            BlockMicroTable<int, readOnly, cpu> inputDims(forwardOutputTable);
            inputDims.getBlockOfRows(0, 1, &auxDims);

            size_t dimsSum = 0;
            for(size_t j = 0; j < nOutputs; j++)
            {
                dimsSum += auxDims[j];
            }

            size_t offsetBefore = 1;
            for(size_t j = 0; j < concatDimension; j++)
            {
                offsetBefore *= dims[j];
            }

            size_t offsetAfter = 1;
            for(size_t j = concatDimension + 1; j < dims.size(); j++)
            {
                offsetAfter *= dims[j];
            }

            size_t sum = 0;
            for(int l = 0; l < nOutputs; l++)
            {
                Tensor *resultTensor = resultTensors[l];

                const Collection<size_t> &resDims = resultTensor->getDimensions();

                const size_t nResultRows = resDims[0];

                WriteOnlySubtensor<algorithmFPType, cpu, Tensor> resultSubtensor(resultTensor, 0, 0, 0, nResultRows);
                DAAL_CHECK_BLOCK_STATUS(resultSubtensor);
                algorithmFPType *resultArray = resultSubtensor.get();

                for(size_t i = 0; i < offsetBefore; i++)
                {
                    for(size_t k = 0; k < auxDims[l]; k++)
                    {
                        for(size_t j = 0; j < offsetAfter; j++)
                        {
                            const size_t outputIndex = (i * auxDims[l] + k) * offsetAfter + j;
                            const size_t inputIndex = (i * dimsSum + k + sum) * offsetAfter + j;
                            resultArray[outputIndex] = inputArray[inputIndex];
                        }
                    }
                }
                sum += auxDims[l];
            }
        }
    }
    return Status();
}

} // namespace internal
} // namespace backward
} // namespace concat
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
