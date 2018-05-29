/* file: spatial_stochastic_pooling2d_layer_forward_batch_container.h */
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
//  Implementation of forward pooling layer container.
//--
*/

#ifndef __SPATIAL_PYRAMID_STOCHASTIC_POOLING2D_LAYER_FORWARD_BATCH_CONTAINER_H__
#define __SPATIAL_PYRAMID_STOCHASTIC_POOLING2D_LAYER_FORWARD_BATCH_CONTAINER_H__

#include "neural_networks/layers/spatial_pooling2d/spatial_stochastic_pooling2d_layer.h"
#include "spatial_pooling2d_layer_forward_kernel.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace spatial_stochastic_pooling2d
{
namespace forward
{
namespace interface1
{
template<typename algorithmFPType, CpuType cpu>
BatchContainer<algorithmFPType, defaultDense, cpu>::BatchContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(spatial_pooling2d::forward::internal::PoolingKernel, algorithmFPType, spatial_pooling2d::internal::stochastic);
}

template<typename algorithmFPType, CpuType cpu>
BatchContainer<algorithmFPType, defaultDense, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, CpuType cpu>
services::Status BatchContainer<algorithmFPType, defaultDense, cpu>::compute()
{
    spatial_stochastic_pooling2d::forward::Input *input = static_cast<spatial_stochastic_pooling2d::forward::Input *>(_in);
    spatial_stochastic_pooling2d::forward::Result *result = static_cast<spatial_stochastic_pooling2d::forward::Result *>(_res);
    spatial_stochastic_pooling2d::Parameter *parameter = static_cast<spatial_stochastic_pooling2d::Parameter *>(_par);

    data_management::Tensor *dataTensor = input->get(layers::forward::data).get();
    data_management::Tensor *valueTensor = result->get(layers::forward::value).get();
    data_management::Tensor *selectedPosTensor = nullptr;
    if(parameter->predictionStage == false)
    {
        selectedPosTensor = result->get(auxSelectedIndices).get();
    }


    __DAAL_CALL_KERNEL(env, spatial_pooling2d::forward::internal::PoolingKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, spatial_pooling2d::internal::stochastic),   \
                       compute, *dataTensor, *valueTensor, selectedPosTensor, *parameter);
}
} // namespace interface1
} // namespace forward

} // namespace spatial_stochastic_pooling2d
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
