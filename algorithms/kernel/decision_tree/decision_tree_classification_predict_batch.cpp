/* file: decision_tree_classification_predict_batch.cpp */
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
//  Implementation of the interface for Decision tree model-based prediction
//--
*/

#include "algorithms/decision_tree/decision_tree_classification_predict_types.h"
#include "decision_tree_classification_model_impl.h"

namespace daal
{
namespace algorithms
{
namespace decision_tree
{
namespace classification
{
namespace prediction
{
namespace interface1
{

using namespace daal::data_management;
using namespace daal::services;

/** Default constructor */
Input::Input() : classifier::prediction::Input() {}

/**
 * Returns the input Model object in the prediction stage of the Decision tree algorithm
 * \param[in] id    Identifier of the input Model object
 * \return          %Input object that corresponds to the given identifier
 */
ModelPtr Input::get(classifier::prediction::ModelInputId id) const
{
    return decision_tree::classification::Model::cast(Argument::get(id));
}

/**
 * Sets the input NumericTable object in the prediction stage of the classification algorithm
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   Pointer to the input object
 */
void Input::set(classifier::prediction::NumericTableInputId id, const data_management::NumericTablePtr & ptr)
{
    Argument::set(id, ptr);
}

/**
 * Sets the input Model object in the prediction stage of the Decision tree algorithm
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   Pointer to the input object
 */
void Input::set(classifier::prediction::ModelInputId id, const ModelPtr & value)
{
    Argument::set(id, value);
}

/**
 * Checks the correctness of the input object
 * \param[in] parameter Pointer to the structure of the algorithm parameters
 * \param[in] method    Computation method
 */
services::Status Input::check(const daal::algorithms::Parameter * parameter, int method) const
{
    return classifier::prediction::Input::check(parameter, method);
}

} // namespace interface1
} // namespace prediction
} // namespace classification
} // namespace decision_tree
} // namespace algorithms
} // namespace daal
