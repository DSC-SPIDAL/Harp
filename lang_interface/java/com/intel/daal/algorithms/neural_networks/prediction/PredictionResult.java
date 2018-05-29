/* file: PredictionResult.java */
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
 * @ingroup neural_networks_prediction
 * @{
 */
package com.intel.daal.algorithms.neural_networks.prediction;

import com.intel.daal.data_management.data.Factory;
import com.intel.daal.data_management.data.Tensor;
import com.intel.daal.data_management.data.KeyValueDataCollection;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__PREDICTION__PREDICTIONRESULT"></a>
 * @brief Provides methods to access result obtained with the compute() method of the neural networks prediction algorithm
 */
public class PredictionResult extends com.intel.daal.algorithms.Result {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the result of the neural networks prediction algorithm
     * @param context   Context to manage the result of the neural networks prediction algorithm
     */
    public PredictionResult(DaalContext context) {
        super(context);
    }

    public PredictionResult(DaalContext context, long cObject) {
        super(context, cObject);
    }

    /**
     * Returns the result of the neural networks model based prediction
     * @param  id   Identifier of the result
     * @return Result that corresponds to the given identifier
     */
    public Tensor get(PredictionResultId id) {
        if (id == PredictionResultId.prediction) {
            return (Tensor)Factory.instance().createObject(getContext(), cGetValue(cObject, id.getValue()));
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Returns the result of the neural networks model based prediction
     * @param  id   Identifier of the result
     * @return Result that corresponds to the given identifier
     */
    public KeyValueDataCollection get(PredictionResultCollectionId id) {
        if (id == PredictionResultCollectionId.predictionCollection) {
            return new KeyValueDataCollection(getContext(), cGetValue(cObject, id.getValue()));
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Returns the result of the neural networks model based prediction
     * @param  id    Identifier of the result
     * @param  key   Key to use to retrieve data
     * @return Result that corresponds to the given identifier
     */
    public Tensor get(PredictionResultCollectionId id, int key) {
        if (id == PredictionResultCollectionId.predictionCollection) {
            return (Tensor)Factory.instance().createObject(getContext(), cGetTensor(this.cObject, id.getValue(), key));
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Sets the result of neural networks model based prediction
     * @param id   Identifier of the result
     * @param val  Result that corresponds to the given identifier
     */
    public void set(PredictionResultId id, Tensor val) {
        if (id == PredictionResultId.prediction) {
            cSetValue(cObject, id.getValue(), val.getCObject());
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Sets the result of neural networks model based prediction
     * @param id   Identifier of the result
     * @param val  Result that corresponds to the given identifier
     */
    public void set(PredictionResultCollectionId id, Tensor val) {
        if (id == PredictionResultCollectionId.predictionCollection) {
            cSetValue(cObject, id.getValue(), val.getCObject());
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Sets the result of neural networks model based prediction
     * @param id    Identifier of the input object
     * @param key   Key to use to retrieve data
     * @param val   Value of the input object
     */
    public void add(PredictionResultCollectionId id, int key, Tensor val) {
        if (id == PredictionResultCollectionId.predictionCollection) {
            cAddTensor(this.cObject, id.getValue(), key, val.getCObject());
        }
        else {
            throw new IllegalArgumentException("Incorrect TrainingInputId");
        }
    }

    private native long cGetValue(long cObject, int id);
    private native void cSetValue(long cObject, int id, long ntAddr);

    private native void cAddTensor(long cObject, int id, int key, long ntAddr);
    private native long cGetTensor(long cObject, int id, int key);
}
/** @} */
