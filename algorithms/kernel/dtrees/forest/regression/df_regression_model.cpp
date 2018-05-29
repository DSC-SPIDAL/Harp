/* file: df_regression_model.cpp */
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
//  Implementation of the class defining the decision forest model
//--
*/

#include "algorithms/decision_forest/decision_forest_regression_model.h"
#include "serialization_utils.h"
#include "df_regression_model_impl.h"
#include "dtrees_model_impl_common.h"

using namespace daal::data_management;
using namespace daal::services;
using namespace daal::algorithms::dtrees::internal;

namespace daal
{
namespace algorithms
{

typedef decision_forest::regression::internal::ModelImpl::TreeType::NodeType::Leaf TLeaf;
typedef daal::algorithms::regression::TreeNodeVisitor TVisitor;

namespace decision_forest
{

namespace regression
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS2(Model, internal::ModelImpl, SERIALIZATION_DECISION_FOREST_REGRESSION_MODEL_ID);

Model::Model(){}

}

namespace internal
{

size_t ModelImpl::numberOfTrees() const
{
    return ImplType::size();
}

void ModelImpl::traverseDF(size_t iTree, algorithms::regression::TreeNodeVisitor& visitor) const
{
    if(iTree >= size())
        return;
    const DecisionTreeTable& t = *at(iTree);
    const DecisionTreeNode* aNode = (const DecisionTreeNode*)t.getArray();
    if(aNode)
        traverseNodeDF<TVisitor>(0, 0, aNode, visitor);
}

void ModelImpl::traverseBF(size_t iTree, algorithms::regression::TreeNodeVisitor& visitor) const
{
    if(iTree >= size())
        return;
    const DecisionTreeTable& t = *at(iTree);
    const DecisionTreeNode* aNode = (const DecisionTreeNode*)t.getArray();
    NodeIdxArray aCur;//nodes of current layer
    NodeIdxArray aNext;//nodes of next layer
    if(aNode)
    {
        aCur.push_back(0);
        traverseNodesBF<TVisitor>(0, aCur, aNext, aNode, visitor);
    }
}

services::Status ModelImpl::serializeImpl(data_management::InputDataArchive  * arch)
{
    auto s = RegressionImplType::serialImpl<data_management::InputDataArchive, false>(arch);
    return s.add(ImplType::serialImpl<data_management::InputDataArchive, false>(arch));
}

services::Status ModelImpl::deserializeImpl(const data_management::OutputDataArchive * arch)
{
    auto s = RegressionImplType::serialImpl<const data_management::OutputDataArchive, true>(arch);
    return s.add(ImplType::serialImpl<const data_management::OutputDataArchive, true>(arch));
}

bool ModelImpl::add(const TreeType& tree)
{
    if(size() >= _serializationData->size())
        return false;
    size_t i = _nTree.inc();
    (*_serializationData)[i - 1].reset(tree.convertToTable());
    return true;
}

} // namespace interface1
} // namespace regression
} // namespace decision_forest
} // namespace algorithms
} // namespace daal
