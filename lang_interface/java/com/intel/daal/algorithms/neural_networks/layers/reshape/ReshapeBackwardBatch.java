/* file: ReshapeBackwardBatch.java */
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
 * @defgroup reshape_layers_backward_batch Batch
 * @ingroup reshape_layers_backward
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers.reshape;

import com.intel.daal.algorithms.neural_networks.layers.Parameter;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.algorithms.ComputeMode;
import com.intel.daal.algorithms.AnalysisBatch;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__RESHAPE__RESHAPEBACKWARDBATCH"></a>
 * \brief Class that computes the results of the backward reshape layer in the batch processing mode
 * <!-- \n<a href="DAAL-REF-RESHAPEBACKWARD">Backward reshape layer description and usage models</a> -->
 *
 * \par References
 *      - @ref ReshapeLayerDataId class
 */
public class ReshapeBackwardBatch extends com.intel.daal.algorithms.neural_networks.layers.BackwardLayer {
    public  ReshapeBackwardInput input;    /*!< %Input data */
    public  ReshapeMethod        method;   /*!< Computation method for the layer */
    public  ReshapeParameter    parameter; /*!< Parameters of the layer */
    private Precision     prec;     /*!< Data type to use in intermediate computations for the layer */

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the backward reshape layer by copying input objects of backward reshape layer
     * @param context    Context to manage the backward reshape layer
     * @param other      A backward reshape layer to be used as the source to initialize the input objects of the backward reshape layer
     */
    public ReshapeBackwardBatch(DaalContext context, ReshapeBackwardBatch other) {
        super(context);
        this.method = other.method;
        prec = other.prec;

        this.cObject = cClone(other.cObject, prec.getValue(), method.getValue());
        input = new ReshapeBackwardInput(context, cGetInput(cObject, prec.getValue(), method.getValue()));
        parameter = new ReshapeParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Constructs the backward reshape layer
     * @param context    Context to manage the backward reshape layer
     * @param cls        Data type to use in intermediate computations for the layer, Double.class or Float.class
     * @param method     The layer computation method, @ref ReshapeMethod
     */
    public ReshapeBackwardBatch(DaalContext context, Class<? extends Number> cls, ReshapeMethod method) {
        super(context);

        this.method = method;

        if (method != ReshapeMethod.defaultDense) {
            throw new IllegalArgumentException("method unsupported");
        }
        if (cls != Double.class && cls != Float.class) {
            throw new IllegalArgumentException("type unsupported");
        }

        if (cls == Double.class) {
            prec = Precision.doublePrecision;
        }
        else {
            prec = Precision.singlePrecision;
        }

        this.cObject = cInit(prec.getValue(), method.getValue());
        input = new ReshapeBackwardInput(context, cGetInput(cObject, prec.getValue(), method.getValue()));
        parameter = new ReshapeParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
    }

    ReshapeBackwardBatch(DaalContext context, Class<? extends Number> cls, ReshapeMethod method, long cObject) {
        super(context);

        this.method = method;

        if (method != ReshapeMethod.defaultDense) {
            throw new IllegalArgumentException("method unsupported");
        }
        if (cls != Double.class && cls != Float.class) {
            throw new IllegalArgumentException("type unsupported");
        }

        if (cls == Double.class) {
            prec = Precision.doublePrecision;
        }
        else {
            prec = Precision.singlePrecision;
        }

        this.cObject = cObject;
        input = new ReshapeBackwardInput(context, cGetInput(cObject, prec.getValue(), method.getValue()));
        parameter = new ReshapeParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Computes the result of the backward reshape layer
     * @return  Backward reshape layer result
     */
    @Override
    public ReshapeBackwardResult compute() {
        super.compute();
        ReshapeBackwardResult result = new ReshapeBackwardResult(getContext(), cGetResult(cObject, prec.getValue(), method.getValue()));
        return result;
    }

    /**
     * Registers user-allocated memory to store the result of the backward reshape layer
     * @param result    Structure to store the result of the backward reshape layer
     */
    public void setResult(ReshapeBackwardResult result) {
        cSetResult(cObject, prec.getValue(), method.getValue(), result.getCObject());
    }

    /**
     * Returns the structure that contains result of the backward layer
     * @return Structure that contains result of the backward layer
     */
    @Override
    public ReshapeBackwardResult getLayerResult() {
        return new ReshapeBackwardResult(getContext(), cGetResult(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Returns the structure that contains input object of the backward layer
     * @return Structure that contains input object of the backward layer
     */
    @Override
    public ReshapeBackwardInput getLayerInput() {
        return input;
    }

    /**
     * Returns the structure that contains parameters of the backward layer
     * @return Structure that contains parameters of the backward layer
     */
    @Override
    public Parameter getLayerParameter() {
        return null;
    }

    /**
     * Returns the newly allocated backward reshape layer
     * with a copy of input objects of this backward reshape layer
     * @param context    Context to manage the layer
     *
     * @return The newly allocated backward reshape layer
     */
    @Override
    public ReshapeBackwardBatch clone(DaalContext context) {
        return new ReshapeBackwardBatch(context, this);
    }

    private native long cInit(int prec, int method);
    private native long cInitParameter(long cAlgorithm, int prec, int method);
    private native long cGetInput(long cAlgorithm, int prec, int method);
    private native long cGetResult(long cAlgorithm, int prec, int method);
    private native void cSetResult(long cAlgorithm, int prec, int method, long cObject);
    private native long cClone(long algAddr, int prec, int method);
}
/** @} */
