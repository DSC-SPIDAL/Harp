/* file: spatial_pooling2d_layer_backward_task.h */
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

//++
//  Declaration of template function that calculate backward pooling layer relults.
//--


#ifndef __SPATIAL_POOLING2D_LAYER_BACKWARD_TASK_H__
#define __SPATIAL_POOLING2D_LAYER_BACKWARD_TASK_H__

#include "neural_networks/layers/spatial_pooling2d/spatial_pooling2d_layer_backward_types.h"
#include "spatial_pooling2d_layer_internal_types.h"
#include "tensor.h"
#include "service_tensor.h"
#include "service_numeric_table.h"

using namespace daal::data_management;
using namespace daal::services;
using namespace daal::internal;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace spatial_pooling2d
{
namespace backward
{
namespace internal
{
template<typename algorithmFPType, CpuType cpu>
class DAAL_EXPORT BasePoolingTask
{
public:
    BasePoolingTask(const Tensor &_inputGradientTensor,
                    Tensor &_gradientTensor,
                    const spatial_pooling2d::Parameter &_spatialParameter) :
        inputGradientTensor(_inputGradientTensor),
        spatialParameter(_spatialParameter),
        poolingParameter(0, 0, 0, 0, 0, 0, 0, 0),
        poolingGradientArray(0),
        targetOutLayout(_gradientTensor.createDefaultSubtensorLayout()),
        gradientTensor(_gradientTensor)
    {}

    Status init()
    {
        const size_t nDims = gradientTensor.getNumberOfDimensions();
        Collection<size_t> extractLayoutCollection(nDims);
        for(size_t i = 0; i < nDims; i++)
        {
            extractLayoutCollection[i] = i;
        }
        daal::services::internal::swap<cpu, size_t>(extractLayoutCollection[spatialParameter.indices.size[0]], extractLayoutCollection[nDims - 2]);
        daal::services::internal::swap<cpu, size_t>(extractLayoutCollection[spatialParameter.indices.size[1]], extractLayoutCollection[nDims - 1]);

        targetOutLayout.shuffleDimensions(extractLayoutCollection);
        gradientSliceDims = targetOutLayout.getDimensions();
        gradientSliceDims[0] = 1;

        poolingGradientArray.reset(gradientTensor.getSize() / gradientTensor.getDimensionSize(0));
        DAAL_CHECK_MALLOC(poolingGradientArray.get());
        Status s;
        poolingInputDimensions = HomogenNumericTable<algorithmFPType>::create(nDims, 1 , NumericTable::doAllocate, &s);
        DAAL_CHECK_STATUS_VAR(s);
        WriteOnlyRows<algorithmFPType, cpu> poolingInputDimensionsBD(*poolingInputDimensions, 0, 1);
        DAAL_CHECK_BLOCK_STATUS(poolingInputDimensionsBD);
        algorithmFPType *inputDimsSlice = poolingInputDimensionsBD.get();
        for(size_t i = 0; i < nDims; i++)
        {
            inputDimsSlice[i] = gradientSliceDims[i];
        }

        WriteOnlySubtensor<algorithmFPType, cpu, Tensor> gradientSubtensor(gradientTensor, 0, 0, 0, gradientTensor.getDimensionSize(0));
        DAAL_CHECK_BLOCK_STATUS(gradientSubtensor);
        algorithmFPType *gradientArray = gradientSubtensor.get();
        for(size_t i = 0; i < gradientTensor.getSize(); i++)
        {
            gradientArray[i] = 0;
        }
        return Status();
    }

    Status proceed();

    virtual Status getSlice(size_t slice)
    {
        return getInputGradientSlice(slice);
    }

    virtual Status getInputGradientSlice(size_t slice)
    {
        inputGradientSubtensor.set(const_cast<Tensor &>(inputGradientTensor), 0, 0, slice, 1);
        DAAL_CHECK_BLOCK_STATUS(inputGradientSubtensor);
        return Status();

    }

    Status prepareCommonPoolingTensors(size_t level, size_t offset)
    {
        inputGradientSliceDims = gradientSliceDims;
        inputGradientSliceDims[2] = ((size_t)1 << level);
        inputGradientSliceDims[3] = ((size_t)1 << level);

        Status s;
        const algorithmFPType *inputGradientData = inputGradientSubtensor.get() + offset;
        poolingInputGradientTensor = HomogenTensor<algorithmFPType>::create(inputGradientSliceDims, const_cast<algorithmFPType *>(inputGradientData), &s);
        DAAL_CHECK_STATUS_VAR(s);
        poolingGradientTensor = HomogenTensor<algorithmFPType>::create(gradientSliceDims, poolingGradientArray.get(), &s);
        return s;
    }

    virtual Status preparePoolingTensors(size_t level, size_t offset)
    {
        return prepareCommonPoolingTensors(level, offset);
    }

    Status preparePoolingParameter(size_t level)
    {
        size_t pow2 = (size_t)1 << level;
        poolingParameter.indices.size[0] = 2;
        poolingParameter.indices.size[1] = 3;
        poolingParameter.kernelSizes.size[0] = (gradientSliceDims[2] % pow2 == 0) ? gradientSliceDims[2] / pow2 : gradientSliceDims[2] / pow2 + 1;
        poolingParameter.kernelSizes.size[1] = (gradientSliceDims[3] % pow2 == 0) ? gradientSliceDims[3] / pow2 : gradientSliceDims[3] / pow2 + 1;
        poolingParameter.strides.size[0] = poolingParameter.kernelSizes.size[0];
        poolingParameter.strides.size[1] = poolingParameter.kernelSizes.size[1];
        poolingParameter.paddings.size[0] = (poolingParameter.kernelSizes.size[0] * pow2 - gradientSliceDims[2]) / 2;
        poolingParameter.paddings.size[1] = (poolingParameter.kernelSizes.size[1] * pow2 - gradientSliceDims[3]) / 2;
        return Status();
    }

    virtual Status computePooling() = 0;

    Status mergeToResult(const size_t slice);

    virtual ~BasePoolingTask() {}

protected:
    pooling2d::Parameter poolingParameter;
    const spatial_pooling2d::Parameter spatialParameter;
    Collection<size_t> inputGradientSliceDims;
    TensorPtr poolingInputGradientTensor;
    TensorPtr poolingGradientTensor;
    NumericTablePtr poolingInputDimensions;

private:
    const Tensor &inputGradientTensor;
    Tensor &gradientTensor;

    ReadSubtensor<algorithmFPType, cpu, Tensor> inputGradientSubtensor;
    Collection<size_t> gradientSliceDims;
    TensorOffsetLayout targetOutLayout;
    TArray<algorithmFPType, cpu> poolingGradientArray;
};

template<typename algorithmFPType, spatial_pooling2d::internal::Method method, CpuType cpu>
class DAAL_EXPORT PoolingTask : public BasePoolingTask<algorithmFPType, cpu>
{
public:
    PoolingTask(const Tensor &_inputGradientTensor,
                      Tensor &_gradientTensor,
                const Tensor &_selectedPosTensor,
                const spatial_pooling2d::Parameter &parameter) :
        BasePoolingTask<algorithmFPType, cpu>(_inputGradientTensor, _gradientTensor, parameter),
        selectedPosTensor(_selectedPosTensor) { }

    Status getSelectedPosSlice(size_t slice)
    {
        selectedPosSubtensor.set(const_cast<Tensor &>(selectedPosTensor), 0, 0, slice, 1);
        DAAL_CHECK_BLOCK_STATUS(selectedPosSubtensor);
        return Status();
    }

    Status getSlice(size_t slice) DAAL_C11_OVERRIDE
    {
        Status s;
        DAAL_CHECK_STATUS(s, this->getInputGradientSlice(slice));
        DAAL_CHECK_STATUS(s, getSelectedPosSlice(slice));
        return s;
    }

    Status preparePoolingTensors(size_t level, size_t offset) DAAL_C11_OVERRIDE
    {
        Status s;
        DAAL_CHECK_STATUS(s, this->prepareCommonPoolingTensors(level, offset));

        const algorithmFPType *selectedPosData = selectedPosSubtensor.get() + offset;
        poolingSelectedPosTensor = HomogenTensor<algorithmFPType>::create(this->inputGradientSliceDims,  const_cast<algorithmFPType *>(selectedPosData), &s);
        return s;
    }

    virtual Status computePooling() DAAL_C11_OVERRIDE;

    const Tensor &selectedPosTensor;
    ReadSubtensor<algorithmFPType, cpu, Tensor> selectedPosSubtensor;
    TensorPtr poolingSelectedPosTensor;
};

template<typename algorithmFPType, CpuType cpu>
class DAAL_EXPORT PoolingTask<algorithmFPType, spatial_pooling2d::internal::average, cpu> : public BasePoolingTask<algorithmFPType, cpu>
{
public:
    PoolingTask(const Tensor &_inputGradientTensor,
                      Tensor &_gradientTensor,
                const spatial_pooling2d::Parameter &parameter) :
        BasePoolingTask<algorithmFPType, cpu>(_inputGradientTensor, _gradientTensor, parameter) { }

    virtual Status computePooling() DAAL_C11_OVERRIDE;
};

} // internal
} // backward
} // spatial_pooling2d
} // layers
} // neural_networks
} // algorithms
} // daal

#endif
