/* file: gbt_classification_train_dense_default_batch_fpt_dispatcher.cpp */
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
//  Implementation of gradient boosted trees container.
//--
*/

#include "gbt_classification_train_container.h"
#include "daal_strings.h"

namespace daal
{
namespace algorithms
{
namespace interface1
{
__DAAL_INSTANTIATE_DISPATCH_CONTAINER(gbt::classification::training::BatchContainer, batch, DAAL_FPTYPE, \
    gbt::classification::training::defaultDense)
}
namespace gbt
{
namespace classification
{
namespace training
{
namespace interface1
{
template<>
DAAL_EXPORT services::Status Batch<DAAL_FPTYPE, gbt::classification::training::defaultDense>::checkComputeParams()
{
    services::Status s = classifier::training::Batch::checkComputeParams();
    if(!s)
        return s;
    const auto x = input.get(classifier::training::data);
    const auto nFeatures = x->getNumberOfColumns();
    DAAL_CHECK_EX(parameter().featuresPerNode <= nFeatures,
        services::ErrorIncorrectParameter, services::ParameterName, featuresPerNodeStr());
    const size_t nSamplesPerTree(parameter().observationsPerTreeFraction*x->getNumberOfRows());
    DAAL_CHECK_EX(nSamplesPerTree > 0,
        services::ErrorIncorrectParameter, services::ParameterName, observationsPerTreeFractionStr());
    return s;
}
}
}
}
}
}
} // namespace daal
