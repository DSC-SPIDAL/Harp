/* file: csv_feature_utils.h */
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

#ifndef __CSV_FEATURE_UTILS_H__
#define __CSV_FEATURE_UTILS_H__

#include <sstream>

#include "services/collection.h"
#include "services/daal_string.h"
#include "data_management/features/defines.h"

namespace daal
{
namespace data_management
{
namespace internal
{

/**
 *  <a name="DAAL-CLASS-DATA_MANAGEMENT__INTERNAL__CSVROWTOKENIZER"></a>
 *  \brief Class that parses single row in CSV file and implements iterator-like
 *         interface to iterate over the parsed tokens separated by comma
 */
class CSVRowTokenizer : public Base
{
private:
    char *_rawData;
    const size_t _rawDataSize;
    const char _delimiter;

    size_t _pos;
    size_t _prevPos;
    size_t _tokenSize;
    bool _goodFlag;

public:
    explicit CSVRowTokenizer(char *rawData, size_t rawDataSize, char delimiter) :
        _rawData(rawData),
        _rawDataSize(rawDataSize),
        _delimiter(delimiter),
        _pos(0),
        _prevPos(0),
        _tokenSize(0),
        _goodFlag(true) { }

    void reset()
    {
        _pos       = 0;
        _prevPos   = 0;
        _tokenSize = 0;
        _goodFlag  = true;

        next();
    }

    DAAL_FORCEINLINE void next()
    {
        /* We assume _rawData is single line of CSV file and
         * has a termination character in the end */

        if (!good()) { return; }

        _prevPos = _pos;

        while (isValidSymbol(_pos) && !isStopSymbol(_pos))
        { _pos++; }

        _tokenSize = _pos - _prevPos;
        _goodFlag = isValidSymbol(_prevPos);

        if (isValidSymbol(_pos) && isStopSymbol(_pos))
        {
            _rawData[_pos] = '\0';
            _pos++;
        }
    }

    DAAL_FORCEINLINE bool good() const
    {
        return _goodFlag;
    }

    DAAL_FORCEINLINE services::StringView getCurrentToken() const
    {
        return services::StringView(_rawData + _prevPos, _tokenSize);
    }

private:
    DAAL_FORCEINLINE bool isValidSymbol(size_t index) const
    {
        return index < _rawDataSize &&
               _rawData[index] != '\0';
    }

    DAAL_FORCEINLINE bool isStopSymbol(size_t index) const
    {
        return _rawData[index] == _delimiter;
    }

    CSVRowTokenizer(const CSVRowTokenizer &);
    CSVRowTokenizer &operator=(const CSVRowTokenizer &);
};

/**
 *  <a name="DAAL-CLASS-DATA_MANAGEMENT__INTERNAL__CSVFEATURESINFO"></a>
 *  \brief Class that holds auxiliary information about features being parsed
 */
class CSVFeaturesInfo : public Base
{
public:
    services::Status addFeatureName(const services::StringView &featureName)
    {
        const services::String featureNameStr(featureName.begin(), featureName.end());
        if ( !_featureNames.safe_push_back(featureNameStr) )
        {
            return services::throwIfPossible(services::ErrorMemoryAllocationFailed);
        }
        return services::Status();
    }

    services::Status addFeatureType(const services::StringView &token)
    {
        const features::FeatureType featureType = detectFeatureType(token);
        if ( !_featureTypes.safe_push_back(featureType) )
        {
            return services::throwIfPossible(services::ErrorMemoryAllocationFailed);
        }
        return services::Status();
    }

    size_t getNumberOfFeatures() const
    {
        /* We allow _featureNames to be empty to support a no-header case */
        if (_featureNames.size() != 0)
        {
            DAAL_ASSERT( _featureNames.size() == _featureTypes.size() );
            return _featureNames.size();
        }
        return _featureTypes.size();
    }

    const services::String &getFeatureName(size_t featureIndex) const
    {
        DAAL_ASSERT( _featureNames.size() == 0 ||
                     _featureNames.size() == _featureTypes.size() );
        DAAL_ASSERT( featureIndex < _featureNames.size() );
        return _featureNames[featureIndex];
    }

    features::FeatureType getDetectedFeatureType(size_t featureIndex) const
    {
        DAAL_ASSERT( featureIndex < _featureTypes.size() );
        return _featureTypes[featureIndex];
    }

    bool areFeatureNamesAvailable() const
    {
        return _featureNames.size() > 0;
    }

private:
    static features::FeatureType detectFeatureType(const services::StringView &token)
    {
        return isNumericalFeature(token)
            ? features::DAAL_CONTINUOUS
            : features::DAAL_CATEGORICAL;
    }

    static bool isNumericalFeature(const services::StringView &token)
    {
        std::istringstream iss(token.c_str());
        DAAL_DATA_TYPE f = 0.0; iss >> f;
        return !(iss.fail());
    }

private:
    services::Collection<services::String> _featureNames;
    services::Collection<features::FeatureType> _featureTypes;
};

} // namespace internal
} // namespace data_management
} // namespace daal

#endif
