/* file: GaussianParameter.java */
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
 * @ingroup initializers_gaussian
 * @{
 */
package com.intel.daal.algorithms.neural_networks.initializers.gaussian;

import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__INITIALIZERS__GAUSSIAN__GAUSSIANPARAMETER"></a>
 * @brief Class that specifies parameters of the neural network weights and biases gaussian initializer
 */
public class GaussianParameter extends com.intel.daal.algorithms.neural_networks.initializers.Parameter {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    public GaussianParameter(DaalContext context, long cObject) {
        super(context, cObject);
    }

    /**
     * Returns the distribution mean
     * @return  The distribution mean
     */
    public double getA() {
        return cGetA(cObject);
    }

    /**
     * Sets the distribution mean
     * @param a   The distribution mean
     */
    public void setA(double a) {
        cSetA(cObject, a);
    }

    /**
     * Returns the standard deviation of the distribution
     * @return  The standard deviation of the distribution
     */
    public double getSigma() {
        return cGetSigma(cObject);
    }

    /**
     * Sets the standard deviation of the distribution
     * @param sigma  The standard deviation of the distribution
     */
    public void setSigma(double sigma) {
        cSetSigma(cObject, sigma);
    }

    private native void cSetA(long cObject, double a);
    private native void cSetSigma(long cObject, double sigma);
    private native double cGetA(long cObject);
    private native double cGetSigma(long cObject);
}
/** @} */
