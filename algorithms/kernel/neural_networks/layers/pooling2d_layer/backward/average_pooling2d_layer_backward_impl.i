/* file: average_pooling2d_layer_backward_impl.i */
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
//  Implementation of backward average pooling layer
//--
*/

#ifndef __AVERAGE_POOLING2D_LAYER_BACKWARD_IMPL_I__
#define __AVERAGE_POOLING2D_LAYER_BACKWARD_IMPL_I__

#include "service_memory.h"
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
namespace backward
{
namespace internal
{


template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::initialize(const services::Collection<size_t> &inDims,
        const services::Collection<size_t> &outDims)
{
    size_t dimension = inDims.size();

    inputSize = inputSizePtr.reset(dimension);
    inputStrides = inputStridesPtr.reset(dimension);
    outputSize = outputSizePtr.reset(dimension);
    outputStrides = outputStridesPtr.reset(dimension);
    DAAL_CHECK_MALLOC(inputSize && inputStrides && outputSize && outputStrides);

    inputSize    [0] = inDims[dimension - 1];
    inputStrides [0] = 1;
    outputSize   [0] = outDims[dimension - 1];
    outputStrides[0] = 1;

    for(size_t i = 1; i < dimension; i++)
    {
        inputSize    [i] = inDims[dimension - 1 - i];
        inputStrides [i] = inputStrides[i - 1] * inputSize[i - 1];
        outputSize   [i] = outDims[dimension - 1 - i];
        outputStrides[i] = outputStrides[i - 1] * outputSize[i - 1];
    }

    ltUserInput  = xDnnLayout(dimension,  inputSize,  inputStrides); ON_ERR(ltUserInput.err);
    ltUserOutput = xDnnLayout(dimension, outputSize, outputStrides); ON_ERR(ltUserOutput.err);
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::compute(const Tensor &inputGradTensor, const pooling2d::Parameter &parameter,
        Tensor &gradTensor, const Tensor *dataTensor)
{
    const algorithmFPType zero = 0.0;

    const Collection<size_t> &inputGradDims = inputGradTensor.getDimensions();
    const Collection<size_t> &gradDims = gradTensor.getDimensions();

    MklTensor<algorithmFPType> *dataMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(const_cast<Tensor *>(dataTensor));
    MklTensor<algorithmFPType> *inputGradMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(const_cast<Tensor *>(&inputGradTensor));
    MklTensor<algorithmFPType> *gradMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(&gradTensor);

    if (dataMklTensor != 0)
    {
        dnnLayout_t inputLayout = (dnnLayout_t)dataMklTensor->getDnnLayout();
        dnnLayout_t inputGradLayout;
        dnnLayout_t workspaceLayout;
        dnnLayout_t resultLayout;
        dnnError_t err;

        algorithmFPType *avePoolRes[dnnResourceNumber] = {0};

        if (avePoolPrim == NULL)
        {
            const int inputOffset[2] = { (int)(-parameter.paddings.size[0]), (int)(-parameter.paddings.size[1]) };
            err = dnn::xPoolingCreateBackward(&avePoolPrim, dnnAlgorithmPoolingAvg, inputLayout,
                                              parameter.kernelSizes.size, parameter.strides.size, inputOffset, dnnBorderZeros);
            ON_ERR(err);
        }

        ReadSubtensor<algorithmFPType, cpu> inputGradBlock;
        LayoutConvertor<algorithmFPType, cpu> cvToInnerInputGrad;

        if (inputGradMklTensor != NULL)
        {
            err = dnn::xLayoutCreateFromPrimitive(&inputGradLayout, avePoolPrim, dnnResourceDiffDst); ON_ERR(err);
            inputGradMklTensor->setDnnLayout(inputGradLayout);
            avePoolRes[dnnResourceDiffDst] = inputGradMklTensor->getDnnArray();
        }
        else
        {
            err = dnn::xLayoutCreateFromPrimitive(&inputGradLayout, avePoolPrim, dnnResourceDiffDst); ON_ERR(err);

            inputGradBlock.set(const_cast<Tensor &>(inputGradTensor), 0, 0, 0, inputGradDims[0]);
            algorithmFPType *inputGradArray = const_cast<algorithmFPType *>(inputGradBlock.get());

            cvToInnerInputGrad.set(&inputGradArray, ltUserInput.get(), true, &avePoolRes[dnnResourceDiffDst], inputGradLayout, false); ON_ERR(cvToInnerInputGrad.err);
            cvToInnerInputGrad.convert(); ON_ERR(cvToInnerInputGrad.err);

            dnn::xLayoutDelete(inputGradLayout);
        }

        avePoolRes[dnnResourceWorkspace] = avePoolRes[dnnResourceDiffDst];

        if (gradMklTensor != NULL)
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, avePoolPrim, dnnResourceDiffSrc); ON_ERR(err);
            gradMklTensor->setDnnLayout(resultLayout);
            avePoolRes[dnnResourceDiffSrc] = gradMklTensor->getDnnArray();

            err = dnn::xExecute(avePoolPrim, (void **)avePoolRes); ON_ERR(err);
        }
        else
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, avePoolPrim, dnnResourceDiffSrc); ON_ERR(err);

            WriteOnlySubtensor<algorithmFPType, cpu> gradBlock(gradTensor, 0, 0, 0, gradDims[0]);
            algorithmFPType *gradArray = gradBlock.get();

            LayoutConvertor<algorithmFPType, cpu> cvFromInnerOutput(&avePoolRes[dnnResourceDiffSrc], resultLayout, false, &gradArray, ltUserOutput.get(), true); ON_ERR(cvFromInnerOutput.err);

            err = dnn::xExecute(avePoolPrim, (void **)avePoolRes); ON_ERR(err);

            cvFromInnerOutput.convert(); ON_ERR(cvFromInnerOutput.err);

            dnn::xLayoutDelete(resultLayout);
        }
    }
    else
    {
        ReadSubtensor<algorithmFPType, cpu> inputBlock(const_cast<Tensor&>(inputGradTensor), 0, 0, 0, inputGradDims[0]);
        DAAL_CHECK_BLOCK_STATUS(inputBlock);
        const algorithmFPType *inputGrad = inputBlock.get();

        WriteOnlySubtensor<algorithmFPType, cpu> gradBlock(gradTensor, 0, 0, 0, gradDims[0]);
        DAAL_CHECK_BLOCK_STATUS(gradBlock);
        algorithmFPType *grad = gradBlock.get();

        daal::services::internal::service_memset<algorithmFPType, cpu>(grad, zero, gradBlock.getSize());

        pooling2d::internal::Parameter par(parameter.indices.size, parameter.paddings   .size,
                                           parameter.strides.size, parameter.kernelSizes.size,
                                           gradTensor, gradDims, inputGradDims);

        defaultCompute(par, inputGrad, NULL, grad);
    }
    return Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
void PoolingKernel<algorithmFPType, method, cpu>::defaultInnerLoop(
    const pooling2d::internal::Parameter &par,
    DAAL_INT i, DAAL_INT f, DAAL_INT k, DAAL_INT s,
    const algorithmFPType *inputGradPtr, const int *selectedPosPtr,
    algorithmFPType *grad)
{
    const algorithmFPType one = 1.0;
    const algorithmFPType gradMultiplier = one / (algorithmFPType)(par.firstKernelSize * par.secondKernelSize);
    for (DAAL_INT j = 0; j < par.offsetAfter; j++)
    {
        algorithmFPType inputValue = gradMultiplier * inputGradPtr[j];

        /*
         * Loops over the kernel
         */
        for (DAAL_INT fi = f; fi < f + par.firstKernelSize; fi++)
        {
            for (DAAL_INT si = s; si < s + par.secondKernelSize; si++)
            {
                DAAL_INT gradIndex = j + par.offsetAfter * (si + par.secondSize * (k + par.offsetBetween * (fi + par.firstSize * i)));
                bool paddingFlag = ((fi < 0) || (fi >= par.firstSize) || (si < 0) || (si >= par.secondSize));

                if (!paddingFlag)
                {
                    grad[gradIndex] += inputValue;
                }
            }
        }
    }
}

} // namespace internal
} // namespace backward
} // namespace average_pooling2d
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
