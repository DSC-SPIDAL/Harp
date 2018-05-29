/* file: Batch.java */
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
 * @defgroup distributions_normal_batch Batch
 * @ingroup distributions_normal
 * @{
 */
/**
 * @brief Contains classes for the normal distribution
 */
package com.intel.daal.algorithms.distributions.normal;

import com.intel.daal.algorithms.distributions.Input;
import com.intel.daal.algorithms.distributions.Result;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__DISTRIBUTIONS__NORMAL__BATCH"></a>
 * \brief Provides methods for normal distribution computations in the batch processing mode
 *
 * \par References
 *      - @ref com.intel.daal.algorithms.distributions.Input class
 */
public class Batch extends com.intel.daal.algorithms.distributions.BatchBase {
    public  Parameter    parameter; /*!< NormalParameters of the normal distribution */
    public  Method       method;    /*!< Computation method for the distribution */
    private Precision    prec;      /*!< Data type to use in intermediate computations for the distribution */

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs normal distribution by copying input objects and parameters of another normal distribution
     * @param context Context to manage the normal distribution
     * @param other   A distributions to be used as the source to initialize the input objects
     *                and parameters of this distribution
     */
    public Batch(DaalContext context, Batch other) {
        super(context);
        this.method = other.method;
        prec = other.prec;

        this.cObject = cClone(other.cObject, prec.getValue(), method.getValue());
        input = new Input(context, cGetInput(cObject));
        parameter = new Parameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Constructs the normal distribution
     * @param context    Context to manage the distribution
     * @param cls        Data type to use in intermediate computations for the distribution, Double.class or Float.class
     * @param method     The distribution computation method, @ref Method
     * @param a          Mean
     * @param sigma      Standard deviation
     */
    public Batch(DaalContext context, Class<? extends Number> cls, Method method, double a, double sigma) {
        super(context);
        constructBatch(context, cls, method, a, sigma);
    }

    private void constructBatch(DaalContext context, Class<? extends Number> cls, Method method, double a, double sigma) {
        this.method = method;

        if (method != Method.defaultDense && method != Method.icdf) {
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

        this.cObject = cInit(prec.getValue(), method.getValue(), a, sigma);
        input = new Input(context, cGetInput(cObject));
        parameter = new Parameter(context, cInitParameter(cObject, prec.getValue(), method.getValue()));
        parameter.setA(a);
        parameter.setSigma(sigma);
    }

    /**
     * Computes the result of the normal distribution
     * @return  Normal distribution result
     */
    @Override
    public Result compute() {
        super.compute();
        return new Result(getContext(), cGetResult(cObject, prec.getValue(), method.getValue()));
    }

    /**
     * Returns the newly allocated normal distribution
     * with a copy of input objects and parameters of this normal distribution
     * @param context    Context to manage the distribution
     * @return The newly allocated normal distribution
     */
    @Override
    public Batch clone(DaalContext context) {
        return new Batch(context, this);
    }

    private native long cInit(int prec, int method, double a, double sigma);
    private native long cInitParameter(long cAlgorithm, int prec, int method);
    private native long cGetResult(long cAlgorithm, int prec, int method);
    private native long cClone(long algAddr, int prec, int method);
}
/** @} */
