/* file: DistributedStep2MasterInput.java */
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
import com.intel.daal.data_management.data.DataCollection;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__IMPLICIT_ALS__TRAINING__DISTRIBUTEDSTEP2MASTERINPUT"></a>
 * @brief %Input objects for the implicit ALS training algorithm in the second step of the distributed processing mode
 */
public final class DistributedStep2MasterInput extends com.intel.daal.algorithms.Input {

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    public DistributedStep2MasterInput(DaalContext context, long cAlgorithm, Precision prec, TrainingMethod method) {
        super(context);
        this.cObject = cGetInput(cAlgorithm, prec.getValue(), method.getValue());
    }

    /**
     * Adds an input object for the implicit ALS training algorithm in the second step of the distributed processing mode
     * @param id            Identifier of the input object
     * @param partialResult Value of the input object
     */
    public void add(MasterInputId id, DistributedPartialResultStep1 partialResult) {
        if (id != MasterInputId.inputOfStep2FromStep1) {
            throw new IllegalArgumentException("Incorrect id");
        }
        cAddDataCollection(this.cObject, id.getValue(), partialResult.getCObject());
    }

    /**
     * Returns an input object for the implicit ALS training algorithm in the second step of the distributed processing mode
     * @param id      Identifier of the input object
     * @return        Input object that corresponds to the given identifier
     */
    public DataCollection get(MasterInputId id) {
        if (id != MasterInputId.inputOfStep2FromStep1) {
            throw new IllegalArgumentException("Incorrect id");
        }
        return new DataCollection(getContext(), cGetDataCollection(this.cObject, id.getValue()));
    }

    private native long cGetInput(long cAlgorithm, int prec, int method);

    private native void cAddDataCollection(long cInput, int id, long partialResultAddr);
    private native long cGetDataCollection(long cInput, int id);
}
/** @} */
