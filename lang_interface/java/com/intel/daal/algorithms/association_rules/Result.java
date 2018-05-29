/* file: Result.java */
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
 * @ingroup association_rules
 * @{
 */
package com.intel.daal.algorithms.association_rules;

import com.intel.daal.algorithms.ComputeMode;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.data_management.data.Factory;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__ASSOCIATION_RULES__RESULT"></a>
 * @brief Results obtained with the compute() method of the association rules algorithm in the batch processing mode
 */
public final class Result extends com.intel.daal.algorithms.Result {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the result
     * @param context  Context to manage the result
     */
    public Result(DaalContext context) {
        super(context);
        this.cObject = cNewResult();
    }

    public Result(DaalContext context, long cObject) {
        super(context, cObject);
    }

    /**
     * Returns the final result of the association rules algorithm
     * @param id   Identifier of the result
     * @return Final result that corresponds to the given identifier
     */
    public NumericTable get(ResultId id) {
        if (id != ResultId.largeItemsets && id != ResultId.largeItemsetsSupport && id != ResultId.antecedentItemsets
                && id != ResultId.consequentItemsets && id != ResultId.confidence) {
            throw new IllegalArgumentException("id unsupported");
        }
        return (NumericTable)Factory.instance().createObject(getContext(), cGetResultTable(cObject, id.getValue()));
    }

    /**
     * Sets the final result of the association rules algorithm
     * @param id   Identifier of the result
     * @param val  Object to store the final result
     */
    public void set(ResultId id, NumericTable val) {
        if (id != ResultId.largeItemsets && id != ResultId.largeItemsetsSupport && id != ResultId.antecedentItemsets
                && id != ResultId.consequentItemsets && id != ResultId.confidence) {
            throw new IllegalArgumentException("id unsupported");
        }
        cSetResultTable(cObject, id.getValue(), val.getCObject());
    }

    private native long cNewResult();

    private native long cGetResultTable(long resAddr, int id);

    private native void cSetResultTable(long resAddr, int id, long ntAddr);

}
/** @} */
