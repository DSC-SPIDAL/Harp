/* file: DistributedStep3LocalInput.java */
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
 * @ingroup implicit_als_training_distributed
 * @{
 */
package com.intel.daal.algorithms.implicit_als.training;

import com.intel.daal.algorithms.Precision;
import com.intel.daal.algorithms.implicit_als.PartialModel;
import com.intel.daal.data_management.data.Factory;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.data_management.data.KeyValueDataCollection;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__IMPLICIT_ALS__TRAINING__DISTRIBUTEDSTEP3LOCALINPUT"></a>
 * @brief %Input objects for the implicit ALS training algorithm in the third step of the distributed processing mode
 */
public final class DistributedStep3LocalInput extends com.intel.daal.algorithms.Input {

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    public DistributedStep3LocalInput(DaalContext context, long cAlgorithm, Precision prec, TrainingMethod method) {
        super(context);
        this.cObject = cGetInput(cAlgorithm, prec.getValue(), method.getValue());
    }

    /**
     * Sets an input partial model object for the implicit ALS training algorithm
     * in the third step of the distributed processing mode
     * @param id      Identifier of the input object
     * @param val     Value of the input object
     */
    public void set(PartialModelInputId id, PartialModel val) {
        if (id != PartialModelInputId.partialModel) {
            throw new IllegalArgumentException("Incorrect PartialModelInputId");
        }
        cSetPartialModel(this.cObject, id.getValue(), val.getCObject());
    }

    /**
     * Returns an input partial model object for the implicit ALS training algorithm
     * in the third step of the distributed processing mode
     * @param id      Identifier of the input object
     * @return        %Input object that corresponds to the given identifier
     */
    public PartialModel get(PartialModelInputId id) {
        if (id != PartialModelInputId.partialModel) {
            throw new IllegalArgumentException("Incorrect PartialModelInputId"); // error processing
        }
        return new PartialModel(getContext(), cGetPartialModel(this.cObject, id.getValue()));
    }

    /**
     * Sets an input numeric table object for the implicit ALS training algorithm
     * in the third step of the distributed processing mode
     * @param id      Identifier of the input object
     * @param val     Value of the input object
     */
    public void set(Step3LocalNumericTableInputId id, NumericTable val) {
        if (id != Step3LocalNumericTableInputId.offset) {
            throw new IllegalArgumentException("Incorrect Step3LocalNumericTableInputId");
        }
        cSetNumericTable(this.cObject, id.getValue(), val.getCObject());
    }

    /**
     * Returns an input numeric table object for the implicit ALS training algorithm
     * in the third step of the distributed processing mode
     * @param id      Identifier of the input object
     * @return        %Input object that corresponds to the given identifier
     */
    public NumericTable get(Step3LocalNumericTableInputId id) {
        if (id != Step3LocalNumericTableInputId.offset) {
            throw new IllegalArgumentException("Incorrect Step3LocalNumericTableInputId"); // error processing
        }
        return (NumericTable)Factory.instance().createObject(getContext(), cGetNumericTable(this.cObject, id.getValue()));
    }

    /**
     * Sets an input key-value data collection object for the implicit ALS training algorithm
     * in the third step of the distributed processing mode
     * @param id      Identifier of the input object
     * @param val     Value of the input object
     */
    public void set(Step3LocalCollectionInputId id, KeyValueDataCollection val) {
        if (id != Step3LocalCollectionInputId.partialModelBlocksToNode) {
            throw new IllegalArgumentException("Incorrect Step3LocalCollectionInputId");
        }
        cSetDataCollection(this.cObject, id.getValue(), val.getCObject());
    }

    /**
     * Returns an input key-value data collection object for the implicit ALS training algorithm
     * in the third step of the distributed processing mode
     * @param id      Identifier of the input object
     * @return        %Input object that corresponds to the given identifier
     */
    public KeyValueDataCollection get(Step3LocalCollectionInputId id) {
        if (id != Step3LocalCollectionInputId.partialModelBlocksToNode) {
            throw new IllegalArgumentException("Incorrect Step3LocalCollectionInputId"); // error processing
        }
        return new KeyValueDataCollection(getContext(), cGetDataCollection(this.cObject, id.getValue()));
    }

    private native long cGetInput(long cAlgorithm, int prec, int method);

    private native void cSetPartialModel(long cObject, int id, long partialModelAddr);
    private native long cGetPartialModel(long cObject, int id);

    private native void cSetNumericTable(long cObject, int id, long numericTableAddr);
    private native long cGetNumericTable(long cObject, int id);

    private native void cSetDataCollection(long cObject, int id, long numericTableAddr);
    private native long cGetDataCollection(long cObject, int id);
}
/** @} */
