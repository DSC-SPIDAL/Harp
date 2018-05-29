/* file: maximum_pooling3d_layer_backward_impl.i */
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

#ifndef __MAXIMUM_POOLING3D_LAYER_BACKWARD_IMPL_I__
#define __MAXIMUM_POOLING3D_LAYER_BACKWARD_IMPL_I__

#include "service_sort.h"
#include "service_memory.h"
#include "service_blas.h"
#include "service_tensor.h"
#include "service_numeric_table.h"

#include "pooling3d_layer_impl.i"

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
namespace maximum_pooling3d
{
namespace backward
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::compute(const Tensor &inputGradTensor,
        const Tensor &selectedPosTensor, Tensor &gradTensor,
        const maximum_pooling3d::Parameter &parameter)
{
    const algorithmFPType zero = 0.0;

    const Collection<size_t> &inputDims = inputGradTensor.getDimensions();
    const Collection<size_t> &gradDims = gradTensor.getDimensions();

    ReadSubtensor<algorithmFPType, cpu> inputGradSubtensor(const_cast<Tensor&>(inputGradTensor), 0, 0, 0, inputDims[0]);
    DAAL_CHECK_BLOCK_STATUS(inputGradSubtensor);
    const algorithmFPType *inputGrad = inputGradSubtensor.get();

    ReadSubtensor<int, cpu> selectedPosSubtensor(const_cast<Tensor&>(selectedPosTensor), 0, 0, 0, inputDims[0]);
    DAAL_CHECK_BLOCK_STATUS(selectedPosSubtensor);
    const int *selectedPos = selectedPosSubtensor.get();

    WriteOnlySubtensor<algorithmFPType, cpu> gradSubtensor(gradTensor, 0, 0, 0, gradDims[0]);
    DAAL_CHECK_BLOCK_STATUS(gradSubtensor);
    algorithmFPType *grad = gradSubtensor.get();

    const size_t gradientSize = gradTensor.getSize();
    daal::services::internal::service_memset<algorithmFPType, cpu>(grad, zero, gradientSize);

    pooling3d::internal::Parameter<cpu> par(parameter.indices.size, parameter.paddings   .size,
                                            parameter.strides.size, parameter.kernelSizes.size,
                                            gradTensor, gradDims, inputDims);

    DAAL_INT ii[nKernelDims + 1];    // index of the input data
    DAAL_INT ik[nKernelDims];        // index of the kernel
    DAAL_INT iv[nKernelDims];        // index of the value
    DAAL_INT inputOffset[nKernelDims + 1];
    DAAL_INT gradOffset[nKernelDims + 1];

    for (ii[0] = 0; ii[0] < par.offset[0]; ii[0]++)
    {
        inputOffset[0] = 0;
        gradOffset[0]  = 0;

        recurrentCompute(0, ii, ik, iv, par.padding, par.stride, par.kernelSize, par.dataSize, par.valueSize,
            par.offset, gradOffset, inputOffset, inputGrad, grad, selectedPos);
    }
    return Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
void PoolingKernel<algorithmFPType, method, cpu>::recurrentCompute(size_t d,
        DAAL_INT *ii, DAAL_INT *ik, DAAL_INT *iv, const DAAL_INT *padding, const DAAL_INT *stride, const DAAL_INT *kernelSize,
        const DAAL_INT *gradSize, const DAAL_INT *inputSize, const DAAL_INT *offset, DAAL_INT *gradOffset, DAAL_INT *inputOffset,
        const algorithmFPType *inputGrad, algorithmFPType *grad, const int *selectedPos)
{
    const algorithmFPType zero = 0.0;
    if (d < nKernelDims)
    {
        /*
         * Loops over the d-th kernel dimension
         */
        for (ik[d] = -padding[d], iv[d] = 0; iv[d] < inputSize[d]; ik[d] += stride[d], iv[d]++)
        {
            inputOffset[d + 1] = offset[d + 1] * (iv[d] + inputSize[d] * (ii[d] + inputOffset[d]));
            gradOffset[d + 1]  = offset[d + 1] * (ik[d] + gradSize[d]  * (ii[d] + gradOffset[d]));

            for (ii[d + 1] = 0; ii[d + 1] < offset[d + 1]; ii[d + 1]++)
            {
                recurrentCompute(d + 1, ii, ik, iv, padding, stride, kernelSize, gradSize, inputSize,
                                 offset, gradOffset, inputOffset, inputGrad, grad, selectedPos);
            }
        }
    }
    else
    {
        /*
         * Input gradient index
         */
        DAAL_INT inputIndex = ii[3] + inputOffset[3];

        DAAL_INT iwk[nKernelDims];              // index of the gradient within kernel
        DAAL_INT gradKernelOffset[nKernelDims];

        if (selectedPos[inputIndex] >= 0)
        {
            DAAL_INT kernelSize01 = kernelSize[1] * kernelSize[2];
            iwk[0] = selectedPos[inputIndex] / kernelSize01;
            DAAL_INT residual = selectedPos[inputIndex] - iwk[0] * kernelSize01;
            iwk[1] = residual / kernelSize[2];
            iwk[2] = residual - iwk[1] * kernelSize[2];
            bool paddingFlag = false;
            for (size_t i = 0; i < nKernelDims; i++)
            {
                DAAL_INT iwkShifted = ik[i] + iwk[i];
                paddingFlag = (paddingFlag || (iwkShifted < 0) || (iwkShifted >= gradSize[i]));
            }
            if (!paddingFlag)
            {
                gradKernelOffset[0] = offset[1] * iwk[0];
                gradKernelOffset[1] = offset[2] * (iwk[1] + gradSize[1] * gradKernelOffset[0]);
                gradKernelOffset[2] = offset[3] * (iwk[2] + gradSize[2] * gradKernelOffset[1]);
                DAAL_INT gradIndex = ii[3] + gradOffset[nKernelDims] + gradKernelOffset[nKernelDims - 1];
                grad[gradIndex] += inputGrad[inputIndex];
            }
        }
    }
}

} // namespace internal
} // namespace backward
} // namespace maximum_pooling3d
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
