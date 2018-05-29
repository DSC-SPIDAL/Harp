/* file: batch_normalization_layer_forward.cpp */
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
//  Implementation of batch normalization calculation algorithm and types methods.
//--
*/

#include "batch_normalization_layer_forward_types.h"
#include "batch_normalization_layer_types.h"
#include "serialization_utils.h"
#include "daal_strings.h"

using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace batch_normalization
{
namespace forward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_BATCH_NORMALIZATION_FORWARD_RESULT_ID);
/** Default constructor */
Input::Input() : layers::forward::Input(lastInputLayerDataId + 1) {}
Input::Input(const Input& other) : super(other) {}

/**
 * Returns dimensions of weights tensor
 * \return Dimensions of weights tensor
 */
const services::Collection<size_t> Input::getWeightsSizes(const layers::Parameter *parameter) const
{
    const Parameter *algParameter =  static_cast<const Parameter *>(parameter);
    const services::Collection<size_t> &dims = get(layers::forward::data)->getDimensions();
    services::Collection<size_t> wDims(1);
    wDims[0] = dims[algParameter->dimension];
    return wDims;
}

/**
 * Returns dimensions of biases tensor
 * \return Dimensions of biases tensor
 */
const services::Collection<size_t> Input::getBiasesSizes(const layers::Parameter *parameter) const
{
    return getWeightsSizes(parameter);
}

/**
 * Returns an input object for forward batch normalization layer
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
data_management::TensorPtr Input::get(InputLayerDataId id) const
{
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Sets input for the forward batch normalization layer
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   Input object to set
 */
void Input::set(InputLayerDataId id, const data_management::TensorPtr &ptr)
{
    Argument::set(id, ptr);
}

/**
 * Checks input object of the forward batch normalization layer
 * \param[in] parameter %Parameter of layer
 * \param[in] method    Computation method of the layer
 */
services::Status Input::check(const daal::algorithms::Parameter *parameter, int method) const
{
    const Parameter *algParameter = static_cast<const Parameter *>(parameter);

    data_management::TensorPtr dataTensor = get(layers::forward::data);
    services::Status s;
    DAAL_CHECK_TENSOR(s, dataTensor.get(), dataStr());

    const services::Collection<size_t> &dataDims = dataTensor->getDimensions();

    size_t dimension = algParameter->dimension;
    double alpha = algParameter->alpha;
    double epsilon = algParameter->epsilon;
    DAAL_CHECK_EX(dimension <= dataDims.size(), services::ErrorIncorrectParameter, services::ParameterName, dimensionStr());
    DAAL_CHECK_EX((alpha > 0.0 && alpha < 1.0), services::ErrorIncorrectParameter, services::ParameterName, alphaStr());
    DAAL_CHECK_EX(epsilon > 0.0 && epsilon < 1.0, services::ErrorIncorrectParameter, services::ParameterName, epsilonStr());

    size_t dimensionSize = dataTensor->getDimensionSize(dimension);
    services::Collection<size_t> weightDims(1);
    weightDims[0] = dimensionSize;

    DAAL_CHECK_TENSOR(s, get(layers::forward::weights).get(), weightsStr(), &weightDims);
    DAAL_CHECK_TENSOR(s, get(layers::forward::biases).get(),  biasesStr(),  &weightDims);

    if (algParameter->predictionStage)
    {
        DAAL_CHECK_TENSOR(s, get(populationMean).get(),     populationMeanStr(),     &weightDims);
        DAAL_CHECK_TENSOR(s, get(populationVariance).get(), populationVarianceStr(), &weightDims);
    }

    return s;
}

/** Default Constructor */
Result::Result() {}

/**
 * Returns dimensions of value tensor
 * \return Dimensions of value tensor
 */
const services::Collection<size_t> Result::getValueSize(const services::Collection<size_t> &inputSize,
                                                        const daal::algorithms::Parameter *par, const int method) const
{
    return inputSize;
}

/**
 * Sets the result that is used in backward batch normalization layer
 * \param[in] input     Pointer to an object containing the input data
 */
services::Status Result::setResultForBackward(const daal::algorithms::Input *input)
{
    const Input *in = static_cast<const Input *>(input);
    set(auxData,    in->get(layers::forward::data));
    set(auxWeights, in->get(layers::forward::weights));
    return services::Status();
}

/**
 * Returns the result of the forward batch normalization layer
 * \param[in] id    Identifier of the result
 * \return          Result that corresponds to the given identifier
 */
data_management::TensorPtr Result::get(LayerDataId id) const
{
    layers::LayerDataPtr layerData = get(layers::forward::resultForBackward);
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*layerData)[id]);
}

/**
 * Sets the result of the forward batch normalization layer
 * \param[in] id    Identifier of the result
 * \param[in] ptr   Result
 */
void Result::set(LayerDataId id, const data_management::TensorPtr &ptr)
{
    layers::LayerDataPtr layerData = get(layers::forward::resultForBackward);
    (*layerData)[id] = ptr;
}

/**
 * Checks the result of the forward batch normalization layer
 * \param[in] input     %Input of the layer
 * \param[in] parameter %Parameter of the layer
 * \param[in] method    Computation method of the layer
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    const Input *algInput = static_cast<const Input *>(input);
    const Parameter *algParameter = static_cast<const Parameter *>(parameter);
    size_t dimension = algParameter->dimension;

    data_management::TensorPtr dataTensor = algInput->get(layers::forward::data);
    const services::Collection<size_t> &dataDims = dataTensor->getDimensions();

    data_management::TensorPtr valueTensor = get(layers::forward::value);
    DAAL_CHECK_TENSOR(s, valueTensor.get(), valueStr(), &dataDims);

    size_t dimensionSize = valueTensor->getDimensionSize(dimension);
    services::Collection<size_t> auxDims(1);
    auxDims[0] = dimensionSize;

    LayerDataPtr layerData = get(layers::forward::resultForBackward);
    if (!layerData) return services::Status(services::ErrorNullLayerData);

    if(!algParameter->predictionStage)
    {
        DAAL_CHECK_TENSOR(s, get(auxMean).get(),               auxMeanStr(),               &auxDims);
        DAAL_CHECK_TENSOR(s, get(auxStandardDeviation).get(),  auxStandardDeviationStr(),  &auxDims);
        DAAL_CHECK_TENSOR(s, get(auxData).get(),               auxDataStr(),               &dataDims);
        DAAL_CHECK_TENSOR(s, get(auxWeights).get(),            auxWeightsStr(),            &auxDims);
        DAAL_CHECK_TENSOR(s, get(auxPopulationMean).get(),     auxPopulationMeanStr(),     &auxDims);
        DAAL_CHECK_TENSOR(s, get(auxPopulationVariance).get(), auxPopulationVarianceStr(), &auxDims);
    }
    return s;
}
}// namespace interface1
}// namespace forward
}// namespace batch_normalization
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
