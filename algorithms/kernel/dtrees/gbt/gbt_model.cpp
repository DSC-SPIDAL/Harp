/* file: gbt_model.cpp */
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
//  Implementation of the class defining the gradient boosted trees model
//--
*/

#include "gbt_model_impl.h"
#include "dtrees_model_impl_common.h"

using namespace daal::data_management;
using namespace daal::services;
using namespace daal::algorithms::dtrees::internal;

namespace daal
{
namespace algorithms
{
namespace gbt
{
namespace internal
{
typedef gbt::internal::ModelImpl::TreeType::NodeType::Leaf TLeaf;
typedef daal::algorithms::regression::TreeNodeVisitor TVisitor;

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

dtrees::internal::DecisionTreeTable* ModelImpl::treeToTable(TreeType& t)
{
    return t.convertToTable();
}

void ModelImpl::add(dtrees::internal::DecisionTreeTable* pTbl)
{
    DAAL_ASSERT(pTbl);
    _nTree.inc();
    _serializationData->push_back(SerializationIfacePtr(pTbl));
}

} // namespace internal
} // namespace gbt
} // namespace algorithms
} // namespace daal
