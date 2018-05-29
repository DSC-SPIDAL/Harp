/* file: Convolution2dParameter.java */
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
 * @ingroup convolution2d
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers.convolution2d;

import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__CONVOLUTION2D__CONVOLUTION2DPARAMETER"></a>
 * \brief Class that specifies parameters of the 2D convolution layer
 */
public class Convolution2dParameter extends com.intel.daal.algorithms.neural_networks.layers.Parameter {
    /**
     * Constructs the parameter of the 2D convolution layer
     * @param context Context to manage the parameter of the 2D convolution layer
     */
    public Convolution2dParameter(DaalContext context) {
        super(context);
        cObject = cInit();
    }
    /**
     * Constructs parameter from C++ parameter
     * @param context Context to manage the parameter
     * @param cObject Address of C++ parameter
     */
    public Convolution2dParameter(DaalContext context, long cObject) {
        super(context, cObject);
    }

    /**
     *  Gets the index of the dimension for which the grouping is applied
     * @return Convolution2dIndex of the dimension for which the grouping is applied
     */
    public long getGroupDimension() {
        return cGetGroupDimension(cObject);
    }

    /**
     *  Sets the index of the dimension for which the grouping is applied
     *  @param groupDimension   Dimension for which the grouping is applied
     */
    public void setGroupDimension(long groupDimension) {
        cSetGroupDimension(cObject, groupDimension);
    }

    /**
    *  Gets the number of kernels applied to the input layer data
    * @return Number of kernels applied to the input layer data
    */
    public long getNKernels() {
        return cGetNKernels(cObject);
    }

    /**
     *  Sets the number of kernels applied to the input layer data
     *  @param nKernels   The number of kernels applied to the input layer data
     */
    public void setNKernels(long nKernels) {
        cSetNKernels(cObject, nKernels);
    }

    /**
     *  Gets the number of groups which the input data is split in groupDimension dimension
     * @return Number of groups which the input data is split in groupDimension dimension
     */
    public long getNGroups() {
        return cGetNGroups(cObject);
    }

    /**
     *  Sets the number of groups which the input data is split in groupDimension dimension
     *  @param nGroups   The number of groups which the input data is split in groupDimension dimension
     */
    public void setNGroups(long nGroups) {
        cSetNGroups(cObject, nGroups);
    }

    /**
     * Gets the data structure representing the sizes of the two-dimensional kernel subtensor
     * @return Data structure representing the sizes of the two-dimensional kernel subtensor
     */
    public Convolution2dKernelSize getKernelSize() {
        long[] size = cGetKernelSize(cObject);
        return new Convolution2dKernelSize(size[0], size[1]);
    }

    /**
     * Sets the data structure representing the sizes of the two-dimensional kernel subtensor
     * @param kernelSize   The data structure representing the sizes of the two-dimensional kernel subtensor
     */
    public void setKernelSize(Convolution2dKernelSize kernelSize) {
        long[] size = kernelSize.getSize();
        cSetKernelSize(cObject, size[0], size[1]);
    }

    /**
     * Gets the data structure representing the intervals on which the kernel should be applied to the input
     * @return Data structure representing the intervals on which the kernel should be applied to the input
     */
    public Convolution2dStride getStride() {
        long[] size = cGetStride(cObject);
        return new Convolution2dStride(size[0], size[1]);
    }

    /**
     *  Sets the data structure representing the intervals on which the kernel should be applied to the input
     *  @param str   The data structure representing the intervals on which the kernel should be applied to the input
     */
    public void setStride(Convolution2dStride str) {
        long[] size = str.getSize();
        cSetStride(cObject, size[0], size[1]);
    }

    /**
     * Gets the data structure representing the number of data elements to implicitly add
     * to each side of the 2D subtensor on which convolution is performed
     * @return Data structure representing the number of data elements to implicitly add
     * to each side of the 2D subtensor on which convolution is performed
     */
    public Convolution2dPadding getPadding() {
        long[] size = cGetPadding(cObject);
        return new Convolution2dPadding(size[0], size[1]);
    }

    /**
     * Sets the data structure representing the number of data elements to implicitly add
     * to each side of the 2D subtensor on which convolution is performed
     * @param padding   The data structure representing the number of data elements to implicitly add
     * to each side of the 2D subtensor on which convolution is performed
     */
    public void setPadding(Convolution2dPadding padding) {
        long[] size = padding.getSize();
        cSetPadding(cObject, size[0], size[1]);
    }

    /**
     * Gets the data structure representing the dimension for convolution kernels
     * @return Data structure representing the dimension for convolution kernels
     */
    public Convolution2dIndices getIndices() {
        long[] size = cGetIndices(cObject);
        return new Convolution2dIndices(size[0], size[1]);
    }

    /**
     * Sets the data structure representing the dimension for convolution kernels
     * @param sd   The data structure representing the dimension for convolution kernels
     */
    public void setIndices(Convolution2dIndices sd) {
        long[] size = sd.getSize();
        cSetIndices(cObject, size[0], size[1]);
    }

    private native long cInit();
    private native long cGetGroupDimension(long cObject);
    private native void cSetGroupDimension(long cObject, long groupDimension);
    private native long cGetNKernels(long cObject);
    private native void cSetNKernels(long cObject, long nKernels);
    private native long cGetNGroups(long cObject);
    private native void cSetNGroups(long cObject, long nGroups);
    private native void cSetKernelSize(long cObject, long first, long second);
    private native void cSetStride(long cObject, long first, long second);
    private native void cSetPadding(long cObject, long first, long second);
    private native void cSetIndices(long cObject, long first, long second);
    private native long[] cGetKernelSize(long cObject);
    private native long[] cGetStride(long cObject);
    private native long[] cGetPadding(long cObject);
    private native long[] cGetIndices(long cObject);
}
/** @} */
