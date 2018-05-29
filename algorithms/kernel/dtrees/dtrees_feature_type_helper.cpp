/* file: dtrees_feature_type_helper.cpp */
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
//  Implementation of service data structures
//--
*/
#include "dtrees_feature_type_helper.h"

namespace daal
{
namespace algorithms
{
namespace dtrees
{
namespace internal
{

FeatureTypes::~FeatureTypes()
{
    destroyBuf();
}

bool FeatureTypes::init(const NumericTable& data)
{
    size_t count = 0;
    _firstUnordered = -1;
    _lastUnordered = -1;
    const size_t p = data.getNumberOfColumns();
    for(size_t i = 0; i < p; ++i)
    {
        if(data.getFeatureType(i) != data_management::features::DAAL_CATEGORICAL)
            continue;
        if(_firstUnordered < 0)
            _firstUnordered = i;
        _lastUnordered = i;
        ++count;
    }
    _bAllUnordered = (p == count);
    if(_bAllUnordered)
    {
        destroyBuf();
        return true;
    }
    if(!count)
        return true;
    allocBuf(_lastUnordered - _firstUnordered + 1);
    if(!_aFeat)
        return false;
    for(size_t i = _firstUnordered; i < _lastUnordered + 1; ++i)
    {
        _aFeat[i - _firstUnordered] = (data.getFeatureType(i) == data_management::features::DAAL_CATEGORICAL);
    }
    return true;
}

void FeatureTypes::allocBuf(size_t n)
{
    destroyBuf();
    if(n)
    {
        _nFeat = n;
        _aFeat = (bool*)daal::services::daal_malloc(_nFeat);
        if(!_aFeat)
            _nFeat = 0;
    }
}

void FeatureTypes::destroyBuf()
{
    if(_aFeat)
    {
        daal::services::daal_free(_aFeat);
        _aFeat = nullptr;
        _nFeat = 0;
    }
}

bool FeatureTypes::findInBuf(size_t iFeature) const
{
    if(iFeature < _firstUnordered)
        return false;
    const size_t i = iFeature - _firstUnordered;
    if(i < _nFeat)
        return _aFeat[i];
    DAAL_ASSERT(iFeature > _lastUnordered);
    return false;
}

IndexedFeatures::~IndexedFeatures()
{
    if(_data)
        daal::services::daal_free(_data);
    delete[] _entries;
}

IndexedFeatures::FeatureEntry::~FeatureEntry()
{
    if(binBorders)
        daal::services::daal_free(binBorders);
}

services::Status IndexedFeatures::FeatureEntry::allocBorders()
{
    DAAL_ASSERT(numIndices > 1);
    if(binBorders)
    {
        daal::services::daal_free(binBorders);
        binBorders = nullptr;
    }
    binBorders = (ModelFPType*)services::daal_malloc(sizeof(ModelFPType)*numIndices);
    return binBorders ? services::Status() : services::Status(services::ErrorMemoryAllocationFailed);
}

services::Status IndexedFeatures::alloc(size_t nC, size_t nR)
{
    const size_t newCapacity = nC*nR;
    if(_data)
    {
        if(newCapacity > _capacity)
        {
            services::daal_free(_data);
            _data = nullptr;
            _capacity = 0;
            _data = (IndexType*)services::daal_malloc(sizeof(IndexType)*newCapacity);
            DAAL_CHECK_MALLOC(_data);
            _capacity = newCapacity;
        }
    }
    else
    {
        _data = (IndexType*)services::daal_malloc(sizeof(IndexType)*newCapacity);
        DAAL_CHECK_MALLOC(_data);
        _capacity = newCapacity;
    }
    if(_entries)
    {
        delete[] _entries;
        _entries = nullptr;
    }
    _entries = new FeatureEntry[nC];
    DAAL_CHECK_MALLOC(_entries);
    _nCols = nC;
    _nRows = nR;
    return services::Status();
}

} /* namespace internal */
} /* namespace dtrees */
} /* namespace algorithms */
} /* namespace daal */
