/* file: TrainingInput.java */
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
 * @ingroup implicit_als_training_batch
 * @{
 */
package com.intel.daal.algorithms.implicit_als.training;

import com.intel.daal.algorithms.Precision;
import com.intel.daal.algorithms.implicit_als.Model;
import com.intel.daal.data_management.data.Factory;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__IMPLICIT_ALS__TRAINING__TRAININGINPUT"></a>
 * @brief %Input objects for the implicit ALS training algorithm in the batch processing mode
 */
public class TrainingInput extends com.intel.daal.algorithms.Input {

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    public TrainingInput(DaalContext context, long cObject) {
        super(context, cObject);
    }

    public TrainingInput(DaalContext context, long cAlgorithm, Precision prec, TrainingMethod method) {
        super(context);
        this.cObject = cInit(cAlgorithm, prec.getValue(), method.getValue());
    }

    /**
     * Sets an input numeric table object for the implicit ALS training algorithm in the batch processing mode
     * @param id      Identifier of the input object
     * @param val     Value of the input object
     */
    public void set(NumericTableInputId id, NumericTable val) {
        if (id != NumericTableInputId.data) {
            throw new IllegalArgumentException("Incorrect NumericTableInputId");
        }
        cSetInput(this.cObject, id.getValue(), val.getCObject());
    }

    /**
     * Sets an input model object for the implicit ALS training algorithm in the batch processing mode
     * @param id      Identifier of the input object
     * @param val     Value of the input object
     */
    public void set(ModelInputId id, Model val) {
        if (id != ModelInputId.inputModel) {
            throw new IllegalArgumentException("Incorrect ModelInputId");
        }
        cSetInput(this.cObject, id.getValue(), val.getCObject());
    }

    /**
     * Returns an input numeric table object for the implicit ALS training algorithm in the batch processing mode
     * @param id      Identifier of the input object
     * @return        Input object that corresponds to the given identifier
     */
    public NumericTable get(NumericTableInputId id) {
        if (id != NumericTableInputId.data) {
            throw new IllegalArgumentException("Incorrect NumericTableInputId"); // error processing
        }
        return (NumericTable)Factory.instance().createObject(getContext(), cGetInput(this.cObject, id.getValue()));
    }

    /**
     * Returns an input model object for the implicit ALS training algorithm in the batch processing mode
     * @param id      Identifier of the input object
     * @return        Input object that corresponds to the given identifier
     */
    public Model get(ModelInputId id) {
        if (id != ModelInputId.inputModel) {
            throw new IllegalArgumentException("Incorrect ModelInputId"); // error processing
        }
        return new Model(getContext(), cGetInput(this.cObject, id.getValue()));
    }

    private native long cInit(long algAddr, int prec, int method);

    private native void cSetInput(long cObject, int id, long resAddr);

    private native long cGetInput(long cObject, int id);
}
/** @} */
