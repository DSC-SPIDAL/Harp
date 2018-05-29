/* file: covariance_container.h */
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
//  Implementation of Covariance algorithm container.
//--
*/

#ifndef __COVARIANCE_CONTAINER_H__
#define __COVARIANCE_CONTAINER_H__

#include "kernel.h"
#include "covariance_batch.h"
#include "covariance_online.h"
#include "covariance_distributed.h"
#include "covariance_kernel.h"

#undef  __DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR
#define __DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(ComputeMethod, KernelClass)   \
    template<typename algorithmFPType, CpuType cpu>                                 \
    BatchContainer<algorithmFPType, ComputeMethod, cpu>::BatchContainer(            \
        daal::services::Environment::env *daalEnv)                                  \
    {                                                                               \
        __DAAL_INITIALIZE_KERNELS(KernelClass, algorithmFPType, ComputeMethod);      \
    }

#undef  __DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR
#define __DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(ComputeMethod)                 \
    template<typename algorithmFPType, CpuType cpu>                                 \
    BatchContainer<algorithmFPType, ComputeMethod, cpu>::~BatchContainer()          \
    {                                                                               \
        __DAAL_DEINITIALIZE_KERNELS();                                               \
    }

#undef  __DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE
#define __DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(ComputeMethod, KernelClass)       \
    template<typename algorithmFPType, CpuType cpu>                                 \
    services::Status BatchContainer<algorithmFPType, ComputeMethod, cpu>::compute() \
    {                                                                               \
        Result *result = static_cast<Result *>(_res);                               \
        Input *input = static_cast<Input *>(_in);                                   \
                                                                                    \
        NumericTable *dataTable = input->get(data).get();                           \
        NumericTable *covTable  = result->get(covariance).get();                    \
        NumericTable *meanTable = result->get(mean).get();                          \
                                                                                    \
        Parameter *parameter = static_cast<Parameter *>(_par);                      \
        daal::services::Environment::env &env = *_env;                              \
                                                                                    \
         __DAAL_CALL_KERNEL(env, KernelClass,                                       \
                    __DAAL_KERNEL_ARGUMENTS(algorithmFPType, ComputeMethod),        \
                    compute, dataTable, covTable, meanTable, parameter);            \
    }


#undef  __DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR
#define __DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(ComputeMethod, KernelClass)  \
    template<typename algorithmFPType, CpuType cpu>                                 \
    OnlineContainer<algorithmFPType, ComputeMethod, cpu>::OnlineContainer(          \
        daal::services::Environment::env *daalEnv)                                  \
    {                                                                               \
        __DAAL_INITIALIZE_KERNELS(KernelClass, algorithmFPType, ComputeMethod);     \
    }

#undef  __DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR
#define __DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(ComputeMethod)                \
    template<typename algorithmFPType, CpuType cpu>                                 \
    OnlineContainer<algorithmFPType, ComputeMethod, cpu>::~OnlineContainer()        \
    {                                                                               \
        __DAAL_DEINITIALIZE_KERNELS();                                              \
    }

#undef  __DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE
#define __DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(ComputeMethod, KernelClass)          \
    template<typename algorithmFPType, CpuType cpu>                                     \
    services::Status OnlineContainer<algorithmFPType, ComputeMethod, cpu>::compute()    \
    {                                                                                   \
        PartialResult *partialResult = static_cast<PartialResult *>(_pres);             \
        Input *input = static_cast<Input *>(_in);                                       \
                                                                                        \
        NumericTable *dataTable = input->get(data).get();                               \
                                                                                        \
        NumericTable *nObsTable         = partialResult->get(nObservations).get();      \
        NumericTable *crossProductTable = partialResult->get(crossProduct).get();       \
        NumericTable *sumTable          = partialResult->get(sum).get();                \
                                                                                        \
        Parameter *parameter = static_cast<Parameter *>(_par);                          \
        daal::services::Environment::env &env = *_env;                                  \
                                                                                        \
                                                                                        \
         __DAAL_CALL_KERNEL(env, KernelClass,                                           \
                   __DAAL_KERNEL_ARGUMENTS(algorithmFPType, ComputeMethod),             \
                   compute, dataTable, nObsTable, crossProductTable, sumTable,          \
                   parameter);                                                          \
    }

#undef  __DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE
#define __DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(ComputeMethod, KernelClass)       \
    template<typename algorithmFPType, CpuType cpu>                                          \
    services::Status OnlineContainer<algorithmFPType, ComputeMethod, cpu>::finalizeCompute() \
    {                                                                                        \
        PartialResult *partialResult = static_cast<PartialResult *>(_pres);                  \
        Result *result = static_cast<Result *>(_res);                                        \
                                                                                             \
        NumericTable *nObsTable         = partialResult->get(nObservations).get();           \
        NumericTable *crossProductTable = partialResult->get(crossProduct).get();            \
        NumericTable *sumTable          = partialResult->get(sum).get();                     \
                                                                                             \
        NumericTable *covTable  = result->get(covariance).get();                             \
        NumericTable *meanTable = result->get(mean).get();                                   \
                                                                                             \
        Parameter *parameter = static_cast<Parameter *>(_par);                               \
        daal::services::Environment::env &env = *_env;                                       \
                                                                                             \
         __DAAL_CALL_KERNEL(env, KernelClass,                                                \
                   __DAAL_KERNEL_ARGUMENTS(algorithmFPType, ComputeMethod),                  \
                   finalizeCompute, nObsTable, crossProductTable,                            \
                   sumTable, covTable, meanTable, parameter);                                \
    }

namespace daal
{
namespace algorithms
{
namespace covariance
{

__DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(defaultDense,    internal::CovarianceDenseBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(singlePassDense, internal::CovarianceDenseBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(sumDense,        internal::CovarianceDenseBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(fastCSR,         internal::CovarianceCSRBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(singlePassCSR,   internal::CovarianceCSRBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_CONSTRUCTOR(sumCSR,          internal::CovarianceCSRBatchKernel)

__DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(defaultDense)
__DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(singlePassDense)
__DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(sumDense)
__DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(fastCSR)
__DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(singlePassCSR)
__DAAL_COVARIANCE_BATCH_CONTAINER_DESTRUCTOR(sumCSR)

__DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(defaultDense,    internal::CovarianceDenseBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(singlePassDense, internal::CovarianceDenseBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(sumDense,        internal::CovarianceDenseBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(fastCSR,         internal::CovarianceCSRBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(singlePassCSR,   internal::CovarianceCSRBatchKernel)
__DAAL_COVARIANCE_BATCH_CONTAINER_COMPUTE(sumCSR,          internal::CovarianceCSRBatchKernel)


__DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(defaultDense,    internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(singlePassDense, internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(sumDense,        internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(fastCSR,         internal::CovarianceCSROnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(singlePassCSR,   internal::CovarianceCSROnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_CONSTRUCTOR(sumCSR,          internal::CovarianceCSROnlineKernel)

__DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(defaultDense)
__DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(singlePassDense)
__DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(sumDense)
__DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(fastCSR)
__DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(singlePassCSR)
__DAAL_COVARIANCE_ONLINE_CONTAINER_DESTRUCTOR(sumCSR)

__DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(defaultDense,    internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(singlePassDense, internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(sumDense,        internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(fastCSR,         internal::CovarianceCSROnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(singlePassCSR,   internal::CovarianceCSROnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_COMPUTE(sumCSR,          internal::CovarianceCSROnlineKernel)

__DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(defaultDense,    internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(singlePassDense, internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(sumDense,        internal::CovarianceDenseOnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(fastCSR,         internal::CovarianceCSROnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(singlePassCSR,   internal::CovarianceCSROnlineKernel)
__DAAL_COVARIANCE_ONLINE_CONTAINER_FINALIZECOMPUTE(sumCSR,          internal::CovarianceCSROnlineKernel)

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Master, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::CovarianceDistributedKernel, algorithmFPType, method);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Master, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Master, algorithmFPType, method, cpu>::compute()
{
    PartialResult *partialResult = static_cast<PartialResult *>(_pres);

    DistributedInput<step2Master> *input = static_cast<DistributedInput<step2Master> *>(_in);
    DataCollection *collection = input->get(partialResults).get();

    NumericTable *nObsTable         = partialResult->get(nObservations).get();
    NumericTable *crossProductTable = partialResult->get(crossProduct).get();
    NumericTable *sumTable          = partialResult->get(sum).get();

    Parameter *parameter = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

     __DAAL_CALL_KERNEL(env, internal::CovarianceDistributedKernel,
                       __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method),
                       compute, collection, nObsTable, crossProductTable, sumTable, parameter);

    collection->clear();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Master, algorithmFPType, method, cpu>::finalizeCompute()
{
    Result *result = static_cast<Result *>(_res);
    PartialResult *partialResult = static_cast<PartialResult *>(_pres);

    NumericTable *nObsTable         = partialResult->get(nObservations).get();
    NumericTable *crossProductTable = partialResult->get(crossProduct).get();
    NumericTable *sumTable          = partialResult->get(sum).get();

    NumericTable *covTable  = result->get(covariance).get();
    NumericTable *meanTable = result->get(mean).get();

    Parameter *parameter = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

     __DAAL_CALL_KERNEL(env, internal::CovarianceDistributedKernel,
                       __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method),
                       finalizeCompute, nObsTable, crossProductTable, sumTable, covTable, meanTable, parameter);
}

}
}
}

#endif
