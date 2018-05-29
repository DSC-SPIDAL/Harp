/* file: average_pooling1d_layer_backward_impl.i */
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

#ifndef __AVERAGE_POOLING1D_LAYER_BACKWARD_IMPL_I__
#define __AVERAGE_POOLING1D_LAYER_BACKWARD_IMPL_I__

#include "service_memory.h"
#include "service_blas.h"
#include "service_tensor.h"

#include "pooling1d_layer_impl.i"

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
namespace average_pooling1d
{
namespace backward
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status PoolingKernel<algorithmFPType, method, cpu>::compute(const Tensor &inputTensor, const average_pooling1d::Parameter &parameter,
                                                          Tensor &gradTensor)
{
    const Collection<size_t> &inputDims = inputTensor.getDimensions();
    const Collection<size_t> &gradDims = gradTensor.getDimensions();

    ReadSubtensor<algorithmFPType, cpu> inputBlock(const_cast<Tensor&>(inputTensor), 0, 0, 0, inputDims[0]);
    DAAL_CHECK_BLOCK_STATUS(inputBlock);
    const algorithmFPType *inputGrad = inputBlock.get();

    WriteOnlySubtensor<algorithmFPType, cpu> gradBlock(gradTensor, 0, 0, 0, gradDims[0]);
    DAAL_CHECK_BLOCK_STATUS(gradBlock);
    algorithmFPType *grad = gradBlock.get();

    const algorithmFPType zero = 0.0;
    daal::services::internal::service_memset<algorithmFPType, cpu>(grad, zero, gradBlock.getSize());

    pooling1d::internal::Parameter par(parameter.index .size[0], parameter.padding   .size[0],
                                       parameter.stride.size[0], parameter.kernelSize.size[0],
                                       gradTensor, gradDims, inputDims);

    const algorithmFPType one = 1.0;
    const algorithmFPType gradMultiplier = one / (algorithmFPType)(par.kernelSize);
    for (DAAL_INT i = 0; i < par.offsetBefore; i++)
    {
        /*
         * Loop by the first kernel dimension
         * f - index of the left upper corner of the kernel
         * fo - index of the output value
         */
        for (DAAL_INT f = -par.padding, fo = 0; fo < par.firstOutSize; f += par.stride, fo++)
        {
            for (DAAL_INT j = 0; j < par.offsetAfter; j++)
            {
                /*
                 * Input value index
                 */
                const DAAL_INT inputIndex = j + par.offsetAfter * (fo + par.firstOutSize * i);
                const algorithmFPType inputValue = gradMultiplier * inputGrad[inputIndex];

                /*
                 * Loops over the kernel
                 */
                for (DAAL_INT fi = f; fi < f + par.kernelSize; fi++)
                {
                    const DAAL_INT gradIndex = j + par.offsetAfter * (fi + par.firstSize * i);
                    const bool paddingFlag = (fi < 0) || (fi >= par.firstSize);

                    if (!paddingFlag)
                    {
                        grad[gradIndex] += inputValue;
                    }
                }
            }
        }
    }
    return Status();
}
} // namespace internal
} // namespace backward
} // namespace average_pooling1d
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
