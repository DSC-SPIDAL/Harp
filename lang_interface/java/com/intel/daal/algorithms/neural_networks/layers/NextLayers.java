/* file: NextLayers.java */
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
 * @ingroup layers
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers;

import com.intel.daal.services.ContextClient;
import com.intel.daal.services.DaalContext;
import com.intel.daal.data_management.data.SerializableBase;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__NEXTLAYERS"></a>
 * \brief Contains list of layer indices of layers following the current layer
 */
public class NextLayers extends ContextClient {
    /**
     * @brief Pointer to C++ implementation of the next layers list
     */
    public long cObject;

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     */
    public NextLayers(DaalContext context) {
        super(context);
        cObject = cInit();
    }

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     * @param index1    First index of the next layer
     */
    public NextLayers(DaalContext context, long index1) {
        super(context);
        cObject = cInit(index1);
    }

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     * @param index1    First index of the next layer
     * @param index2    Second index of the next layer
     */
    public NextLayers(DaalContext context, long index1, long index2) {
        super(context);
        cObject = cInit(index1, index2);
    }

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     * @param index1    First index of the next layer
     * @param index2    Second index of the next layer
     * @param index3    Third index of the next layer
     */
    public NextLayers(DaalContext context, long index1, long index2, long index3) {
        super(context);
        cObject = cInit(index1, index2, index3);
    }

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     * @param index1    First index of the next layer
     * @param index2    Second index of the next layer
     * @param index3    Third index of the next layer
     * @param index4    Fourth index of the next layer
     */
    public NextLayers(DaalContext context, long index1, long index2, long index3, long index4) {
        super(context);
        cObject = cInit(index1, index2, index3, index4);
    }

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     * @param index1    First index of the next layer
     * @param index2    Second index of the next layer
     * @param index3    Third index of the next layer
     * @param index4    Fourth index of the next layer
     * @param index5    Fifth index of the next layer
     */
    public NextLayers(DaalContext context, long index1, long index2, long index3, long index4, long index5) {
        super(context);
        cObject = cInit(index1, index2, index3, index4, index5);
    }

    /**
     * Constructs list of layer indices of layers following the current layer
     * @param context   Context to manage the list of layer indices
     * @param index1    First index of the next layer
     * @param index2    Second index of the next layer
     * @param index3    Third index of the next layer
     * @param index4    Fourth index of the next layer
     * @param index5    Fifth index of the next layer
     * @param index6    Sixth index of the next layer
     */
    public NextLayers(DaalContext context, long index1, long index2, long index3, long index4, long index5, long index6) {
        super(context);
        cObject = cInit(index1, index2, index3, index4, index5, index6);
    }

    /**
     * Releases memory allocated for the native next layers list object
     */
    @Override
    public void dispose() {
        if (this.cObject != 0) {
            cDispose(this.cObject);
            this.cObject = 0;
        }
    }

    private native long cInit();
    private native long cInit(long index1);
    private native long cInit(long index1, long index2);
    private native long cInit(long index1, long index2, long index3);
    private native long cInit(long index1, long index2, long index3, long index4);
    private native long cInit(long index1, long index2, long index3, long index4, long index5);
    private native long cInit(long index1, long index2, long index3, long index4, long index5, long index6);
    private native void cDispose(long cObject);
}
/** @} */
