/* file: multiclassclassifier_train.cpp */
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
//  Implementation of multi class classifier algorithm and types methods.
//--
*/

#include "multi_class_classifier_train_types.h"
#include "serialization_utils.h"
#include "daal_strings.h"

namespace daal
{
namespace algorithms
{
namespace multi_class_classifier
{
namespace training
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_MULTICLASS_CLASSIFIER_RESULT_ID);
Result::Result() {}

/**
 * Returns the model trained with the Multi class classifier algorithm
 * \param[in] id    Identifier of the result, \ref classifier::training::ResultId
 * \return          Model trained with the Multi class classifier algorithm
 */
daal::algorithms::multi_class_classifier::ModelPtr Result::get(classifier::training::ResultId id) const
{
    return services::staticPointerCast<daal::algorithms::multi_class_classifier::Model, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Checks the correctness of the Result object
 * \param[in] input     Pointer to the structure of the input objects
 * \param[in] parameter Pointer to the structure of the algorithm parameters
 * \param[in] method    Computation method
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, checkImpl(input, parameter));

    const Parameter *par = static_cast<const Parameter *>(parameter);
    DAAL_CHECK_EX(par->training.get(), services::ErrorNullAuxiliaryAlgorithm, services::ParameterName, trainingStr());
    daal::algorithms::multi_class_classifier::ModelPtr m = get(classifier::training::model);
    if(m->getNumberOfTwoClassClassifierModels() == 0)
    {
        return services::Status(services::ErrorModelNotFullInitialized);
    }
    if(m->getNumberOfTwoClassClassifierModels() != par->nClasses * (par->nClasses - 1) / 2)
    {
        return services::Status(services::ErrorModelNotFullInitialized);
    }
    return s;
}

}// namespace interface1
}// namespace training
}// namespace multi_class_classifier
}// namespace algorithms
}// namespace daal
