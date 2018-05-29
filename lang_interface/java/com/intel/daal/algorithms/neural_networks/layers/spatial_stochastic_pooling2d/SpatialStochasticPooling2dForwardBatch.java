/* file: SpatialStochasticPooling2dForwardBatch.java */
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
 * @defgroup spatial_stochastic_pooling2d_forward_batch Batch
 * @ingroup spatial_stochastic_pooling2d_forward
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers.spatial_stochastic_pooling2d;

import com.intel.daal.algorithms.neural_networks.layers.spatial_pooling2d.SpatialPooling2dIndices;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.algorithms.ComputeMode;
import com.intel.daal.algorithms.AnalysisBatch;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__SPATIAL_STOCHASTIC_POOLING2D__SPATIALSTOCHASTICPOOLING2DFORWARDBATCH"></a>
 * @brief Class that computes the results of the forward two-dimensional spatial stochastic pooling layer in the batch processing mode
 * <!-- \n<a href="DAAL-REF-STOCHASTICPOOLING2DFORWARD-ALGORITHM">Forward two-dimensional spatial stochastic pooling layer description and usage models</a> -->
 *
 * @par References
 *      - @ref SpatialStochasticPooling2dLayerDataId class
 */
public class SpatialStochasticPooling2dForwardBatch extends com.intel.daal.algorithms.neural_networks.layers.ForwardLayer {
    public  SpatialStochasticPooling2dForwardInput input;     /*!< %Input data */
    public  SpatialStochasticPooling2dMethod       method;    /*!< Computation method for the layer */
    public  SpatialStochasticPooling2dParameter    parameter; /*!< SpatialStochasticPooling2dParameters of the layer */
    private Precision    prec;      /*!< Data type to use in intermediate computations for the layer */

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the forward two-dimensional spatial stochastic pooling layer by copying input objects of
     * another forward two-dimensional spatial stochastic pooling layer
     * @param context    Context to manage the forward two-dimensional spatial stochastic pooling layer
     * @param other      A forward two-dimensional spatial stochastic pooling layer to be used as the source to
     *                   initialize the input objects of the forward two-dimensional spatial stochastic pooling layer
     */
    public SpatialStochasticPooling2dForwardBatch(DaalContext context, SpatialStochasticPooling2dForwardBatch other) {
        super(context);
        this.method = other.method;
        prec = other.prec;

        this.cObject = cClone(other.cObject, prec.getValue(), method.getValue());
        input = new SpatialStochasticPooling2dForwardInput(context, cGetInput(cObject, prec.getValue(), method.getValue()));
        parameter = new SpatialStochasticPooling2dParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Constructs the forward two-dimensional spatial stochastic pooling layer
     * @param context       Context to manage the layer
     * @param cls           Data type to use in intermediate computations for the layer, Double.class or Float.class
     * @param method        The layer computation method, @ref SpatialStochasticPooling2dMethod
     * @param pyramidHeight The value of pyramid height
     * @param nDim          Number of dimensions in input data
     */
    public SpatialStochasticPooling2dForwardBatch(DaalContext context, Class<? extends Number> cls, SpatialStochasticPooling2dMethod method, long pyramidHeight, long nDim) {
        super(context);

        this.method = method;

        if (method != SpatialStochasticPooling2dMethod.defaultDense) {
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

        this.cObject = cInit(prec.getValue(), method.getValue(), pyramidHeight, nDim);
        input = new SpatialStochasticPooling2dForwardInput(context, cGetInput(cObject, prec.getValue(), method.getValue()));
        parameter = new SpatialStochasticPooling2dParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
    }

    SpatialStochasticPooling2dForwardBatch(DaalContext context, Class<? extends Number> cls, SpatialStochasticPooling2dMethod method, long cObject, long pyramidHeight, long nDim) {
        super(context);

        this.method = method;

        if (method != SpatialStochasticPooling2dMethod.defaultDense) {
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
        input = new SpatialStochasticPooling2dForwardInput(context, cGetInput(cObject, prec.getValue(), method.getValue()));
        parameter = new SpatialStochasticPooling2dParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
        SpatialPooling2dIndices sd = new SpatialPooling2dIndices(nDim - 2, nDim - 1);
        parameter.setIndices(sd);
    }

    /**
     * Computes the result of the forward two-dimensional spatial stochastic pooling layer
     * @return  Forward two-dimensional spatial stochastic pooling layer result
     */
    @Override
    public SpatialStochasticPooling2dForwardResult compute() {
        super.compute();
        SpatialStochasticPooling2dForwardResult result = new SpatialStochasticPooling2dForwardResult(getContext(), cGetResult(cObject, prec.getValue(), method.getValue()));
        return result;
    }

    /**
     * Registers user-allocated memory to store the result of the forward two-dimensional spatial stochastic pooling layer
     * @param result    Structure to store the result of the forward two-dimensional spatial stochastic pooling layer
     */
    public void setResult(SpatialStochasticPooling2dForwardResult result) {
        cSetResult(cObject, prec.getValue(), method.getValue(), result.getCObject());
    }

    /**
     * Returns the structure that contains result of the forward layer
     * @return Structure that contains result of the forward layer
     */
    @Override
    public SpatialStochasticPooling2dForwardResult getLayerResult() {
        return new SpatialStochasticPooling2dForwardResult(getContext(), cGetResult(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Returns the structure that contains input object of the forward layer
     * @return Structure that contains input object of the forward layer
     */
    @Override
    public SpatialStochasticPooling2dForwardInput getLayerInput() {
        return input;
    }

    /**
     * Returns the structure that contains parameters of the forward layer
     * @return Structure that contains parameters of the forward layer
     */
    @Override
    public SpatialStochasticPooling2dParameter getLayerParameter() {
        return parameter;
    }

    /**
     * Returns the newly allocated forward two-dimensional spatial stochastic pooling layer
     * with a copy of input objects of this forward two-dimensional spatial stochastic pooling layer
     * @param context    Context to manage the layer
     *
     * @return The newly allocated forward two-dimensional spatial stochastic pooling layer
     */
    @Override
    public SpatialStochasticPooling2dForwardBatch clone(DaalContext context) {
        return new SpatialStochasticPooling2dForwardBatch(context, this);
    }

    private native long cInit(int prec, int method, long pyramidHeight, long nDim);
    private native long cInitParameter(long cAlgorithm, int prec, int method);
    private native long cGetInput(long cAlgorithm, int prec, int method);
    private native long cGetResult(long cAlgorithm, int prec, int method);
    private native void cSetResult(long cAlgorithm, int prec, int method, long cObject);
    private native long cClone(long algAddr, int prec, int method);
}
/** @} */
