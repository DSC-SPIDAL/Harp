/* file: maximum_pooling2d_layer_backward_impl.i */
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
//  Implementation of backward pooling layer
//--
*/

#ifndef __MAXIMUM_POOLING2D_LAYER_BACKWARD_IMPL_I__
#define __MAXIMUM_POOLING2D_LAYER_BACKWARD_IMPL_I__

#include "service_memory.h"
#include "service_blas.h"
#include "service_tensor.h"
#include "service_numeric_table.h"
#include "threading.h"

#include "service_dnn.h"
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
namespace maximum_pooling2d
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
    return Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::compute(const Tensor &inputGradTensor,
        const Tensor &selectedPosTensor, Tensor &gradTensor, const Tensor *dataTensor,
        const pooling2d::Parameter &parameter)
{
    const algorithmFPType zero = 0.0;

    const Collection<size_t> &inputGradDims = inputGradTensor.getDimensions();
    const Collection<size_t> &gradDims = gradTensor.getDimensions();

    MklTensor<algorithmFPType> *dataMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(const_cast<Tensor *>(dataTensor));
    MklTensor<algorithmFPType> *inputGradMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(const_cast<Tensor *>(&inputGradTensor));
    MklTensor<algorithmFPType> *gradMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(&gradTensor);
    MklTensor<double> *selectedPosMklTensorDouble = dynamic_cast<MklTensor<double>*>(const_cast<Tensor *>(const_cast<Tensor *>(&selectedPosTensor)));
    MklTensor<float> *selectedPosMklTensorFloat = dynamic_cast<MklTensor<float>*>(const_cast<Tensor *>(const_cast<Tensor *>(&selectedPosTensor)));

    if (dataMklTensor != NULL && (selectedPosMklTensorDouble != NULL || selectedPosMklTensorFloat != NULL))
    {
        dnnLayout_t inputLayout = (dnnLayout_t)dataMklTensor->getDnnLayout();
        dnnLayout_t inputGradLayout;
        dnnLayout_t workspaceLayout;
        dnnLayout_t resultLayout;
        dnnError_t err;

        algorithmFPType *maxPoolRes[dnnResourceNumber] = {0};

        if (maxPoolPrim == NULL)
        {
            const int inputOffset[2] = { (int)(-parameter.paddings.size[0]), (int)(-parameter.paddings.size[1]) };
            err = dnn::xPoolingCreateBackward(&maxPoolPrim, dnnAlgorithmPoolingMax, inputLayout,
                                              parameter.kernelSizes.size, parameter.strides.size, inputOffset, dnnBorderZeros);
            ON_ERR(err);
        }

        ReadSubtensor<algorithmFPType, cpu> inputGradBlock;
        LayoutConvertor<algorithmFPType, cpu> cvToInnerInputGrad;

        if (inputGradMklTensor != NULL)
        {
            err = dnn::xLayoutCreateFromPrimitive(&inputGradLayout, maxPoolPrim, dnnResourceDiffDst); ON_ERR(err);
            inputGradMklTensor->setDnnLayout(inputGradLayout);
            maxPoolRes[dnnResourceDiffDst] = inputGradMklTensor->getDnnArray();
        }
        else
        {
            err = dnn::xLayoutCreateFromPrimitive(&inputGradLayout, maxPoolPrim, dnnResourceDiffDst); ON_ERR(err);

            inputGradBlock.set(const_cast<Tensor&>(inputGradTensor), 0, 0, 0, inputGradDims[0]);
            DAAL_CHECK_BLOCK_STATUS(inputGradBlock);
            algorithmFPType *inputGradArray = const_cast<algorithmFPType *>(inputGradBlock.get());

            cvToInnerInputGrad.set(&inputGradArray, ltUserInput.get(), true, &maxPoolRes[dnnResourceDiffDst], inputGradLayout, false); ON_ERR(cvToInnerInputGrad.err);
            cvToInnerInputGrad.convert(); ON_ERR(cvToInnerInputGrad.err);

            dnn::xLayoutDelete(inputGradLayout);
        }

        err = dnn::xLayoutCreateFromPrimitive(&workspaceLayout, maxPoolPrim, dnnResourceWorkspace); ON_ERR(err);
        if (selectedPosMklTensorDouble != NULL)
        {
            selectedPosMklTensorDouble->setDnnLayout(workspaceLayout);
            maxPoolRes[dnnResourceWorkspace] = (algorithmFPType *)selectedPosMklTensorDouble->getDnnArray();
        }
        else
        {
            selectedPosMklTensorFloat->setDnnLayout(workspaceLayout);
            maxPoolRes[dnnResourceWorkspace] = (algorithmFPType *)selectedPosMklTensorFloat->getDnnArray();
        }

        if (gradMklTensor != NULL)
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, maxPoolPrim, dnnResourceDiffSrc); ON_ERR(err);
            gradMklTensor->setDnnLayout(resultLayout);
            maxPoolRes[dnnResourceDiffSrc] = gradMklTensor->getDnnArray();

            err = dnn::xExecute(maxPoolPrim, (void **)maxPoolRes); ON_ERR(err);
        }
        else
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, maxPoolPrim, dnnResourceDiffSrc); ON_ERR(err);

            WriteOnlySubtensor<algorithmFPType, cpu> gradBlock(gradTensor, 0, 0, 0, gradDims[0]);
            algorithmFPType *gradArray = gradBlock.get();

            LayoutConvertor<algorithmFPType, cpu> cvFromInnerOutput(&maxPoolRes[dnnResourceDiffSrc], resultLayout, false, &gradArray, ltUserOutput.get(), true); ON_ERR(cvFromInnerOutput.err);

            err = dnn::xExecute(maxPoolPrim, (void **)maxPoolRes); ON_ERR(err);

            cvFromInnerOutput.convert(); ON_ERR(cvFromInnerOutput.err);

            dnn::xLayoutDelete(resultLayout);
        }
    }
    else
    {
        ReadSubtensor<algorithmFPType, cpu> inputGradSubtensor(const_cast<Tensor&>(inputGradTensor), 0, 0, 0, inputGradDims[0]);
        DAAL_CHECK_BLOCK_STATUS(inputGradSubtensor);
        const algorithmFPType *inputGrad = inputGradSubtensor.get();

        ReadSubtensor<int, cpu> selectedPosSubtensor(const_cast<Tensor&>(selectedPosTensor), 0, 0, 0, inputGradDims[0]);
        DAAL_CHECK_BLOCK_STATUS(selectedPosSubtensor);
        const int *selectedPos = selectedPosSubtensor.get();

        WriteOnlySubtensor<algorithmFPType, cpu> gradSubtensor(gradTensor, 0, 0, 0, gradDims[0]);
        DAAL_CHECK_BLOCK_STATUS(gradSubtensor);
        algorithmFPType *grad = gradSubtensor.get();

        size_t gradSize = gradTensor.getSize();
        daal::services::internal::service_memset<algorithmFPType, cpu>(grad, zero, gradSize);

        pooling2d::internal::Parameter par(parameter.indices.size, parameter.paddings   .size,
                                           parameter.strides.size, parameter.kernelSizes.size,
                                           gradTensor, gradDims, inputGradDims);

        size_t nDim = inputGradDims.size();
        if (par.firstIndex == nDim - 2 && par.secondIndex == nDim - 1 && par.firstPadding == 0 && par.secondPadding == 0)
        {
            indicesLastZeroPaddingsCompute(par, inputGrad, selectedPos, grad);
        }
        else if (par.firstIndex == 0 && par.secondIndex == 1 && par.firstPadding == 0 && par.secondPadding == 0)
        {
            indicesFirstZeroPaddingsCompute(par, inputGrad, selectedPos, grad);
        }
        else
        {
            defaultCompute(par, inputGrad, selectedPos, grad);
        }
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
    for (DAAL_INT j = 0; j < par.offsetAfter; j++)
    {
        if (selectedPosPtr[j] >= 0)
        {
            DAAL_INT fOffset = selectedPosPtr[j] / par.secondKernelSize;
            DAAL_INT sOffset = selectedPosPtr[j] - fOffset * par.secondKernelSize;
            DAAL_INT fi = f + fOffset;
            DAAL_INT si = s + sOffset;
            bool paddingFlag = ((fi < 0) || (fi >= par.firstSize) || (si < 0) || (si >= par.secondSize));
            if (!paddingFlag)
            {
                DAAL_INT gradIndex = j + par.offsetAfter * (si + par.secondSize * (k + par.offsetBetween * (fi + par.firstSize * i)));
                grad[gradIndex] += inputGradPtr[j];
            }
        }
    }
}

template<typename algorithmFPType, Method method, CpuType cpu>
void PoolingKernel<algorithmFPType, method, cpu>::indicesLastZeroPaddingsCompute(
    const pooling2d::internal::Parameter &par,
    const algorithmFPType *inputGrad, const int *selectedPos,
    algorithmFPType *grad)
{
    threader_for(par.offsetBefore, par.offsetBefore, [&](size_t i)
    {
        /*
         * Loop by the first kernel dimension
         * f - index of the left upper corner of the kernel
         * fo - index of the output value
         */
        for (DAAL_INT f = 0, fo = 0; fo < par.firstOutSize; f += par.firstStride, fo++)
        {
            /*
             * Loop by the second kernel dimension
             * s - index of the left upper corner of the kernel
             */
            DAAL_INT inputIndex = par.secondOutSize * (fo + par.firstOutSize * i);
            const algorithmFPType *inputGradPtr = inputGrad + inputIndex;
            const int *selectedPosPtr = selectedPos + inputIndex;
            for (DAAL_INT s = 0, so = 0; so < par.secondOutSize; s += par.secondStride, so++)
            {
                if (selectedPosPtr[so] >= 0)
                {
                    DAAL_INT fOffset = selectedPosPtr[so] / par.secondKernelSize;
                    DAAL_INT sOffset = selectedPosPtr[so] - fOffset * par.secondKernelSize;
                    DAAL_INT fi = f + fOffset;
                    DAAL_INT si = s + sOffset;
                    bool paddingFlag = ((fi < 0) || (fi >= par.firstSize) || (si < 0) || (si >= par.secondSize));
                    if (!paddingFlag)
                    {
                        DAAL_INT gradIndex = si + par.secondSize * (fi + par.firstSize * i);
                        grad[gradIndex] += inputGradPtr[so];
                    }
                }
            }
        }
    } );
}

template<typename algorithmFPType, Method method, CpuType cpu>
void PoolingKernel<algorithmFPType, method, cpu>::indicesFirstZeroPaddingsCompute(
    const pooling2d::internal::Parameter &par,
    const algorithmFPType *inputGrad, const int *selectedPos,
    algorithmFPType *grad)
{
    /*
     * Loop by the first kernel dimension
     * f - index of the left upper corner of the kernel
     * fo - index of the output value
     */
    threader_for(par.firstOutSize, par.firstOutSize, [&](size_t fo)
    {
        DAAL_INT f = fo * par.firstStride;
        /*
         * Loop by the second kernel dimension
         * s - index of the left upper corner of the kernel
         * so - index of the output value
         */
        for (DAAL_INT s = 0, so = 0; so < par.secondOutSize; s += par.secondStride, so++)
        {
            DAAL_INT inputIndex = par.offsetAfter * (so + par.secondOutSize * fo);
            const algorithmFPType *inputGradPtr = inputGrad + inputIndex;
            const int *selectedPosPtr = selectedPos + inputIndex;
            for (DAAL_INT j = 0; j < par.offsetAfter; j++)
            {
                if (selectedPos[inputIndex] >= 0)
                {
                    DAAL_INT fOffset = selectedPosPtr[j] / par.secondKernelSize;
                    DAAL_INT sOffset = selectedPosPtr[j] - fOffset * par.secondKernelSize;
                    DAAL_INT fi = f + fOffset;
                    DAAL_INT si = s + sOffset;
                    bool paddingFlag = ((fi < 0) || (fi >= par.firstSize) || (si < 0) || (si >= par.secondSize));
                    if (!paddingFlag)
                    {
                        DAAL_INT gradIndex = j + par.offsetAfter * (si + par.secondSize * fi);
                        grad[gradIndex] += inputGradPtr[j];
                    }
                }
            }
        }
    } );
}

} // namespace internal
} // namespace backward
} // namespace maximum_pooling2d
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
