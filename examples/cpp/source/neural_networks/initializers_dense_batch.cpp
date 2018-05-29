/* file: initializers_dense_batch.cpp */
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
!  Content:
!    C++ example of initializers
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-INITIALIZERS_DENSE_BATCH"></a>
 * \example initializers_dense_batch.cpp
 */

#include "daal.h"
#include "service.h"

using namespace daal;
using namespace daal::algorithms;
using namespace daal::algorithms::neural_networks;
using namespace daal::algorithms::neural_networks::layers;
using namespace daal::data_management;
using namespace daal::services;

int main(int argc, char *argv[])
{
    /* Create collection of dimension sizes of the input data tensor */
    Collection<size_t> inDims;
    inDims.push_back(2);
    inDims.push_back(1);
    inDims.push_back(3);
    inDims.push_back(4);
    TensorPtr tensorData = TensorPtr(new HomogenTensor<>(inDims, Tensor::doAllocate));

    /* Fill tensor data using truncated gaussian initializer */
    /* Create an algorithm to initialize data using default method */
    initializers::truncated_gaussian::Batch<> truncatedGaussInitializer(0.0, 1.0);

    /* Set input object and parameters for the truncated gaussian initializer */
    truncatedGaussInitializer.input.set(initializers::data, tensorData);

    /* Compute truncated gaussian initializer */
    truncatedGaussInitializer.compute();

    /* Print the results of the truncated gaussian initializer */
    printTensor(tensorData, "Data with truncated gaussian distribution:");


    /* Fill tensor data using gaussian initializer */
    /* Create an algorithm to initialize data using default method */
    initializers::gaussian::Batch<> gaussInitializer(1.0, 0.5);

    /* Set input object for the gaussian initializer */
    gaussInitializer.input.set(initializers::data, tensorData);

    /* Compute gaussian initializer */
    gaussInitializer.compute();

    /* Print the results of the gaussian initializer */
    printTensor(tensorData, "Data with gaussian distribution:");


    /* Fill tensor data using uniform initializer */
    /* Create an algorithm to initialize data using default method */
    initializers::uniform::Batch<> uniformInitializer(-5.0, 5.0);

    /* Set input object and parameters for the uniform initializer */
    uniformInitializer.input.set(initializers::data, tensorData);

    /* Compute uniform initializer */
    uniformInitializer.compute();

    /* Print the results of the uniform initializer */
    printTensor(tensorData, "Data with uniform distribution:");


    /* Fill layer weights using xavier initializer */
    /* Create an algorithm to compute forward fully-connected layer results using default method */
    fullyconnected::forward::Batch<> fullyconnectedLayerForward(5);

    /* Set input objects and parameter for the forward fully-connected layer */
    fullyconnectedLayerForward.input.set(forward::data, tensorData);
    fullyconnectedLayerForward.parameter.weightsInitializer.reset(new initializers::xavier::Batch<>());

    /* Compute forward fully-connected layer results */
    fullyconnectedLayerForward.compute();

    /* Print the results of the xavier initializer */
    printTensor(fullyconnectedLayerForward.input.get(layers::forward::weights), "Weights filled by xavier initializer:");

    return 0;
}
