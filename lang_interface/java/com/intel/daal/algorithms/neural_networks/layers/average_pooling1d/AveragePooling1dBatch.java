/* file: AveragePooling1dBatch.java */
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
 * @defgroup average_pooling1d One-dimensional Average Pooling Layer
 * @brief Contains classes for average one-dimensional (1D) pooling layer
 * @ingroup pooling1d
 * @{
 */
/**
 * @brief Contains classes of the one-dimensional (1D) average pooling layer
 */
package com.intel.daal.algorithms.neural_networks.layers.average_pooling1d;

import com.intel.daal.algorithms.neural_networks.layers.ForwardLayer;
import com.intel.daal.algorithms.neural_networks.layers.BackwardLayer;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.algorithms.ComputeMode;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__AVERAGE_POOLING1D__AVERAGEPOOLING1DBATCH"></a>
 * @brief Provides methods for the one-dimensional average pooling layer in the batch processing mode
 * <!-- \n<a href="DAAL-REF-AVERAGEPOOLING1DFORWARD-ALGORITHM">Forward one-dimensional average pooling layer description and usage models</a> -->
 * <!-- \n<a href="DAAL-REF-AVERAGEPOOLING1DBACKWARD-ALGORITHM">Backward one-dimensional average pooling layer description and usage models</a> -->
 *
 * @par References
 *      - @ref AveragePooling1dForwardBatch class
 *      - @ref AveragePooling1dBackwardBatch class
 */
public class AveragePooling1dBatch extends com.intel.daal.algorithms.neural_networks.layers.LayerIface {
    public    AveragePooling1dMethod        method;      /*!< Computation method for the layer */
    public    AveragePooling1dParameter     parameter;   /*!< Pooling layer parameters */
    protected Precision     prec;        /*!< Data type to use in intermediate computations for the layer */

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the one-dimensional average pooling layer
     * @param context    Context to manage the one-dimensional average pooling layer
     * @param cls        Data type to use in intermediate computations for the layer, Double.class or Float.class
     * @param method     The layer computation method, @ref AveragePooling1dMethod
     * @param nDim       Number of dimensions in input data
     */
    public AveragePooling1dBatch(DaalContext context, Class<? extends Number> cls, AveragePooling1dMethod method, long nDim) {
        super(context);

        this.method = method;

        if (method != AveragePooling1dMethod.defaultDense) {
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

        this.cObject = cInit(prec.getValue(), method.getValue(), nDim);
        parameter = new AveragePooling1dParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));

        forwardLayer = (ForwardLayer)(new AveragePooling1dForwardBatch(context, cls, method, cGetForwardLayer(cObject, prec.getValue(), method.getValue()), nDim));
        backwardLayer = (BackwardLayer)(new AveragePooling1dBackwardBatch(context, cls, method, cGetBackwardLayer(cObject, prec.getValue(),
                                                                                                  method.getValue()), nDim));
    }

    private native long cInit(int prec, int method, long nDim);
    private native long cInitParameter(long cAlgorithm, int prec, int method);
    private native long cGetForwardLayer(long cAlgorithm, int prec, int method);
    private native long cGetBackwardLayer(long cAlgorithm, int prec, int method);
}
/** @} */
