/* file: svd_dense_default_distr_step3.h */
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
//  Implementation of svd algorithm and types methods.
//--
*/
#ifndef __SVD_DENSE_DEFAULT_DISTR_STEP3__
#define __SVD_DENSE_DEFAULT_DISTR_STEP3__

#include "svd_types.h"

using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace svd
{
namespace interface1
{

/**
 * Allocates memory to store partial results of the SVD algorithm
 * \param[in] input     Pointer to the input object
 * \param[in] parameter Pointer to the parameter
 * \param[in] method    Algorithm computation method
 */
template <typename algorithmFPType>
DAAL_EXPORT Status DistributedPartialResultStep3::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    Argument::set(finalResultFromStep3, ResultPtr(new Result()));
    return Status();
}

/**
 * Allocates memory to store partial results of the SVD algorithm obtained in the third step in the distributed processing mode
 * \tparam     algorithmFPType            Data type to use for storage in the resulting HomogenNumericTable
 * \param[in]  qCollection  DataCollection of all partial results from step 1 of the SVD algorithm in the distributed processing mode
 */
template <typename algorithmFPType>
DAAL_EXPORT Status DistributedPartialResultStep3::setPartialResultStorage(data_management::DataCollection *qCollection)
{
    size_t qSize = qCollection->size();
    size_t m = 0;
    size_t n = 0;
    for(size_t i = 0 ; i < qSize ; i++)
    {
        data_management::NumericTable  *qNT = static_cast<data_management::NumericTable *>((*qCollection)[i].get());
        m  = qNT->getNumberOfColumns();
        n += qNT->getNumberOfRows();
    }
    ResultPtr result = services::staticPointerCast<Result, data_management::SerializationIface>(Argument::get(finalResultFromStep3));

    return result->allocateImpl<algorithmFPType>(m, n);
}

}// namespace interface1
}// namespace svd
}// namespace algorithms
}// namespace daal

#endif
