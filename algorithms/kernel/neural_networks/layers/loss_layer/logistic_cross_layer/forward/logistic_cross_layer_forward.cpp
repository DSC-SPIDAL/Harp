/* file: logistic_cross_layer_forward.cpp */
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
//  Implementation of logistic cross calculation algorithm and types methods.
//--
*/

#include "logistic_cross_layer_types.h"
#include "logistic_cross_layer_forward_types.h"
#include "serialization_utils.h"
#include "daal_strings.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace loss
{
namespace logistic_cross
{
namespace forward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_LOSS_LOGISTIC_CROSS_FORWARD_RESULT_ID);
/** Default constructor */
Input::Input() : loss::forward::Input() {};
Input::Input(const Input& other) : super(other) {}

/**
 * Returns dimensions of weights tensor
 * \return Dimensions of weights tensor
 */
const Collection<size_t> Input::getWeightsSizes(const layers::Parameter *parameter) const
{
    return Collection<size_t>();
}

/**
 * Returns dimensions of biases tensor
 * \return Dimensions of biases tensor
 */
const Collection<size_t> Input::getBiasesSizes(const layers::Parameter *parameter) const
{
    return Collection<size_t>();
}

/**
 * Checks an input object for the layer algorithm
 * \param[in] par     %Parameter of algorithm
 * \param[in] method  Computation method of the algorithm
 */
services::Status Input::check(const daal::algorithms::Parameter *par, int method) const
{
    if(Argument::size() != 5) return services::Status(ErrorIncorrectNumberOfInputNumericTables);

    TensorPtr dataTensor = get(layers::forward::data);
    TensorPtr groundTruthTensor = get(layers::loss::forward::groundTruth);

    services::Status s;
    DAAL_CHECK_STATUS(s, checkTensor(dataTensor.get(), dataStr()));
    const Collection<size_t> &inputDims = dataTensor->getDimensions();

    DAAL_CHECK_STATUS(s, checkTensor(groundTruthTensor.get(), groundTruthStr()));
    const Collection<size_t> &gtDims = groundTruthTensor->getDimensions();

    DAAL_CHECK_EX(dataTensor->getSize() == groundTruthTensor->getSize(), ErrorIncorrectSizeOfDimensionInTensor, ParameterName, groundTruthStr());
    DAAL_CHECK_EX(gtDims.size() == 1 || gtDims.size() == inputDims.size() , ErrorIncorrectNumberOfDimensionsInTensor, ParameterName, dataStr());
    DAAL_CHECK_EX(gtDims[0] == inputDims[0] , ErrorIncorrectSizeOfDimensionInTensor, ParameterName, dataStr());
    return s;
}

    /** Default constructor */
Result::Result() : loss::forward::Result() {};

/**
 * Returns the result of the forward logistic cross-entropy layer
 * \param[in] id    Identifier of the result
 * \return          Result that corresponds to the given identifier
 */
TensorPtr Result::get(LayerDataId id) const
{
    LayerDataPtr layerData = layers::LayerData::cast<SerializationIface>(Argument::get(layers::forward::resultForBackward));
    if(!layerData)
        return data_management::TensorPtr();
    return Tensor::cast<SerializationIface>((*layerData)[id]);
}

/**
 * Sets the result of the forward logistic cross-entropy layer
 * \param[in] id      Identifier of the result
 * \param[in] value   Result
 */
void Result::set(LayerDataId id, const TensorPtr &value)
{
    LayerDataPtr layerData = layers::LayerData::cast<SerializationIface>(Argument::get(layers::forward::resultForBackward));
    if (!layerData) return;
    (*layerData)[id] = value;
}

/**
 * Checks the result of the forward logistic cross-entropy layer
 * \param[in] input   %Input object for the layer
 * \param[in] par     %Parameter of the layer
 * \param[in] method  Computation method
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    const Input *in = static_cast<const Input * >(input);
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::forward::Result::check(input, par, method));

    Collection<size_t> valueDim(1);
    valueDim[0] = 1;
    DAAL_CHECK_STATUS(s, checkTensor(get(layers::forward::value).get(), valueStr(), &valueDim));
    DAAL_CHECK_STATUS(s, checkTensor(get(auxData).get(), auxDataStr(), &(in->get(layers::forward::data)->getDimensions())));
    const layers::Parameter *parameter = static_cast<const layers::Parameter * >(par);
    if(!parameter->predictionStage)
    {
        DAAL_CHECK_STATUS(s, checkTensor(get(auxGroundTruth).get(), auxGroundTruthStr(), &(in->get(loss::forward::groundTruth)->getDimensions())));
    }
    return s;
}

/**
 * Returns dimensions of value tensor
 * \return Dimensions of value tensor
 */
const Collection<size_t> Result::getValueSize(const Collection<size_t> &inputSize,
        const daal::algorithms::Parameter *par, const int method) const
{
    return inputSize;
}

/**
 * Sets the result that is used in backward abs layer
 * \param[in] input     Pointer to an object containing the input data
 */
services::Status Result::setResultForBackward(const daal::algorithms::Input *input)
{
    const loss::logistic_cross::forward::Input *in = static_cast<const loss::logistic_cross::forward::Input * >(input);
    set(auxData, in->get(layers::forward::data));
    set(auxGroundTruth, in->get(loss::forward::groundTruth));
    return services::Status();
}
}// namespace interface1
}// namespace forward
}// namespace logistic_cross
}// namespace loss
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
