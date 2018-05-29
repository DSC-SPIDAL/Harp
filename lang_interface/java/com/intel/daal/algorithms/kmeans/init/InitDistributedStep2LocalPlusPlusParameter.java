/* file: InitDistributedStep2LocalPlusPlusParameter.java */
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
 * @ingroup kmeans_init
 * @{
 */
package com.intel.daal.algorithms.kmeans.init;

import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__KMEANS__INIT__INITDISTRIBUTEDSTEP2LOCALPLUSPLUSPARAMETER"></a>
 * @brief Parameters for computing initial clusters on the step 2 on local nodes. Kmeans++ and || only.
 */
public class InitDistributedStep2LocalPlusPlusParameter extends InitParameter {

    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /** @private */
    public InitDistributedStep2LocalPlusPlusParameter(DaalContext context, long cParameter, long nClusters, boolean bFirstIteration) {
        super(context, nClusters);
        this.cObject = cParameter;
    }

    /**
     * Constructs a parameter
     * @param context           Context to manage the parameter for computing initial clusters for the K-Means algorithm
     * @param nClusters         Number of clusters
     * @param bFirstIteration   True if step2Local is called for the first time
     */
    public InitDistributedStep2LocalPlusPlusParameter(DaalContext context, long nClusters, boolean bFirstIteration) {
        super(context, nClusters);
        initialize(nClusters, bFirstIteration);
    }

    private void initialize(long nClusters, boolean bFirstIteration) {
        this.cObject = init(nClusters, bFirstIteration);
    }

    /**
     * Returns true if step2Local is called for the first time
     * @return Number of clusters
     */
    public boolean getIsFirstIteration() {
        return cGetIsFirstIteration(this.cObject);
    }

    /**
     * Kmeans|| only. Returns true if the last iteration of parallelPlus algorithm processing is performed
     *         and  the output for the 5th step is required
     * @return Total number of rows
     */
    public boolean getOutputForStep5Required() {
        return cGetOutputForStep5Required(this.cObject);
    }

    /**
    * Sets the firstIteration flag
    * @param bFirstIteration firstIteration flag
    */
    public void setIsFirstIteration(boolean bFirstIteration) {
        cSetIsFirstIteration(this.cObject, bFirstIteration);
    }

    /**
    * Sets the outputForStep5Required flag
    * @param bRequired outputForStep5Required flag
    */
    public void setOutputForStep5Required(boolean bRequired) {
        cSetOutputForStep5Required(this.cObject, bRequired);
    }

    private native long init(long nClusters, boolean bFirstIteration);

    private native boolean cGetIsFirstIteration(long parameterAddress);

    private native boolean cGetOutputForStep5Required(long parameterAddress);

    private native void cSetIsFirstIteration(long parameterAddress, boolean bFirstIteration);

    private native void cSetOutputForStep5Required(long parameterAddress, boolean bRequired);
}
/** @} */
