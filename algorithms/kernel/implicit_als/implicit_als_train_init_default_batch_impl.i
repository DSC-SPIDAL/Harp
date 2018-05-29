/* file: implicit_als_train_init_default_batch_impl.i */
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
//  Implementation of defaultDense method for impicit ALS initialization
//--
*/

#ifndef __IMPLICIT_ALS_TRAIN_INIT_DEFAULT_BATCH_IMPL_I__
#define __IMPLICIT_ALS_TRAIN_INIT_DEFAULT_BATCH_IMPL_I__

#include "uniform_kernel.h"
#include "uniform_impl.i"

namespace daal
{
namespace algorithms
{
namespace implicit_als
{
namespace training
{
namespace init
{
namespace internal
{

template <typename algorithmFPType, CpuType cpu>
services::Status ImplicitALSInitKernelBase<algorithmFPType, cpu>::randFactors(
            size_t nItems, size_t nFactors, algorithmFPType *itemsFactors, int *buffer, engines::BatchBase &engine)
{
    int randMax = 1000;
    const algorithmFPType invRandMax = 1.0 / algorithmFPType(randMax);

    Status s;

    for (size_t i = 0; i < nItems; i++)
    {
        DAAL_CHECK_STATUS(s, (distributions::uniform::internal::UniformKernelDefault<int, cpu>::compute(0, randMax, engine, nFactors - 1, buffer)));
        for (size_t j = 1; j < nFactors; j++)
            itemsFactors[i*nFactors + j] = invRandMax * algorithmFPType(buffer[j-1]);
        }
    return s;
}

}
}
}
}
}
}

#endif
