/* file: convolution2d_layer_backward.cpp */
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
//  Implementation of onvolution2d calculation algorithm and types methods.
//--
*/

#include "convolution2d_layer_backward_types.h"
#include "convolution2d_layer_types.h"
#include "serialization_utils.h"
#include "daal_strings.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace convolution2d
{
namespace backward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_CONVOLUTION2D_BACKWARD_RESULT_ID);
/**
 * Default constructor
 */
Input::Input() {};

/**
 * Returns an input object for backward 2D convolution layer
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
data_management::TensorPtr Input::get(LayerDataId id) const
{
    layers::LayerDataPtr layerData =
        services::staticPointerCast<layers::LayerData, data_management::SerializationIface>(Argument::get(layers::backward::inputFromForward));
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*layerData)[id]);
}

/**
 * Sets input for the backward 2D convolution layer
 * \param[in] id    Identifier of the input  object
 * \param[in] value Input object to set
 */
void Input::set(LayerDataId id, const data_management::TensorPtr &value)
{
    layers::LayerDataPtr layerData = get(layers::backward::inputFromForward);
    (*layerData)[id] = value;
}

/**
 * Checks an input object of the 2D convolution layer
 * \param[in] parameter %Parameter of layer
 * \param[in] method    Computation method of the layer
 */
services::Status Input::check(const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::backward::Input::check(parameter, method));

    const Parameter *param = static_cast<const Parameter *>(parameter);

    data_management::TensorPtr xTensor = get(auxData);

    DAAL_CHECK_STATUS(s, data_management::checkTensor(xTensor.get(), auxDataStr()));

    const services::Collection<size_t> &xDims = xTensor->getDimensions();
    const services::Collection<size_t> &gDims = get(layers::backward::inputGradient)->getDimensions();

    size_t c1 =
        (xDims[param->indices.dims[0]] + 2 * param->paddings.size[0] - param->kernelSizes.size[0]) / param->strides.size[0] + 1;
    size_t c2 =
        (xDims[param->indices.dims[1]] + 2 * param->paddings.size[1] - param->kernelSizes.size[1]) / param->strides.size[1] + 1;

    DAAL_CHECK(((int)xDims[2] - (int)param->kernelSizes.size[0] - 1) >= 0, services::ErrorIncorrectKernelSise1);
    DAAL_CHECK(((int)xDims[3] - (int)param->kernelSizes.size[1] - 1) >= 0, services::ErrorIncorrectKernelSise2);

    services::Collection<size_t> gradDims;
    for(size_t i = 0; i < xDims.size(); i++)
    {
        if(i == param->indices.dims[0]) { gradDims.push_back(c1); }
        else if(i == param->indices.dims[1]) { gradDims.push_back(c2); }
        else if(i == param->groupDimension) { gradDims.push_back(param->nKernels); }
        else { gradDims.push_back( xDims[i] ); }
    }

    services::Collection<size_t> wDims;
    if (param->nGroups > 1)
    {
        wDims.push_back(param->nGroups);
    }
    wDims.push_back(param->nKernels / param->nGroups);
    wDims.push_back(xDims[param->groupDimension] / param->nGroups);
    wDims.push_back(param->kernelSizes.size[0]);
    wDims.push_back(param->kernelSizes.size[1]);

    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::inputGradient).get(), inputGradientStr(), &gradDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxWeights).get(), auxWeightsStr(), &wDims));
    return s;
}

/**
 * Default constructor
 */
Result::Result() : layers::backward::Result() {}

/**
 * Checks the result of the 2D convolution layer
 * \param[in] input   %Input object of the layer
 * \param[in] par     %Parameter of the layer
 * \param[in] method  Computation method of the layer
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::backward::Result::check(input, par, method));

    const Input *algInput = static_cast<const Input *>(input);
    const Parameter *param = static_cast<const Parameter *>(par);

    if (param->propagateGradient)
    {
        DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::gradient).get(), gradientStr(),
                                          &(algInput->get(auxData)->getDimensions())));
    }
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::weightDerivatives).get(), weightDerivativesStr(),
                                      &(algInput->get(auxWeights)->getDimensions())));

    services::Collection<size_t> bDims;
    bDims.push_back(param->nKernels);

    return data_management::checkTensor(get(layers::backward::biasDerivatives).get(), biasDerivativesStr(), &bDims);
}

}// namespace interface1
}// namespace forward
}// namespace convolution2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
