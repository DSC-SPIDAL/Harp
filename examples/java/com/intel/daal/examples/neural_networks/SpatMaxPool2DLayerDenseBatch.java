/* file: SpatMaxPool2DLayerDenseBatch.java */
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

/*
 //  Content:
 //  Java example of neural network forward and backward two-dimensional maximum pooling layers usage
 ////////////////////////////////////////////////////////////////////////////////
 */

package com.intel.daal.examples.neural_networks;

import com.intel.daal.algorithms.neural_networks.layers.spatial_maximum_pooling2d.*;
import com.intel.daal.algorithms.neural_networks.layers.ForwardResultId;
import com.intel.daal.algorithms.neural_networks.layers.ForwardResultLayerDataId;
import com.intel.daal.algorithms.neural_networks.layers.ForwardInputId;
import com.intel.daal.algorithms.neural_networks.layers.BackwardResultId;
import com.intel.daal.algorithms.neural_networks.layers.BackwardInputId;
import com.intel.daal.algorithms.neural_networks.layers.BackwardInputLayerDataId;
import com.intel.daal.data_management.data.Tensor;
import com.intel.daal.data_management.data.HomogenTensor;
import com.intel.daal.examples.utils.Service;
import com.intel.daal.services.DaalContext;

import com.intel.daal.data_management.data.NumericTable;
/**
 * <a name="DAAL-EXAMPLE-JAVA-SPATMAXPOOL2DLAYERDENSEBATCH">
 * @example SpatMaxPool2DLayerDenseBatch.java
 */
class SpatMaxPool2DLayerDenseBatch {
    private static final String datasetFileName = "../data/batch/layer.csv";
    private static DaalContext context = new DaalContext();
    private static long pyramidHeight = 2;

    public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException {
        /* Read datasetFileName from a file and create a tensor to store forward input data */
        /* Create a collection of dimension sizes of input data */
        long[] dimensionSizes = new long[4];
        dimensionSizes[0] = 2;
        dimensionSizes[1] = 3;
        dimensionSizes[2] = 2;
        dimensionSizes[3] = 4;

        /* Create input data tensor */
        float[] data = {1,  2,  3,  4,
                        5,  6,  7,  8,
                                    9, 10, 11, 12,
                                    13, 14, 15, 16,
                                                17, 18, 19, 20,
                                                21, 22, 23, 24,
                                    -1, -2, -3, -4,
                                    -5, -6, -7, -8,
                                                -9, -10, -11, -12,
                                                -13, -14, -15, -16,
                                                -17, -18, -19, -20,
                                                -21, -22, -23, -24};
        Tensor dataTensor = new HomogenTensor(context, dimensionSizes, data);

        long nDim = dataTensor.getDimensions().length;

        /* Print the input of the forward two-dimensional pooling */
        Service.printTensor("Forward two-dimensional spatial pyramid maximum pooling layer input (first 10 rows):", dataTensor, 10, 0);

        /* Create an algorithm to compute forward two-dimensional pooling results using default method */
        SpatialMaximumPooling2dForwardBatch spatialMaxPooling2DLayerForward = new SpatialMaximumPooling2dForwardBatch(context, Float.class,
                                                                                                                      SpatialMaximumPooling2dMethod.defaultDense,
                                                                                                                      pyramidHeight, nDim);

        /* Set input objects for the forward two-dimensional pooling */
        spatialMaxPooling2DLayerForward.input.set(ForwardInputId.data, dataTensor);

        /* Compute forward two-dimensional pooling results */
        SpatialMaximumPooling2dForwardResult forwardResult = spatialMaxPooling2DLayerForward.compute();

        /* Print the results of the forward two-dimensional pooling */
        Service.printTensor("Forward two-dimensional spatial pyramid maximum pooling layer result (first 5 rows):", forwardResult.get(ForwardResultId.value), 5, 0);
        Service.printTensor("Forward two-dimensional spatial pyramid maximum pooling layer selected indices (first 10 rows):",
                            forwardResult.get(SpatialMaximumPooling2dLayerDataId.auxSelectedIndices), 10, 0);

        /* Create an algorithm to compute backward two-dimensional pooling results using default method */
        SpatialMaximumPooling2dBackwardBatch spatialMaxPooling2DLayerBackward = new SpatialMaximumPooling2dBackwardBatch(context, Float.class,
                                                                                                                         SpatialMaximumPooling2dMethod.defaultDense,
                                                                                                                         pyramidHeight, nDim);

        /* Set input objects for the backward two-dimensional pooling */
        spatialMaxPooling2DLayerBackward.input.set(BackwardInputId.inputGradient, forwardResult.get(ForwardResultId.value));
        spatialMaxPooling2DLayerBackward.input.set(BackwardInputLayerDataId.inputFromForward,
                forwardResult.get(ForwardResultLayerDataId.resultForBackward));

        /* Compute backward two-dimensional pooling results */
        SpatialMaximumPooling2dBackwardResult backwardResult = spatialMaxPooling2DLayerBackward.compute();

        /* Print the results of the backward two-dimensional pooling */
        Service.printTensor("Backward two-dimensional spatial pyramid maximum pooling layer result (first 10 rows):", backwardResult.get(BackwardResultId.gradient), 10, 0);

        context.dispose();
    }
}
