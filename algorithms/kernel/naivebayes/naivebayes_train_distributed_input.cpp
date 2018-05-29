/* file: naivebayes_train_distributed_input.cpp */
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
//  Implementation of input for multinomial naive bayes training algorithm
//  in distributed computing mode.
//--
*/

#include "multinomial_naive_bayes_training_types.h"
#include "daal_strings.h"

namespace daal
{
namespace algorithms
{
namespace multinomial_naive_bayes
{
namespace training
{
namespace interface1
{
using namespace daal::data_management;
using namespace daal::services;

DistributedInput::DistributedInput() : classifier::training::InputIface(lastStep2MasterInputId + 1)
{
    Argument::set(partialModels, DataCollectionPtr(new DataCollection()));
}

size_t DistributedInput::getNumberOfFeatures() const
{
    DataCollectionPtr models = get(partialModels);
    if (!models) return 0;
    PartialModelPtr firstModel = multinomial_naive_bayes::PartialModel::cast((*models)[0]);
    if (!firstModel) return 0;
    return firstModel->getNFeatures();
}

/**
 * Returns input objects of the classification algorithm in the distributed processing mode
 * \param[in] id    Identifier of the input objects
 * \return          Input object that corresponds to the given identifier
 */
DataCollectionPtr DistributedInput::get(Step2MasterInputId id) const
{
    return DataCollection::cast(Argument::get(id));
}

/**
 * Adds input object on the master node in the training stage of the classification algorithm
 * \param[in] id            Identifier of the input object
 * \param[in] partialResult Pointer to the object
 */
void DistributedInput::add(const Step2MasterInputId &id, const PartialResultPtr &partialResult)
{
    DataCollectionPtr collection = get(id);
    if (!collection) return;
    collection->push_back(partialResult->get(classifier::training::partialModel));
}

/**
 * Sets input object in the training stage of the classification algorithm
 * \param[in] id   Identifier of the object
 * \param[in] value Pointer to the object
 */
void DistributedInput::set(Step2MasterInputId id, const DataCollectionPtr &value)
{
    Argument::set(id, value);
}

/**
 * Checks input parameters in the training stage of the classification algorithm
 * \param[in] parameter %Parameter of the algorithm
 * \param[in] method    Algorithm method
 */
Status DistributedInput::check(const daal::algorithms::Parameter *parameter, int method) const
{
    services::Status status;

    DataCollectionPtr collection = DataCollection::cast(get(partialModels));
    DAAL_CHECK_EX(collection, ErrorNullInputDataCollection, ArgumentName, inputCollectionStr());

    const size_t size = collection->size();
    DAAL_CHECK_EX(size > 0, ErrorEmptyInputCollection, ArgumentName, numberOfModelsStr());

    const multinomial_naive_bayes::Parameter *algPar = static_cast<const multinomial_naive_bayes::Parameter *>(parameter);
    size_t nFeatures=0;

    auto checkModel = [&](const SerializationIfacePtr &model) -> services::Status
    {
        services::Status s;
        DAAL_CHECK(model, ErrorNullModel);
        multinomial_naive_bayes::PartialModelPtr partialModel = multinomial_naive_bayes::PartialModel::cast(model);
        DAAL_CHECK(partialModel, ErrorIncorrectElementInPartialResultCollection);

        const size_t trainingDataFeatures = partialModel->getNFeatures();
        DAAL_CHECK(trainingDataFeatures, services::ErrorModelNotFullInitialized);

        if (!nFeatures)
        {
            nFeatures = trainingDataFeatures;
        }

        s |= checkNumericTable(partialModel->getClassSize().get(), classSizeStr(), 0, 0, 1, algPar->nClasses);
        s |= checkNumericTable(partialModel->getClassGroupSum().get(), groupSumStr(), 0, 0, nFeatures, algPar->nClasses);
        return s;
    };

    for (size_t i = 0; i < size; i++)
    {
        status|=checkModel((*collection)[i]);
    }
    return status;
}
} // namespace interface1
} // namespace training
} // namespace multinomial_naive_bayes
} // namespace algorithms
} // namespace daal
