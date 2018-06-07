/* file: RatingsPartialModelInputId.java */
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

/**
 * @ingroup implicit_als_prediction
 * @{
 */
package com.intel.daal.algorithms.implicit_als.prediction.ratings;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__IMPLICIT_ALS__PREDICTION__RATINGS__RATINGSPARTIALMODELINPUTID"></a>
 * @brief Available identifiers of input PartialModel objects for the rating prediction stage
 *        of the implicit ALS algorithm
 */
public final class RatingsPartialModelInputId {
    private int _value;

    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the ratings input partial model object identifier using the provided value
     * @param value     Value corresponding to the ratings input partial model object identifier
     */
    public RatingsPartialModelInputId(int value) {
        _value = value;
    }

    /**
     * Returns the value corresponding to the ratings input partial model object identifier
     * @return Value corresponding to the ratings input partial model object identifier
     */
    public int getValue() {
        return _value;
    }

    private static final int usersPartialModelId = 0;
    private static final int itemsPartialModelId = 1;

    /** %Input partial model containing users factors trained by the implicit ALS algorithm in the distributed processing mode */
    public static final RatingsPartialModelInputId usersPartialModel = new RatingsPartialModelInputId(usersPartialModelId);
    /** %Input partial model containing items factors trained by the implicit ALS algorithm in the distributed processing mode */
    public static final RatingsPartialModelInputId itemsPartialModel = new RatingsPartialModelInputId(itemsPartialModelId);
}
/** @} */