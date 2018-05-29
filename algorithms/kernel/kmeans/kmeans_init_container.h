/* file: kmeans_init_container.h */
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
//  Implementation of K-means algorithm container -- a class that contains
//  Lloyd K-means kernels for supported architectures.
//--
*/

#include "kmeans_init_types.h"
#include "kmeans_init_batch.h"
#include "kmeans_init_distributed.h"
#include "kmeans_init_kernel.h"
#include "kmeans_init_impl.h"

namespace daal
{
namespace algorithms
{
namespace kmeans
{
namespace init
{

template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::BatchContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status BatchContainer<algorithmFPType, method, cpu>::compute()
{
    Input *input = static_cast<Input *>(_in);
    Result *result = static_cast<Result *>(_res);

    const size_t na = 1;
    NumericTable *a[na];
    a[0] = static_cast<NumericTable *>(input->get(data).get());

    const size_t nr = 1;
    NumericTable *r[nr];
    r[0] = static_cast<NumericTable *>(result->get(centroids).get());

    Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    __DAAL_CALL_KERNEL(env, internal::KMeansinitKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute, na, a, nr, r, par, *par->engine);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step1Local, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitStep1LocalKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step1Local, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step1Local, algorithmFPType, method, cpu>::compute()
{
    NumericTable* pData = static_cast<Input *>(_in)->get(data).get();
    PartialResult *pRes = static_cast<PartialResult *>(_pres);

    NumericTablePtr pPartialClusters = pRes->get(partialClusters);
    NumericTable* pNumPartialClusters = pRes->get(partialClustersNumber).get();
    Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;
    services::Status s = __DAAL_CALL_KERNEL_STATUS(env, internal::KMeansinitStep1LocalKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute, pData, par,
        pNumPartialClusters, pPartialClusters, *par->engine);
    static_cast<PartialResult *>(_pres)->set(partialClusters, pPartialClusters); //can be null
    return s;
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step1Local, algorithmFPType, method, cpu>::finalizeCompute()
{
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Master, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitStep2MasterKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Master, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Master, algorithmFPType, method, cpu>::compute()
{
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Master, algorithmFPType, method, cpu>::finalizeCompute()
{
    DistributedStep2MasterInput *input = static_cast<DistributedStep2MasterInput *>(_in);
    Result *result = static_cast<Result *>(_res);
    data_management::DataCollection *dcInput = input->get(partialResults).get();

    size_t nPartials = dcInput->size();

    size_t na = nPartials * 2;
    NumericTable **a = new NumericTable*[na];
    for(size_t i = 0; i < nPartials; i++)
    {
        PartialResult *inPres = static_cast<PartialResult *>((*dcInput)[i].get());
        a[i * 2 + 0] = static_cast<NumericTable *>(inPres->get(partialClustersNumber).get());
        a[i * 2 + 1] = static_cast<NumericTable *>(inPres->get(partialClusters).get());
    }

    NumericTable* ntClusters = static_cast<NumericTable *>(result->get(centroids).get());

    Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    services::Status s = __DAAL_CALL_KERNEL_STATUS(env, internal::KMeansinitStep2MasterKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType),
        finalizeCompute, na, a, ntClusters, par);

    delete[] a;

    dcInput->clear();
    return s;
}

/////////////////////////////// init plusPlus/parallelPlus distributed containers ///////////////////////////////////////////////
template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Local, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitStep2LocalKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Local, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Local, algorithmFPType, method, cpu>::compute()
{
    const DistributedStep2LocalPlusPlusParameter* par = (const DistributedStep2LocalPlusPlusParameter*)(_par);
    DistributedStep2LocalPlusPlusInput *input = static_cast<DistributedStep2LocalPlusPlusInput *>(_in);
    const NumericTable* pData = input->get(data).get();
    const NumericTable* pNewCenters = input->get(inputOfStep2).get();
    DistributedStep2LocalPlusPlusPartialResult *pPartRes = static_cast<DistributedStep2LocalPlusPlusPartialResult *>(_pres);
    NumericTable* pRes = pPartRes->get(outputOfStep2ForStep3).get();
    DataCollectionPtr pLocalData = (par->firstIteration ? pPartRes->get(internalResult) : input->get(internalInput));
    NumericTable* aLocalData[internal::localDataSize] = { 0, 0, 0, 0 };
    for(size_t i = 0; i < pLocalData->size(); ++i)
        aLocalData[i] = NumericTable::cast((*pLocalData)[i]).get();

    NumericTable* pOutputForStep5 = (isParallelPlusMethod(method) && par->outputForStep5Required ?
        pPartRes->get(outputOfStep2ForStep5).get() : nullptr);

    __DAAL_CALL_KERNEL(env, internal::KMeansinitStep2LocalKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute,
        par, pData, pNewCenters, aLocalData, pRes, pOutputForStep5);
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Local, algorithmFPType, method, cpu>::finalizeCompute()
{
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step3Master, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitStep3MasterKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step3Master, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step3Master, algorithmFPType, method, cpu>::compute()
{
    DistributedStep3MasterPlusPlusInput* input = static_cast<DistributedStep3MasterPlusPlusInput*>(_in);
    DistributedStep3MasterPlusPlusPartialResult* pr = static_cast<DistributedStep3MasterPlusPlusPartialResult *>(_pres);
    data_management::MemoryBlock* pRngState = dynamic_cast<data_management::MemoryBlock*>(pr->get(rngState).get());
    const Parameter *par = (const Parameter*)(_par);
    __DAAL_CALL_KERNEL(env, internal::KMeansinitStep3MasterKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute,
        par, input->get(inputOfStep3FromStep2).get(), pRngState, pr->get(outputOfStep3ForStep4).get(), *par->engine);
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step3Master, algorithmFPType, method, cpu>::finalizeCompute()
{
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step4Local, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitStep4LocalKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step4Local, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step4Local, algorithmFPType, method, cpu>::compute()
{
    const Parameter* par = (const Parameter*)(_par);
    DistributedStep4LocalPlusPlusInput *input = static_cast<DistributedStep4LocalPlusPlusInput *>(_in);
    const NumericTable* pData = input->get(data).get();
    const NumericTable* pInput = input->get(inputOfStep4FromStep3).get();
    DistributedStep4LocalPlusPlusPartialResult *pPartRes = static_cast<DistributedStep4LocalPlusPlusPartialResult *>(_pres);
    NumericTable* pOutput = pPartRes->get(outputOfStep4).get();
    DataCollectionPtr pLocalData = input->get(internalInput);
    NumericTable* aLocalData[internal::localDataSize] = { 0, 0, 0, 0 };
    for(size_t i = 0; i < pLocalData->size(); ++i)
        aLocalData[i] = NumericTable::cast((*pLocalData)[i]).get();

    __DAAL_CALL_KERNEL(env, internal::KMeansinitStep4LocalKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute,
        pData, pInput, aLocalData, pOutput);
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step4Local, algorithmFPType, method, cpu>::finalizeCompute()
{
    return services::Status();
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step5Master, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::KMeansinitStep5MasterKernel, method, algorithmFPType);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step5Master, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step5Master, algorithmFPType, method, cpu>::compute()
{
    const Parameter* par = (const Parameter*)(_par);
    DistributedStep5MasterPlusPlusInput *input = static_cast<DistributedStep5MasterPlusPlusInput*>(_in);
    const DataCollection* pCandidates = input->get(inputCentroids).get();
    const DataCollection* pRating = input->get(inputOfStep5FromStep2).get();
    DistributedStep5MasterPlusPlusPartialResult *pPartRes = static_cast<DistributedStep5MasterPlusPlusPartialResult *>(_pres);
    NumericTable* pResCand = pPartRes->get(candidates).get();
    NumericTable* pResWeights = pPartRes->get(weights).get();
    __DAAL_CALL_KERNEL(env, internal::KMeansinitStep5MasterKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute,
        pCandidates, pRating, pResCand, pResWeights);
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step5Master, algorithmFPType, method, cpu>::finalizeCompute()
{
    const Parameter* par = (const Parameter*)(_par);
    DistributedStep5MasterPlusPlusPartialResult *pPartRes = static_cast<DistributedStep5MasterPlusPlusPartialResult *>(_pres);
    DistributedStep5MasterPlusPlusInput *input = static_cast<DistributedStep5MasterPlusPlusInput*>(_in);
    NumericTable* ntCandidates = pPartRes->get(candidates).get();
    NumericTable* ntWeights = pPartRes->get(weights).get();
    data_management::MemoryBlock* pRngState = dynamic_cast<data_management::MemoryBlock*>(input->get(inputOfStep5FromStep3).get());
    Result *pRes = static_cast<Result *>(_res);
    __DAAL_CALL_KERNEL(env, internal::KMeansinitStep5MasterKernel, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), finalizeCompute,
        par, ntCandidates, ntWeights, pRngState, pRes->get(centroids).get(), *par->engine);
}

} // namespace daal::algorithms::kmeans::init
} // namespace daal::algorithms::kmeans
} // namespace daal::algorithms
} // namespace daal
