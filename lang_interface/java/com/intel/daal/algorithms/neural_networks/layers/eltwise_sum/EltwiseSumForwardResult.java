/* file: EltwiseSumForwardResult.java */
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
 * @ingroup eltwise_sum_forward
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers.eltwise_sum;

import com.intel.daal.data_management.data.Factory;
import com.intel.daal.data_management.data.Tensor;
import com.intel.daal.data_management.data.Factory;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__ELTWISE_SUM__ELTWISESUMFORWARDRESULT"></a>
 * @brief Class that provides methods to access the result obtained with the compute() method of the forward element-wise sum layer
 */
public final class EltwiseSumForwardResult extends com.intel.daal.algorithms.neural_networks.layers.ForwardResult {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the forward element-wise sum layer result
     * @param context   Context to manage the forward element-wise sum layer result
     */
    public EltwiseSumForwardResult(DaalContext context) {
        super(context);
        this.cObject = cNewResult();
    }

    public EltwiseSumForwardResult(DaalContext context, long cObject) {
        super(context, cObject);
    }

    /**
     * Returns the result tensor of forward element-wise sum layer
     * @param  id Identifier of the result tensor
     * @return    Result tensor that corresponds to the given identifier
     */
    public Tensor get(EltwiseSumLayerDataId id) {
        if (id == EltwiseSumLayerDataId.auxCoefficients) {
            return (Tensor)Factory.instance().createObject(getContext(), cGetTensor(cObject, id.getValue()));
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Returns the result numeric table of the forward element-wise sum layer
     * @param  id Identifier of the result numeric table
     * @return    Result numeric table that corresponds to the given identifier
     */
    public NumericTable get(EltwiseSumLayerDataNumericTableId id) {
        if (id == EltwiseSumLayerDataNumericTableId.auxNumberOfCoefficients) {
            return (NumericTable)Factory.instance().createObject(getContext(), cGetNumericTable(cObject, id.getValue()));
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Sets the result tensor of forward element-wise sum layer
     * @param id    Identifier of the result tensor
     * @param value Result tensor
     */
    public void set(EltwiseSumLayerDataId id, Tensor value) {
        if (id == EltwiseSumLayerDataId.auxCoefficients) {
            cSetTensor(cObject, id.getValue(), value.getCObject());
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Sets the result numeric table of the forward element-wise sum layer
     * @param id    Identifier of the result numeric table
     * @param value Result numeric tensor
     */
    public void set(EltwiseSumLayerDataNumericTableId id, NumericTable value) {
        if (id == EltwiseSumLayerDataNumericTableId.auxNumberOfCoefficients) {
            cSetNumericTable(cObject, id.getValue(), value.getCObject());
        }
        else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    private native long cNewResult();

    private native long cGetTensor(long cObject, int id);
    private native long cGetNumericTable(long cObject, int id);

    private native void cSetTensor(long cObject, int id, long tensorAddr);
    private native void cSetNumericTable(long cObject, int id, long ntAddr);
}
/** @} */
