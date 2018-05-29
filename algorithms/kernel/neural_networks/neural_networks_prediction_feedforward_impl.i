/* file: neural_networks_prediction_feedforward_impl.i */
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
//++
//  Implementation of feedforward algorithm
//--
*/

#ifndef __NEURAL_NETWORKS_PREDICTION_FEEDFORWARD_IMPL_I__
#define __NEURAL_NETWORKS_PREDICTION_FEEDFORWARD_IMPL_I__

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace prediction
{
namespace internal
{
using namespace daal::services;

template<typename algorithmFPType, Method method, CpuType cpu>
Status NeuralNetworksFeedforwardPredictionKernel<algorithmFPType, method, cpu>::initialize(
    const Input *input, const neural_networks::prediction::Parameter *parameter, Result *result)
{
    nLayers   = input->get(prediction::model)->getLayers()->size();
    nSamples  = input->get(prediction::data)->getDimensions().get(0);
    batchSize = parameter->batchSize;
    if (nSamples < batchSize) { return Status(); }

    /* Get the number of last layers in the network and their indeces */
    Collection<layers::NextLayers> *nextLayers = input->get(prediction::model)->getNextLayers().get();
    KeyValueDataCollectionPtr predictionCollectionPtr = result->get(prediction::predictionCollection);

    lastLayersIndices.reset(new LastLayerIndices(nextLayers, predictionCollectionPtr));
    DAAL_CHECK_MALLOC(lastLayersIndices.get() && lastLayersIndices->isValid())

    nLastLayers = lastLayersIndices->nLast(); /* number of last layers in the network */

    /* Create a tensor to pass as an input to the first forward layer in neural network */
    Collection<size_t> sampleSize = input->get(prediction::data)->getDimensions();
    sampleSize[0] = batchSize;
    Status s;
    sample = HomogenTensor<algorithmFPType>::create(sampleSize, Tensor::doNotAllocate, &s);
    DAAL_CHECK_STATUS_VAR(s);

    /* Initialize buffers to manage reading memory operations for the last layer results */
    lastLayerResults.reset(nLastLayers);
    DAAL_CHECK_MALLOC(lastLayerResults.get())

    /* Initialize buffers to manage writing memory operations for the prediction results */
    predictions.reset(nLastLayers);
    DAAL_CHECK_MALLOC(predictions.get())

    return Status();
}

/**
 *  \brief Kernel for Neural Network prediction
 */
template<typename algorithmFPType, Method method, CpuType cpu>
Status NeuralNetworksFeedforwardPredictionKernel<algorithmFPType, method, cpu>::compute(const Input *input, Result *result)
{
    Status s;
    ForwardLayersPtr forwardLayers = input->get(prediction::model)->getLayers();
    TensorPtr data = input->get(prediction::data);
    if (nSamples < batchSize) { return s; }

    forwardLayers->get(0)->getLayerInput()->set(forward::data, sample);

    /* Buffer that manages reading memory operations for the input data tensor */
    ReadSubtensor<algorithmFPType, cpu> sampleSubtensor(data.get(), 0, 0, 0, 0);

    for (size_t i = 0; i < nLastLayers; i++)
    {
        TensorPtr valueTensor = forwardLayers->get(lastLayersIndices->layerIndex(i))->getLayerResult()->get(forward::value);
        lastLayerResults[i].set(*valueTensor, 0, 0, 0, 0);
    }

    /* Initialize buffers to manage writing memory operations for the prediction results */
    for (size_t i = 0; i < nLastLayers; i++)
    {
        TensorPtr predictionTensor = result->get(prediction::predictionCollection, lastLayersIndices->tensorIndex(i));
        predictions[i].set(*predictionTensor, 0, 0, 0, 0);
    }

    for(size_t i = 0; i < nSamples - batchSize + 1; i += batchSize)
    {
        /* Retrieve next batch of input data and pass it to the first layer */
        const algorithmFPType *sampleArray = sampleSubtensor.next(0, 0, i, batchSize);
        DAAL_CHECK_BLOCK_STATUS(sampleSubtensor)
        sample->setArray(const_cast<algorithmFPType *>(sampleArray));

        /* Forward pass through the neural network */
        for(size_t layerId = 0; layerId < nLayers; layerId++)
        {
            layers::forward::LayerIfacePtr forwardLayer = forwardLayers->get(layerId);
            DAAL_CHECK_STATUS(s, processLayerErrors(layerId, forwardLayer->computeNoThrow()))
        }

        /* Copy results from the last layers into the user provided memory */
        for (size_t j = 0; j < nLastLayers; j++)
        {
            const algorithmFPType *lastLayerResultArray = lastLayerResults[j].next(0, 0, 0, batchSize);
            DAAL_CHECK_BLOCK_STATUS(lastLayerResults[j])
            algorithmFPType *predictionArray = predictions[j].next(0, 0, i, batchSize);
            DAAL_CHECK_BLOCK_STATUS(predictions[j])

            size_t blockSize = lastLayerResults[j].getSize() * sizeof(algorithmFPType);
            daal_memcpy_s(predictionArray, blockSize, lastLayerResultArray, blockSize);
        }
    }
    return s;
}

template<typename algorithmFPType, Method method, CpuType cpu>
Status NeuralNetworksFeedforwardPredictionKernel<algorithmFPType, method, cpu>::reset()
{
    lastLayersIndices.reset();
    lastLayerResults.reset(0);
    predictions.reset(0);
    sample.reset();
    return Status();
}

} // namespace internal
} // namespace feedforward
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
