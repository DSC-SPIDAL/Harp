/* file: locallyconnected2d_layer.cpp */
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
//  Implementation of locallyconnected2d calculation algorithm and types methods.
//--
*/

#include "locallyconnected2d_layer_types.h"
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
namespace locallyconnected2d
{
namespace interface1
{
/**
 *  Default constructor
 */
Parameter::Parameter() : groupDimension(1), indices(2, 3), kernelSizes(2, 2), strides(2, 2), paddings(0, 0), nKernels(1), nGroups(1) {}

/**
 * Checks the correctness of the parameter
 */
services::Status Parameter::check() const
{
    DAAL_CHECK_EX(groupDimension <= 3, services::ErrorIncorrectParameter, services::ParameterName, groupDimensionStr());
    DAAL_CHECK_EX(indices.dims[0] <= 3 && indices.dims[1] <= 3, services::ErrorIncorrectParameter, services::ParameterName, indicesStr());
    DAAL_CHECK_EX(indices.dims[0] != indices.dims[1], services::ErrorIncorrectParameter, services::ParameterName, indicesStr());
    DAAL_CHECK_EX(strides.size[0] != 0  && strides.size[1] != 0, services::ErrorIncorrectParameter, services::ParameterName, stridesStr());
    DAAL_CHECK_EX(kernelSizes.size[0] != 0 && kernelSizes.size[1] != 0, services::ErrorIncorrectParameter, services::ParameterName, kernelSizesStr());
    return services::Status();
}

}// namespace interface1
}// namespace locallyconnected2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
