/* file: TruncatedGaussianBatch.java */
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
 * @defgroup initializers_truncated_gaussian_batch Batch
 * @ingroup initializers_truncated_gaussian
 * @{
 */
/**
 * @brief Contains classes for the truncated gaussian initializer
 */
package com.intel.daal.algorithms.neural_networks.initializers.truncated_gaussian;

import com.intel.daal.algorithms.neural_networks.initializers.Input;
import com.intel.daal.algorithms.neural_networks.initializers.Result;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__INITIALIZERS__TRUNCATED_GAUSSIAN__TRUNCATEDGAUSSIANBATCH"></a>
 * \brief Provides methods for truncated gaussian initializer computations in the batch processing mode
 *
 * \par References
 *      - @ref com.intel.daal.algorithms.neural_networks.initializers.Input class
 */
public class TruncatedGaussianBatch extends com.intel.daal.algorithms.neural_networks.initializers.InitializerIface {
    public  TruncatedGaussianParameter    parameter; /*!< Parameters of the truncated gaussian initializer */
    public  TruncatedGaussianMethod       method;    /*!< Computation method for the initializer */
    private Precision    prec;                       /*!< Data type to use in intermediate computations for the initializer */

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs truncated gaussian initializer by copying input objects and parameters of another truncated gaussian initializer
     * @param context Context to manage the truncated gaussian initializer
     * @param other   An initializer to be used as the source to initialize the input objects
     *                and parameters of this initializer
     */
    public TruncatedGaussianBatch(DaalContext context, TruncatedGaussianBatch other) {
        super(context);
        this.method = other.method;
        prec = other.prec;

        this.cObject = cClone(other.cObject, prec.getValue(), method.getValue());
        input = new Input(context, cGetInput(cObject));
        parameter = new TruncatedGaussianParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()), prec);
    }

    /**
     * Constructs the truncated gaussian initializer
     * @param context    Context to manage the initializer
     * @param cls        Data type to use in intermediate computations for the initializer, Double.class or Float.class
     * @param method     The initializer computation method, @ref TruncatedGaussianMethod
     * @param mean       The distribution mean
     * @param sigma      The standard deviation of the distribution
     */
    public TruncatedGaussianBatch(DaalContext context, Class<? extends Number> cls, TruncatedGaussianMethod method, double mean, double sigma) {
        super(context);
        construct(context, cls, method, mean, sigma);
    }

    /**
    * Constructs the truncated gaussian initializer
    * @param context    Context to manage the initializer
    * @param cls        Data type to use in intermediate computations for the initializer, Double.class or Float.class
    * @param method     The initializer computation method, @ref TruncatedGaussianMethod
    */
    public TruncatedGaussianBatch(DaalContext context, Class<? extends Number> cls, TruncatedGaussianMethod method) {
        super(context);

        double mean = 0.0;
        double sigma = 1.0;
        construct(context, cls, method, mean, sigma);
    }

    private void construct(DaalContext context, Class<? extends Number> cls, TruncatedGaussianMethod method, double mean, double sigma) {
        this.method = method;

        if (method != TruncatedGaussianMethod.defaultDense) {
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
        this.cObject = cInit(prec.getValue(), method.getValue(), mean, sigma);
        input = new Input(context, cGetInput(cObject));
        parameter = new TruncatedGaussianParameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()), prec);

        parameter.setMean(mean);
        parameter.setSigma(sigma);
    }

    /**
     * Computes the result of the truncated gaussian initializer
     * @return  TruncatedGaussian initializer result
     */
    @Override
    public Result compute() {
        super.compute();
        return new Result(getContext(), cGetResult(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Returns the newly allocated truncated gaussian initializer
     * with a copy of input objects and parameters of this truncated gaussian initializer
     * @param context    Context to manage the initializer
     * @return The newly allocated truncated gaussian initializer
     */
    @Override
    public TruncatedGaussianBatch clone(DaalContext context) {
        return new TruncatedGaussianBatch(context, this);
    }

    private native long cInit(int prec, int method, double mean, double sigma);
    private native long cInitParameter(long cAlgorithm, int prec, int method);
    private native long cGetResult(long cAlgorithm, int prec, int method);
    private native long cClone(long algAddr, int prec, int method);
}
/** @} */
