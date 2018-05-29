/* file: stochastic_pooling2d_layer_forward.cpp */
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
//  Implementation of stochastic_pooling2d calculation algorithm and types methods.
//--
*/

#include "stochastic_pooling2d_layer_forward_types.h"
#include "stochastic_pooling2d_layer_types.h"
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
namespace stochastic_pooling2d
{
namespace forward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_STOCHASTIC_POOLING2D_FORWARD_RESULT_ID);
/** Default constructor */
Result::Result() {}

/**
 * Returns the result of the forward stochastic 2D pooling layer
 * \param[in] id    Identifier of the result
 * \return          Result that corresponds to the given identifier
 */
data_management::TensorPtr Result::get(LayerDataId id) const
{
    layers::LayerDataPtr layerData = get(layers::forward::resultForBackward);
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*layerData)[id]);
}

/**
 * Returns the result of the forward stochastic 2D pooling layer
 * \param[in] id    Identifier of the result
 * \return          Result that corresponds to the given identifier
 */
data_management::NumericTablePtr Result::get(LayerDataNumericTableId id) const
{
    layers::LayerDataPtr layerData = get(layers::forward::resultForBackward);
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>((*layerData)[id]);
}

/**
 * Sets the result of the forward stochastic 2D pooling layer
 * \param[in] id Identifier of the result
 * \param[in] ptr Result
 */
void Result::set(LayerDataId id, const data_management::TensorPtr &ptr)
{
    layers::LayerDataPtr layerData = get(layers::forward::resultForBackward);
    (*layerData)[id] = ptr;
}

/**
 * Sets the result of the forward stochastic 2D pooling layer
 * \param[in] id Identifier of the result
 * \param[in] ptr Result
 */
void Result::set(LayerDataNumericTableId id, const data_management::NumericTablePtr &ptr)
{
    layers::LayerDataPtr layerData = get(layers::forward::resultForBackward);
    (*layerData)[id] = ptr;
}

/**
 * Checks the result of the forward stochastic 2D pooling layer
 * \param[in] input     %Input of the layer
 * \param[in] parameter %Parameter of the layer
 * \param[in] method    Computation method of the layer
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status s;
    DAAL_CHECK_STATUS(s, pooling2d::forward::Result::check(input, parameter, method));

    const Parameter *param =  static_cast<const Parameter *>(parameter);

    if(!param->predictionStage)
    {
        const Input *in = static_cast<const Input *>(input);

        const services::Collection<size_t> &valueDimensions = get(layers::forward::value)->getDimensions();
        DAAL_CHECK_STATUS(s, data_management::checkTensor(get(stochastic_pooling2d::auxSelectedIndices).get(), auxSelectedIndicesStr(), &valueDimensions));

        const services::Collection<size_t> &dataDims = in->get(layers::forward::data)->getDimensions();
        data_management::NumericTablePtr auxInputDimensions = get(stochastic_pooling2d::auxInputDimensions);

        DAAL_CHECK_STATUS(s, data_management::checkNumericTable(auxInputDimensions.get(), auxInputDimensionsStr(), data_management::packed_mask, 0, dataDims.size(), 1));
    }
    return s;
}

}// namespace interface1
}// namespace forward
}// namespace stochastic_pooling2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
