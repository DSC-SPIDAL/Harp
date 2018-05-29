/* file: naivebayes_train_container.h */
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
//  Implementation of Naive Bayes algorithm container -- a class that contains
//  Naive Bayes kernels for supported architectures.
//--
*/

#include "multinomial_naive_bayes_training_batch.h"
#include "multinomial_naive_bayes_training_online.h"
#include "multinomial_naive_bayes_training_distributed.h"
#include "multinomial_naive_bayes_training_types.h"
#include "naivebayes_train_kernel.h"

namespace daal
{
namespace algorithms
{
namespace multinomial_naive_bayes
{
namespace training
{

template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::BatchContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::NaiveBayesBatchTrainKernel, algorithmFPType, method);
}

template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status BatchContainer<algorithmFPType, method, cpu>::compute()
{
    classifier::training::Input *input = static_cast<classifier::training::Input *>(_in);
    Result *result = static_cast<Result *>(_res);

    const NumericTable *data   = input->get(classifier::training::data).get();
    const NumericTable *labels = input->get(classifier::training::labels).get();

    Model *model = result->get(classifier::training::model).get();

    const Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    __DAAL_CALL_KERNEL(env, internal::NaiveBayesBatchTrainKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), compute, data, labels, model, par);
}

template<typename algorithmFPType, Method method, CpuType cpu>
OnlineContainer<algorithmFPType, method, cpu>::OnlineContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::NaiveBayesBatchTrainKernel, algorithmFPType, method);
}

template<typename algorithmFPType, Method method, CpuType cpu>
OnlineContainer<algorithmFPType, method, cpu>::~OnlineContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status OnlineContainer<algorithmFPType, method, cpu>::compute()
{
    classifier::training::Input *input = static_cast<classifier::training::Input *>(_in);
    PartialResult *partialResult = static_cast<PartialResult *>(_pres);
    size_t na = input->size();

    const NumericTable *data   = input->get(classifier::training::data).get();
    const NumericTable *labels = input->get(classifier::training::labels).get();

    PartialModel *pModel = partialResult->get(classifier::training::partialModel).get();

    const Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    __DAAL_CALL_KERNEL(env, internal::NaiveBayesOnlineTrainKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), compute, data, labels, pModel, par);
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status OnlineContainer<algorithmFPType, method, cpu>::finalizeCompute()
{
    Result *result = static_cast<Result *>(_res);
    PartialResult *partialResult = static_cast<PartialResult *>(_pres);

    PartialModel *pModel = partialResult->get(classifier::training::partialModel).get();
    Model        *rModel = result->get(classifier::training::model).get();

    const Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    __DAAL_CALL_KERNEL(env, internal::NaiveBayesOnlineTrainKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), finalizeCompute, pModel, rModel, par);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Master, algorithmFPType, method, cpu>::DistributedContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::NaiveBayesBatchTrainKernel, algorithmFPType, method);
}

template<typename algorithmFPType, Method method, CpuType cpu>
DistributedContainer<step2Master, algorithmFPType, method, cpu>::~DistributedContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Master, algorithmFPType, method, cpu>::compute()
{
    DistributedInput *input = static_cast<DistributedInput *>(_in);
    PartialResult *partialResult = static_cast<PartialResult *>(_pres);

    data_management::DataCollection *models = input->get(partialModels).get();

    size_t na = models->size();

    PartialModel **a = new PartialModel*[na];

    for(size_t i = 0; i < na; i++)
    {
        a[i] = static_cast<PartialModel *>((*models)[i].get());
    }

    PartialModel *pModel = partialResult->get(classifier::training::partialModel).get();

    const Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    services::Status s = __DAAL_CALL_KERNEL_STATUS(env, internal::NaiveBayesDistributedTrainKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), merge, na, a, pModel, par);

    models->clear();
    delete[] a;
    return s;
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status DistributedContainer<step2Master, algorithmFPType, method, cpu>::finalizeCompute()
{
    Result *result = static_cast<Result *>(_res);
    PartialResult *partialResult = static_cast<PartialResult *>(_pres);

    PartialModel *pModel = partialResult->get(classifier::training::partialModel).get();
    Model        *rModel = result->get(classifier::training::model).get();

    const Parameter *par = static_cast<Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    __DAAL_CALL_KERNEL(env, internal::NaiveBayesDistributedTrainKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), finalizeCompute, pModel,
                       rModel, par);
}

} // namespace training
} // namespace multinomial_naive_bayes
} // namespace algorithms
} // namespace daal
