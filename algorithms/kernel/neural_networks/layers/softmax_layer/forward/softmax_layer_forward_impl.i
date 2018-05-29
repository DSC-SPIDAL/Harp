/* file: softmax_layer_forward_impl.i */
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
//  Implementation of softmax layer.
//--
*/

#ifndef __SOFTMAX_LAYER_FORWARD_IMPL_I__
#define __SOFTMAX_LAYER_FORWARD_IMPL_I__

#include "service_data_utils.h"
#include "service_math.h"
#include "service_tensor.h"
#include "service_numeric_table.h"
#include "service_error_handling.h"
#include "threading.h"

using namespace daal::internal;
using namespace daal::services;
using namespace daal::services::internal;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace softmax
{
namespace forward
{
namespace internal
{

/* Threshold for vector exp negative args domain  */
template<typename algorithmFPType> inline algorithmFPType exp_threshold        (void) { return algorithmFPType(0.0); }
template<>                         inline float           exp_threshold<float> (void) { return float (-87.0);  }
template<>                         inline double          exp_threshold<double>(void) { return double(-708.0); }

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status SoftmaxKernel<algorithmFPType, method, cpu>::compute(
    const Tensor &inputTensor,
    const softmax::Parameter &parameter,
    Tensor &resultTensor)
{
    const algorithmFPType minValue = -services::internal::MaxVal<algorithmFPType>::get();

    const size_t dimension = parameter.dimension;
    const size_t dimensionSize = inputTensor.getDimensionSize(dimension);
    const size_t offsetInclude = inputTensor.getSize(dimension, inputTensor.getNumberOfDimensions() - dimension);
    const size_t offsetBefore = inputTensor.getSize() / offsetInclude;
    const size_t offsetAfter = offsetInclude / dimensionSize;

    ReadSubtensor<algorithmFPType, cpu> inputBlock(const_cast<Tensor &>(inputTensor), 0, 0, 0, inputTensor.getDimensionSize(0));
    DAAL_CHECK_BLOCK_STATUS(inputBlock);
    const algorithmFPType *inputArray = inputBlock.get();

    WriteOnlySubtensor<algorithmFPType, cpu> resultBlock(resultTensor, 0, 0, 0, inputTensor.getDimensionSize(0));
    DAAL_CHECK_BLOCK_STATUS(resultBlock);
    algorithmFPType *resultArray = resultBlock.get();

    SafeStatus safeStat;
    threader_for(offsetBefore, offsetBefore, [&](size_t i)
    {
        TArrayScalable<algorithmFPType, cpu> expArrayPtr(dimensionSize * offsetAfter);
        algorithmFPType *expArray = expArrayPtr.get();
        DAAL_CHECK_THR(expArray, ErrorMemoryAllocationFailed);

        TArrayScalable<algorithmFPType, cpu> maxArrayPtr(offsetAfter);
        algorithmFPType *maxArray = maxArrayPtr.get();
        DAAL_CHECK_THR(maxArray, ErrorMemoryAllocationFailed);

        for(size_t j = 0; j < offsetAfter; j++)
        {
            maxArray[j] = minValue;
        }

        for(size_t k = 0; k < dimensionSize; k++)
        {
            for(size_t j = 0; j < offsetAfter; j++)
            {
                const size_t inputIndex = (i * dimensionSize + k) * offsetAfter + j;
                if(inputArray[inputIndex] > maxArray[j])
                {
                    maxArray[j] = inputArray[inputIndex];
                }
            }
        }

        for(size_t k = 0; k < dimensionSize; k++)
        {
            for(size_t j = 0; j < offsetAfter; j++)
            {
                const size_t inputIndex = (i * dimensionSize + k) * offsetAfter + j;
                const size_t expIndex = k * offsetAfter + j;
                expArray[expIndex] = inputArray[inputIndex] - maxArray[j];

                // make all values less than threshold as threshold value
                // to fix slow work on vExp on large negative inputs
#if (__CPUID__(DAAL_CPU) != __avx512_mic__)
                if( expArray[expIndex] < exp_threshold<algorithmFPType>() )
                {
                    expArray[expIndex] = exp_threshold<algorithmFPType>();
                }
#endif
            }
        }

        Math<algorithmFPType, cpu>::vExp(dimensionSize * offsetAfter, expArray, expArray);

        for(size_t j = 0; j < offsetAfter; j++)
        {
            maxArray[j] = (algorithmFPType)0;
        }

        for(size_t k = 0; k < dimensionSize; k++)
        {
            for(size_t j = 0; j < offsetAfter; j++)
            {
                const size_t expIndex = k * offsetAfter + j;
                maxArray[j] += expArray[expIndex];
            }
        }

        for(size_t j = 0; j < offsetAfter; j++)
        {
            maxArray[j] = ((algorithmFPType)1.0) / maxArray[j];
        }


        for(size_t k = 0; k < dimensionSize; k++)
        {
            for(size_t j = 0; j < offsetAfter; j++)
            {
                const size_t resultIndex = (i * dimensionSize + k) * offsetAfter + j;
                const size_t expIndex = k * offsetAfter + j;
                resultArray[resultIndex] = expArray[expIndex] * maxArray[j];
            }
        }
    });
    return Status();
}



} // internal
} // forward
} // namespace softmax
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
