/* file: average_pooling2d_layer_forward_impl.i */
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
//  Implementation of forward average pooling layer
//--
*/

#ifndef __AVERAGE_POOLING2D_LAYER_FORWARD_IMPL_I__
#define __AVERAGE_POOLING2D_LAYER_FORWARD_IMPL_I__

#include "service_memory.h"
#include "service_data_utils.h"
#include "service_blas.h"
#include "service_tensor.h"

#include "service_mkl_tensor.h"

using namespace daal::services;
using namespace daal::internal;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace average_pooling2d
{
namespace forward
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::initialize(const services::Collection<size_t> &inDims,
        const services::Collection<size_t> &outDims)
{
    size_t dimension = inDims.size();

    outputSize = outputSizePtr.reset(dimension);
    outputStrides = outputStridesPtr.reset(dimension);
    DAAL_CHECK_MALLOC(outputSize && outputStrides);

    outputSize   [0] = outDims[dimension - 1];
    outputStrides[0] = 1;

    for(size_t i = 1; i < dimension; i++)
    {
        outputSize   [i] = outDims[dimension - 1 - i];
        outputStrides[i] = outputStrides[i - 1] * outputSize[i - 1];
    }

    ltUserOutput = xDnnLayout(dimension, outputSize, outputStrides); ON_ERR(ltUserOutput.err);
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::compute(const Tensor &dataTensor, const average_pooling2d::Parameter &parameter, Tensor &valueTensor)
{
    const Collection<size_t> &dims = dataTensor.getDimensions();
    const Collection<size_t> &valueDims = valueTensor.getDimensions();

    MklTensor<algorithmFPType> *dataMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(const_cast<Tensor*>(&dataTensor));
    MklTensor<algorithmFPType> *valueMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(&valueTensor);

    if (dataMklTensor != NULL)
    {
        algorithmFPType *avePoolRes[dnnResourceNumber] = {0};

        dnnLayout_t inputLayout;
        dnnLayout_t workspaceLayout;
        dnnLayout_t resultLayout;

        inputLayout = (dnnLayout_t)dataMklTensor->getDnnLayout();
        avePoolRes[dnnResourceSrc] = dataMklTensor->getDnnArray();

        dnnError_t err;

        if (avePoolPrim == NULL)
        {
            const int inputOffset[2] = { (int)(-parameter.paddings.size[0]), (int)(-parameter.paddings.size[1]) };
            err = dnn::xPoolingCreateForward(&avePoolPrim, dnnAlgorithmPoolingAvg, inputLayout,
                                             parameter.kernelSizes.size, parameter.strides.size, inputOffset, dnnBorderZeros);
            ON_ERR(err);
        }

        if (valueMklTensor != NULL)
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, avePoolPrim, dnnResourceDst); ON_ERR(err);
            valueMklTensor->setDnnLayout(resultLayout);
            avePoolRes[dnnResourceDst] = valueMklTensor->getDnnArray();
            avePoolRes[dnnResourceWorkspace] = avePoolRes[dnnResourceDst];

            err = dnn::xExecute(avePoolPrim, (void **)avePoolRes); ON_ERR(err);
        }
        else
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, avePoolPrim, dnnResourceDst); ON_ERR(err);

            WriteOnlySubtensor<algorithmFPType, cpu> valueBlock(valueTensor, 0, 0, 0, valueDims[0]);
            algorithmFPType *valueArray = valueBlock.get();

            LayoutConvertor<algorithmFPType, cpu> cvFromInnerOutput(&avePoolRes[dnnResourceDst], resultLayout, false, &valueArray, ltUserOutput.get(), true); ON_ERR(cvFromInnerOutput.err);
            avePoolRes[dnnResourceWorkspace] = avePoolRes[dnnResourceDst];

            err = dnn::xExecute(avePoolPrim, (void **)avePoolRes); ON_ERR(err);

            cvFromInnerOutput.convert(); ON_ERR(cvFromInnerOutput.err);

            dnn::xLayoutDelete(resultLayout);
        }
    }
    else
    {
        const algorithmFPType zero = 0.0;
        const algorithmFPType one = 1.0;

        ReadSubtensor<algorithmFPType, cpu> dataBlock(const_cast<Tensor&>(dataTensor), 0, 0, 0, dims[0]);
        DAAL_CHECK_BLOCK_STATUS(dataBlock);
        const algorithmFPType *data = dataBlock.get();

        WriteOnlySubtensor<algorithmFPType, cpu> valueBlock(valueTensor, 0, 0, 0, valueDims[0]);
        DAAL_CHECK_BLOCK_STATUS(valueBlock);
        algorithmFPType *value = valueBlock.get();

        pooling2d::internal::Parameter par(parameter.indices.size, parameter.paddings.size,
                                           parameter.strides.size, parameter.kernelSizes.size,
                                           dataTensor, dims, valueDims);

        defaultCompute(par, data, value);
    }
    return Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
void PoolingKernel<algorithmFPType, method, cpu>::defaultInnerLoop(const pooling2d::internal::Parameter &par,
        DAAL_INT i, DAAL_INT f, DAAL_INT k, DAAL_INT s, DAAL_INT j,
        const algorithmFPType *data, algorithmFPType *valuePtr)
{
    const algorithmFPType zero = 0.0;
    algorithmFPType average = zero;

    /*
     * Loops over the kernel
     */
    DAAL_INT fUpper = f + par.firstKernelSize;
    if (fUpper > par.firstSize + par.firstPadding)
    {
        fUpper = par.firstSize + par.firstPadding;
    }

    for (DAAL_INT fi = f; fi < fUpper; fi++)
    {
        DAAL_INT sUpper = s + par.secondKernelSize;
        if (sUpper > par.secondSize + par.secondPadding)
        {
            sUpper = par.secondSize + par.secondPadding;
        }

        for (DAAL_INT si = s; si < sUpper; si++)
        {
            const DAAL_INT dataIndex = j + par.offsetAfter * (si + par.secondSize * (k + par.offsetBetween * (fi + par.firstSize * i)));
            const bool paddingFlag = ((fi < 0) || (fi >= par.firstSize) || (si < 0) || (si >= par.secondSize));
            const algorithmFPType dataValue = (paddingFlag ? zero : data[dataIndex]);

            average += dataValue;
        }
    }
    valuePtr[j] = average / (algorithmFPType)(par.firstKernelSize * par.secondKernelSize);
}

} // namespace internal
} // namespace forward
} // namespace average_pooling2d
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
