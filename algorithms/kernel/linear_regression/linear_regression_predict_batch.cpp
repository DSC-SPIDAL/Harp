/* file: linear_regression_predict_batch.cpp */
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
//  Implementation of linear regression algorithm classes.
//--
*/

#include "algorithms/linear_regression/linear_regression_predict_types.h"
#include "algorithms/linear_model/linear_model_predict.h"
#include "serialization_utils.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace linear_regression
{
namespace prediction
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_LINEAR_REGRESSION_PREDICTION_RESULT_ID);

/** Default constructor */
Input::Input() : linear_model::prediction::Input(lastModelInputId + 1) {}

/**
 * Returns an input object for making linear regression model-based prediction
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
NumericTablePtr Input::get(NumericTableInputId id) const
{
    return linear_model::prediction::Input::get(linear_model::prediction::NumericTableInputId(id));
}

/**
 * Returns an input object for making linear regression model-based prediction
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
linear_regression::ModelPtr Input::get(ModelInputId id) const
{
    return linear_regression::Model::cast(linear_model::prediction::Input::get(linear_model::prediction::ModelInputId(id)));
}

/**
 * Sets an input object for making linear regression model-based prediction
 * \param[in] id      Identifier of the input object
 * \param[in] value   %Input object
 */
void Input::set(NumericTableInputId id, const NumericTablePtr &value)
{
    linear_model::prediction::Input::set(linear_model::prediction::NumericTableInputId(id), value);
}

/**
 * Sets an input object for making linear regression model-based prediction
 * \param[in] id      Identifier of the input object
 * \param[in] value   %Input object
 */
void Input::set(ModelInputId id, const linear_regression::ModelPtr &value)
{
    linear_model::prediction::Input::set(linear_model::prediction::ModelInputId(id), value);
}

Result::Result() : linear_model::prediction::Result(lastResultId + 1) {};

/**
 * Returns the result of linear regression model-based prediction
 * \param[in] id    Identifier of the result
 * \return          Result that corresponds to the given identifier
 */
NumericTablePtr Result::get(ResultId id) const
{
    return linear_model::prediction::Result::get(linear_model::prediction::ResultId(id));
}

/**
 * Sets the result of linear regression model-based prediction
 * \param[in] id      Identifier of the input object
 * \param[in] value   %Input object
 */
void Result::set(ResultId id, const NumericTablePtr &value)
{
    linear_model::prediction::Result::set(linear_model::prediction::ResultId(id), value);
}

} // namespace interface1
} // namespace prediction
} // namespace linear_regression
} // namespace algorithms
} // namespace daal
