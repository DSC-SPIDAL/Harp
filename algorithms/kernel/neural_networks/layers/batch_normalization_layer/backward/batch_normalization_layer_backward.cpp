/* file: batch_normalization_layer_backward.cpp */
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

#include "batch_normalization_layer_backward_types.h"
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
namespace backward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_BATCH_NORMALIZATION_BACKWARD_RESULT_ID);
/** Default constructor */
Input::Input() {};
Input::Input(const Input& other) : super(other) {}

/**
 * Returns an input object for the backward batch normalization layer
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
data_management::TensorPtr Input::get(LayerDataId id) const
{
    layers::LayerDataPtr inputData = get(layers::backward::inputFromForward);
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*inputData)[id]);
}

/**
 * Sets an input object for the backward batch normalization layer
 * \param[in] id  Identifier of the input object
 * \param[in] ptr Pointer to the object
 */
void Input::set(LayerDataId id, const data_management::TensorPtr &ptr)
{
    layers::LayerDataPtr inputData = get(layers::backward::inputFromForward);
    (*inputData)[id] = ptr;
}

/**
 * Checks an input object for the backward batch normalization layer
 * \param[in] parameter Layer parameter
 * \param[in] method    Computation method
 */
services::Status Input::check(const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::backward::Input::check(parameter, method));

    const Parameter *algParameter = static_cast<const Parameter *>(parameter);

    data_management::TensorPtr inputGradientTensor = get(layers::backward::inputGradient);
    DAAL_CHECK_STATUS(s, data_management::checkTensor(inputGradientTensor.get(), inputGradientStr()));
    const services::Collection<size_t> &dataDims = inputGradientTensor->getDimensions();

    size_t dimension = algParameter->dimension;
    double epsilon = algParameter->epsilon;
    DAAL_CHECK_EX(dimension <= dataDims.size(), services::ErrorIncorrectParameter, services::ParameterName, dimensionStr());
    DAAL_CHECK_EX(epsilon > 0.0 && epsilon < 1.0, services::ErrorIncorrectParameter, services::ParameterName, epsilonStr());

    size_t dimensionSize = dataDims[dimension];
    services::Collection<size_t> auxDims(1);
    auxDims[0] = dimensionSize;

    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxData).get(),               auxDataStr(),              &dataDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxWeights).get(),            auxWeightsStr(),            &auxDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxMean).get(),               auxMeanStr(),               &auxDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxStandardDeviation).get(),  auxStandardDeviationStr(),  &auxDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxPopulationMean).get(),     auxPopulationMeanStr(),     &auxDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxPopulationVariance).get(), auxPopulationVarianceStr(), &auxDims));
    return s;
}

/** Default constructor */
Result::Result() {}

/**
 * Checks the result of the backward batch normalization layer
 * \param[in] input     %Input object for the layer
 * \param[in] parameter %Parameter of the layer
 * \param[in] method    Computation method
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    const Input *algInput = static_cast<const Input *>(input);
    const Parameter *algParameter = static_cast<const Parameter *>(parameter);
    size_t dimension = algParameter->dimension;

    data_management::TensorPtr inputGradientTensor = algInput->get(layers::backward::inputGradient);
    const services::Collection<size_t> &gradientDims = inputGradientTensor->getDimensions();

    services::Status s;
    if (algParameter->propagateGradient)
    {
        DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::gradient).get(), gradientStr(), &gradientDims));
    }

    size_t dimensionSize = gradientDims[dimension];
    services::Collection<size_t> derDims(1);
    derDims[0] = dimensionSize;

    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::weightDerivatives).get(), weightDerivativesStr(), &derDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::biasDerivatives).get(), biasDerivativesStr(), &derDims));
    return s;
}

}// namespace interface1
}// namespace backward
}// namespace batch_normalization
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
