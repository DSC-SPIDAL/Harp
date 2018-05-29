/* file: lcn_layer_forward.cpp */
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
//  Implementation of lcn calculation algorithm and types methods.
//--
*/

#include "lcn_layer_forward_types.h"
#include "lcn_layer_types.h"
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
namespace lcn
{
namespace forward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_LCN_FORWARD_RESULT_ID);
/**
 * Default constructor
 */
Input::Input() {};
Input::Input(const Input& other) : super(other) {}

/**
 * Returns dimensions of weights tensor
 * \return Dimensions of weights tensor
 */
const services::Collection<size_t> Input::getWeightsSizes(const layers::Parameter *parameter) const
{
    return services::Collection<size_t>();
}

/**
* Returns dimensions of biases tensor
* \return Dimensions of biases tensor
*/
const services::Collection<size_t> Input::getBiasesSizes(const layers::Parameter *parameter) const
{
    return services::Collection<size_t>();
}

/**
* Checks input object of the forward local contrast normalization layer
* \param[in] parameter %Parameter of layer
* \param[in] method    Computation method of the layer
*/
services::Status Input::check(const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::forward::Input::check(parameter, method));

    const Parameter *algParameter = static_cast<const Parameter *>(parameter);

    data_management::TensorPtr dataTensor = get(layers::forward::data);
    DAAL_CHECK_STATUS(s, data_management::checkTensor(dataTensor.get(), dataStr()));

    size_t nDims = dataTensor->getNumberOfDimensions();

    if( nDims != 4 ) return( services::ErrorIncorrectNumberOfDimensionsInTensor );
    return services::Status();
}
/**
 * Default constructor
 */
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
 * Returns the result of forward local contrast normalization layer
 * \param[in] id   Identifier of the result
 * \return         Result that corresponds to the given identifier
 */
data_management::TensorPtr Result::get(LayerDataId id) const
{
    layers::LayerDataPtr layerData =
        services::staticPointerCast<layers::LayerData, data_management::SerializationIface>(Argument::get(layers::forward::resultForBackward));
    if(!layerData)
        return data_management::TensorPtr();
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*layerData)[id]);
}

/**
 * Sets the result of forward local contrast normalization layer
 * \param[in] id     Identifier of the result
 * \param[in] value  Result
 */
void Result::set(LayerDataId id, const data_management::TensorPtr &value)
{
    layers::LayerDataPtr layerData =
        services::staticPointerCast<layers::LayerData, data_management::SerializationIface>(Argument::get(layers::forward::resultForBackward));
    if(layerData)
        (*layerData)[id] = value;
}

/**
 * Checks the result of the forward local contrast normalization layer
 * \param[in] input   %Input object of the layer
 * \param[in] par     %Parameter of the layer
 * \param[in] method  Computation method of the layer
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::forward::Result::check(input, par, method));

    services::SharedPtr<services::Error> error;

    const Input     *algInput     = static_cast<const Input *>(input);
    const Parameter *algParameter = static_cast<const Parameter *>(par);

    LayerDataPtr layerData = get(layers::forward::resultForBackward);
    if (!layerData) return services::Status(services::ErrorNullLayerData);

    const services::Collection<size_t> dataDims  = algInput->get(layers::forward::data)->getDimensions();
    services::Collection<size_t> sigmaDims;
    getSigmaDimensions(algInput, algParameter, sigmaDims);

    services::Collection<size_t> cDims;
    getCDimensions(algInput, algParameter, cDims);

    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::forward::value).get(), valueStr(), &dataDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxCenteredData).get(), auxCenteredDataStr(), &dataDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxC).get(), auxCStr(), &cDims));
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxInvMax).get(), auxInvMaxStr(), &sigmaDims));
    if(!algParameter->predictionStage)
    {
        DAAL_CHECK_STATUS(s, data_management::checkTensor(get(auxSigma).get(), auxSigmaStr(), &sigmaDims));
    }
    return s;
}

void Result::getSigmaDimensions(const Input *in, const Parameter *algParameter, services::Collection<size_t> &sigmaDims) const
{
    sigmaDims = in->get(layers::forward::data)->getDimensions();

    if(algParameter->sumDimension)
    {
        data_management::NumericTablePtr dimensionTable = algParameter->sumDimension;

        data_management::BlockDescriptor<int> block;
        dimensionTable->getBlockOfRows(0, 1, data_management::readOnly, block);
        int *dataInt = block.getBlockPtr();
        size_t dim = dataInt[0];
        dimensionTable->releaseBlockOfRows(block);
        sigmaDims.erase(dim);
    }
}

void Result::getCDimensions(const Input *in, const Parameter *algParameter, services::Collection<size_t> &cDims) const
{
    getSigmaDimensions(in, algParameter, cDims);

    if(algParameter->sumDimension)
    {
        cDims.erase(algParameter->indices.dims[1] - 1);
        cDims.erase(algParameter->indices.dims[0] - 1);
    }
    else
    {
        cDims.erase(algParameter->indices.dims[1]);
        cDims.erase(algParameter->indices.dims[0]);
    }
}


}// namespace interface1
}// namespace forward
}// namespace lcn
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
