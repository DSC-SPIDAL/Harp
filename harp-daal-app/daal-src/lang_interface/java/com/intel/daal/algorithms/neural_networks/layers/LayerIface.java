/* file: LayerIface.java */
/*******************************************************************************
* Copyright 2014-2016 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

package com.intel.daal.algorithms.neural_networks.layers;

import com.intel.daal.services.ContextClient;
import com.intel.daal.services.DaalContext;
import com.intel.daal.data_management.data.SerializableBase;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__LAYERIFACE"></a>
 * \brief Abstract class that specifies the interface of layer
 */
public abstract class LayerIface extends ContextClient {
    /**
     * @brief Pointer to C++ implementation of the layer
     */
    public long cObject;

    public  ForwardLayer  forwardLayer;  /*!< Forward stage of the layer algorithm */
    public  BackwardLayer backwardLayer; /*!< Backward stage of the layer algorithm */

    protected LayerIface(DaalContext context) {
        super(context);
    }

    /**
     * Releases memory allocated for the layer of the neural network
     */
    @Override
    public void dispose() {
        if (this.cObject != 0) {
            cDispose(this.cObject);
            this.cObject = 0;
        }
    }

    private native void cDispose(long cObject);
}
