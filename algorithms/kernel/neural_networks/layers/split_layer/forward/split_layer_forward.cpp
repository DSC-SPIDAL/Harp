/* file: split_layer_forward.cpp */
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
//  Implementation of split calculation algorithm and types methods.
//--
*/

#include "split_layer_forward_types.h"
#include "split_layer_types.h"
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
namespace split
{
namespace forward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_SPLIT_FORWARD_RESULT_ID);
/** \brief Default constructor */
Input::Input() {};
Input::Input(const Input& other) : super(other) {}

/** \brief Default constructor */
Result::Result() : layers::forward::Result() {};

/**
 * Returns a tensor with a given index from the result
 * \param[in] id    Identifier of the collection of input tensors
 * \param[in] index Index of the tensor to be returned
 * \return          Pointer to the table with the input tensor
 */
data_management::TensorPtr Result::get(ResultLayerDataId id, size_t index) const
{
    LayerDataPtr resCollection = get(id);
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*resCollection)[index]);
}

/**
 * Returns result of the layer algorithm
 * \param[in] id   Identifier of the result
 * \return         Result that corresponds to the given identifier
 */
LayerDataPtr Result::get(ResultLayerDataId id) const
{
    return services::staticPointerCast<LayerData, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Sets the result of the forward split layer
 * \param[in] id      Identifier of the result
 * \param[in] value   Result
 *
 * \return Status of computations
 */
void Result::set(ResultLayerDataId id, const LayerDataPtr &value)
{
    Argument::set(id, value);
}

/**
 * Sets the result of the forward split layer
 * \param[in] id      Identifier of the result
 * \param[in] value   Result
 * \param[in] index   Index of the result
 *
 * \return Status of computations
 */
void Result::set(ResultLayerDataId id, const data_management::TensorPtr &value, size_t index)
{
    LayerDataPtr layerData = this->get(id);
    (*layerData)[index] = value;
}

/**
 * Returns the layout of the result object for the layer algorithm
 * \return Layout of the result object for the layer algorithm
 */
LayerResultLayout Result::getLayout()  { return collectionResult; }

/**
 * Returns resulting value of the forward split layer
 * \param[in] index Index of the tensor with value
 * \return Resulting value that corresponds to the given index
 */
data_management::TensorPtr Result::getValue(size_t index) const
{
    return get(valueCollection, index);
}

/**
 * Checks the result of the forward split layer
 * \param[in] input   %Input object for the algorithm
 * \param[in] par     %Parameter of the algorithm
 * \param[in] method  Computation method
 *
 * \return Status of computations
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    DAAL_CHECK(Argument::size() == 2, services::ErrorIncorrectNumberOfInputNumericTables);

    const Input *algInput = static_cast<const Input *>(input);
    const Parameter *parameter = static_cast<const Parameter *>(par);
    const size_t nOutputs = parameter->nOutputs;

    data_management::TensorPtr inputTensor = algInput->get(layers::forward::data);
    services::Status s;
    DAAL_CHECK_STATUS(s, data_management::checkTensor(inputTensor.get(), dataStr()));

    const services::Collection<size_t> &inputDims = inputTensor->getDimensions();

    for (size_t i = 0; i < nOutputs; i++)
    {
        DAAL_CHECK_STATUS(s, data_management::checkTensor(get(valueCollection, i).get(), valueCollectionStr(), &inputDims));
    }
    return s;
}

/**
* Returns collection of dimensions of split layer output
* \param[in] inputSize   Collection of input tensor dimensions
* \param[in] par         Parameters of the algorithm
* \param[in] method      Method of the algorithm
* \return    Collection of dimensions of split layer output
*/
const services::Collection<size_t> Result::getValueSize(const services::Collection<size_t> &inputSize,
                                                        const daal::algorithms::Parameter *par, const int method) const
{
    return services::Collection<size_t>();
}

/**
* Returns collection of dimensions of split layer output
* \param[in] inputSize   Collection of input tensor dimensions
* \param[in] parameter   Parameters of the algorithm
* \param[in] method      Method of the algorithm
* \return    Collection of dimensions of split layer output
*/
services::Collection< services::Collection<size_t> > Result::getValueCollectionSize(const services::Collection<size_t> &inputSize,
                                                                            const daal::algorithms::Parameter *parameter, const int method)
{
    const Parameter *par = static_cast<const Parameter *>(parameter);
    const size_t nOutputs = par->nOutputs;

    services::Collection<services::Collection<size_t> > dimsCollection;

    for (size_t i = 0; i < nOutputs; i++)
    {
        dimsCollection.push_back(inputSize);
    }

    return dimsCollection;
}

}// namespace interface1
}// namespace forward
}// namespace split
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
