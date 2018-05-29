/* file: svd_dense_default_distr_step2.h */
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
#ifndef __SVD_DENSE_DEFAULT_DISTR_STEP2__
#define __SVD_DENSE_DEFAULT_DISTR_STEP2__

#include "svd_types.h"

using namespace daal::services;
using namespace daal::data_management;

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
 */
template <typename algorithmFPType>
DAAL_EXPORT Status DistributedPartialResult::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    set(outputOfStep2ForStep3, KeyValueDataCollectionPtr(new KeyValueDataCollection()));
    Argument::set(finalResultFromStep2Master, ResultPtr(new Result()));
    KeyValueDataCollectionPtr inCollection = static_cast<const DistributedStep2Input *>(input)->get(inputOfStep2FromStep1);
    size_t nBlocks = 0;
    return setPartialResultStorage<algorithmFPType>(inCollection.get(), nBlocks);
}

/**
 * Allocates memory to store partial results of the SVD algorithm based on the known structure of partial results from step 1 in the
 * distributed processing mode.
 * KeyValueDataCollection under outputOfStep2ForStep3 id is structured the same as KeyValueDataCollection under
 * inputOfStep2FromStep1 id of the algorithm input
 * \tparam     algorithmFPType Data type to use for storage in the resulting HomogenNumericTable
 * \param[in]  inCollection    KeyValueDataCollection of all partial results from the first step of  the SVD algorithm in the distributed
 *                             processing mode
 * \param[out] nBlocks         Number of rows in the input data set
 */
template <typename algorithmFPType>
DAAL_EXPORT Status DistributedPartialResult::setPartialResultStorage(KeyValueDataCollection *inCollection, size_t &nBlocks)
{
    KeyValueDataCollectionPtr partialCollection =
        staticPointerCast<KeyValueDataCollection, SerializationIface>(Argument::get(outputOfStep2ForStep3));
    if(!partialCollection)
    {
        return Status();
    }

    ResultPtr result = staticPointerCast<Result, SerializationIface>(Argument::get(finalResultFromStep2Master));

    size_t inSize = inCollection->size();

    DataCollection *fisrtNodeCollection = static_cast<DataCollection *>((*inCollection).getValueByIndex(0).get());
    NumericTable *firstNumericTable     = static_cast<NumericTable *>((*fisrtNodeCollection)[0].get());

    size_t m = firstNumericTable->getNumberOfColumns();
    if(result->get(singularValues).get() == NULL)
    {
        Status s = result->allocateImpl<algorithmFPType>(m, 0);
        if(!s)
            return s;
    }

    nBlocks = 0;
    Status st;
    for(size_t i = 0 ; i < inSize ; i++)
    {
        DataCollection   *nodeCollection = static_cast<DataCollection *>((*inCollection).getValueByIndex((int)i).get());
        size_t            nodeKey        = (*inCollection).getKeyByIndex((int)i);
        size_t nodeSize = nodeCollection->size();
        nBlocks += nodeSize;

        DataCollectionPtr nodePartialResult(new DataCollection());
        for(size_t j = 0 ; j < nodeSize ; j++)
        {
            nodePartialResult->push_back(HomogenNumericTable<algorithmFPType>::create(m, m, NumericTable::doAllocate, &st));
        }
        (*partialCollection)[ nodeKey ] = nodePartialResult;
    }
    return st;
}

}// namespace interface1
}// namespace svd
}// namespace algorithms
}// namespace daal

#endif
