/* file: naivebayes_train_partial_result.cpp */
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
//  Implementation of multinomial naive bayes algorithm and types methods.
//--
*/

#include "multinomial_naive_bayes_training_types.h"
#include "serialization_utils.h"
#include "daal_strings.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace multinomial_naive_bayes
{

namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(PartialModel, SERIALIZATION_NAIVE_BAYES_PARTIALMODEL_ID);
}

namespace training
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(PartialResult, SERIALIZATION_NAIVE_BAYES_PARTIAL_RESULT_ID);

PartialResult::PartialResult() {}

/**
 * Returns the partial model trained with the classification algorithm
 * \param[in] id    Identifier of the partial model, \ref classifier::training::PartialResultId
 * \return          Model trained with the classification algorithm
 */
multinomial_naive_bayes::PartialModelPtr PartialResult::get(classifier::training::PartialResultId id) const
{
    return services::staticPointerCast<multinomial_naive_bayes::PartialModel, data_management::SerializationIface>(Argument::get(id));
}

/**
* Returns number of columns in the naive Bayes partial result
* \return Number of columns in the partial result
*/
size_t PartialResult::getNumberOfFeatures() const
{
    PartialModelPtr ntPtr =
        services::staticPointerCast<PartialModel, data_management::SerializationIface>(Argument::get(classifier::training::partialModel));
    return ntPtr ? ntPtr->getNFeatures() : 0;
}

/**
 * Checks partial result of the naive Bayes training algorithm
 * \param[in] input      Algorithm %input object
 * \param[in] parameter  Algorithm %parameter
 * \param[in] method     Computation method
 */
services::Status PartialResult::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, classifier::training::PartialResult::checkImpl(input, parameter));

    const classifier::training::InputIface *algInput = static_cast<const classifier::training::InputIface *>(input);

    size_t nFeatures = algInput->getNumberOfFeatures();
    DAAL_CHECK_STATUS(s, checkImpl(nFeatures,parameter));

    return s;
}

/**
* Checks partial result of the naive Bayes training algorithm
* \param[in] parameter  Algorithm %parameter
* \param[in] method     Computation method
*/
services::Status PartialResult::check(const daal::algorithms::Parameter *parameter, int method)  const
{
    services::Status s;
    size_t nFeatures = getNumberOfFeatures();

    DAAL_CHECK_STATUS(s, checkImpl(nFeatures,parameter));

    return s;
}

services::Status PartialResult::checkImpl(size_t nFeatures,const daal::algorithms::Parameter* parameter) const
{
    services::Status s;
    PartialModelPtr resModel = get(classifier::training::partialModel);
    DAAL_CHECK(resModel, ErrorNullModel);
    const size_t trainingDataFeatures = resModel->getNFeatures();
    DAAL_CHECK(trainingDataFeatures, services::ErrorModelNotFullInitialized);
    const multinomial_naive_bayes::Parameter *algPar = static_cast<const multinomial_naive_bayes::Parameter *>(parameter);
    size_t nClasses = algPar->nClasses;

    s |= checkNumericTable(resModel->getClassSize().get(), classSizeStr(), 0, 0, 1, nClasses);
    s |= checkNumericTable(resModel->getClassGroupSum().get(), groupSumStr(), 0, 0, nFeatures, nClasses);

    return s;
}


}// namespace interface1
}// namespace training
}// namespace multinomial_naive_bayes
}// namespace algorithms
}// namespace daal
