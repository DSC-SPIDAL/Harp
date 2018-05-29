/* file: layer_backward.cpp */
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
//  Implementation of neural_networks backward layer methods.
//--
*/

#include "layer_backward_types.h"
#include "layer_types.h"
#include "daal_strings.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace backward
{
namespace interface1
{
InputIface::InputIface(const InputIface& other) : daal::algorithms::Input(other) {}
Input::Input(const Input& other) : InputIface(other){}

/** \brief Constructor */
Input::Input() : InputIface(lastInputLayerDataId + 1)
{
    set(inputFromForward, LayerDataPtr(new LayerData()));
}

/**
 * Returns input Tensor of the layer algorithm
 * \param[in] id    Identifier of the input tensor
 * \return          %Input tensor that corresponds to the given identifier
 */
data_management::TensorPtr Input::get(InputId id) const
{
    return data_management::Tensor::cast(Argument::get(id));
}

/**
 * Returns input Tensor of the layer algorithm
 * \param[in] id    Identifier of the input tensor
 * \return          %Input tensor that corresponds to the given identifier
 */
LayerDataPtr Input::get(InputLayerDataId id) const
{
    return services::staticPointerCast<LayerData, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Sets input for the layer algorithm
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   Pointer to the object
 */
void Input::set(InputId id, const data_management::TensorPtr &ptr)
{
    Argument::set(id, ptr);
}

/**
 * Sets input for the layer algorithm
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   Pointer to the object
 */
void Input::set(InputLayerDataId id, const LayerDataPtr &ptr)
{
    Argument::set(id, ptr);
}

/**
 * Adds tensor with input gradient to the input object of the layer algorithm
 * \param[in] igTensor  Tensor with input gradient
 * \param[in] index     Index of the tensor with input gradient
 */
services::Status Input::addInputGradient(const data_management::TensorPtr &igTensor, size_t index)
{
    set(layers::backward::inputGradient, igTensor);
    return services::Status();
}

/**
 * Sets input structure retrieved from the result of the forward layer
 * \param[in] result Result of the forward layer
 */
services::Status Input::setInputFromForward(forward::ResultPtr result)
{
    set(inputFromForward, result->get(forward::resultForBackward));
    return services::Status();
}

/**
 * Checks an input object for the layer algorithm
 * \param[in] par     %Parameter of algorithm
 * \param[in] method  Computation method of the algorithm
 */
services::Status Input::check(const daal::algorithms::Parameter *par, int method) const
{
    if(Argument::size() != 2) return services::Status(services::ErrorIncorrectNumberOfInputNumericTables);
    services::Status s;
    s |= data_management::checkTensor(get(inputGradient).get(), inputGradientStr());
    if(!s) return s;

    LayerDataPtr layerData = get(inputFromForward);
    if (!layerData) return services::Status(services::ErrorNullLayerData);
    return s;
}

/**
 * Returns the layout of the input object for the layer algorithm
 * \return Layout of the input object for the layer algorithm
 */
LayerInputLayout Input::getLayout() const { return tensorInput; }

/**
 * Returns result of the layer algorithm
 * \param[in] id   Identifier of the result
 * \return         Result that corresponds to the given identifier
 */
data_management::TensorPtr Result::get(ResultId id) const
{
    return data_management::Tensor::cast(Argument::get(id));
}

/** \brief Constructor */
Result::Result() : daal::algorithms::Result(lastResultLayerDataId + 1)
{
    Argument::set(resultLayerData, LayerDataPtr(new LayerData()));
};

/**
 * Sets the result of the layer algorithm
 * \param[in] id    Identifier of the result
 * \param[in] ptr   Pointer to the result
 */
void Result::set(ResultId id, const data_management::TensorPtr &ptr)
{
    Argument::set(id, ptr);
}

/**
* Returns result InputLayerData of the layer algorithm
* \param[in] id    Identifier of the result object
* \return          Resulting InputLayerData that corresponds to the given identifier
*/
LayerDataPtr Result::get(backward::ResultLayerDataId id) const
{
    return services::staticPointerCast<LayerData, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Sets result for the layer algorithm
 * \param[in] id    Identifier of the result object
 * \param[in] ptr   Pointer to the object
 */
void Result::set(ResultLayerDataId id, const LayerDataPtr &ptr)
{
    Argument::set(id, ptr);
}

/**
 * Checks the result object for the layer algorithm
 * \param[in] input         %Input of algorithm
 * \param[in] parameter     %Parameter of algorithm
 * \param[in] method        Computation method of the algorithm
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    if(Argument::size() != 4) return services::Status(services::ErrorIncorrectNumberOfInputNumericTables);
    services::Status s;

    const layers::Parameter *param = static_cast<const layers::Parameter *>(parameter);
    if (param->propagateGradient)
    {
        s |= data_management::checkTensor(get(gradient).get(), gradientStr());
        if(!s) return s;
    }
    return s;
}

/**
 * Returns the layout of the result object for the layer algorithm
 * \return Layout of the result object for the layer algorithm
 */
LayerResultLayout Result::getLayout() const { return tensorResult; }

/**
 * Returns resulting gradient of the layer algorithm
 * \param[in] index Index of the tensor with gradient
 * \return Resulting gradient that corresponds to the given index
 */
data_management::TensorPtr Result::getGradient(size_t index) const
{
    return get(layers::backward::gradient);
}

}// namespace interface1
}// namespace backward
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
