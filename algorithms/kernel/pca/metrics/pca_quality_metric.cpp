/* file: pca_quality_metric.cpp */
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
//  Implementation of pca quality metric methods.
//--
*/

#include "pca_quality_metric_set_batch.h"
#include "pca_explained_variance_default_batch_container.h"
#include "algorithms/pca/pca_explained_variance_batch.h"
#include "daal_defines.h"
#include "serialization_utils.h"
#include "daal_strings.h"

using namespace daal::data_management;
using namespace daal::services;


namespace daal
{
namespace algorithms
{
namespace pca
{
namespace quality_metric_set
{
namespace interface1
{

Parameter::Parameter(size_t nComponents, size_t nFeatures):
        nComponents(nComponents), nFeatures(nFeatures) {}

services::Status Parameter::check() const
{
    return services::Status();
}

void Batch::initializeQualityMetrics()
{
    inputAlgorithms[explainedVariancesMetrics] = SharedPtr<pca::quality_metric::explained_variance::Batch<> >(
        new pca::quality_metric::explained_variance::Batch<>(parameter.nFeatures, parameter.nComponents));
    _inputData->add(explainedVariancesMetrics, pca::quality_metric::explained_variance::InputPtr(
        new pca::quality_metric::explained_variance::Input));
}

/**
 * Returns the result of the quality metrics algorithm
 * \param[in] id   Identifier of the result
 * \return         Result that corresponds to the given identifier
 */
algorithms::ResultPtr ResultCollection::getResult(QualityMetricId id) const
{
    return staticPointerCast<Result, SerializationIface>((*this)[(size_t)id]);
}

/**
 * Returns the input object of the quality metrics algorithm
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
algorithms::InputPtr InputDataCollection::getInput(QualityMetricId id) const
{
    return algorithms::quality_metric_set::InputDataCollection::getInput((size_t)id);
}

}//namespace interface1
}//namespace quality_metric_set

namespace quality_metric
{
namespace explained_variance
{
namespace interface1
{

__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_PCA_QUALITY_METRIC_RESULT_ID);

Parameter::Parameter(size_t nFeatures, size_t nComponents): nFeatures(nFeatures), nComponents(nComponents) {}
services::Status Parameter::check() const
{
    return services::Status();
}

/** Default constructor */
Input::Input() : daal::algorithms::Input(lastInputId + 1) {}

/**
* Returns an input object for pca quality metric
* \param[in] id    Identifier of the input object
* \return          %Input object that corresponds to the given identifier
*/
data_management::NumericTablePtr Input::get(InputId id) const
{
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>(Argument::get(id));
}

/**
* Sets an input object for pca quality metric
* \param[in] id      Identifier of the input object
* \param[in] value   Pointer to the object
*/
void Input::set(InputId id, const data_management::NumericTablePtr &value)
{
    Argument::set(id, value);
}


/**
* Checks an input object for the pca algorithm
* \param[in] par     Algorithm parameter
* \param[in] method  Computation method
    */
services::Status Input::check(const daal::algorithms::Parameter *par, int method) const
{
    services::Status s;
    DAAL_CHECK(Argument::size() == 1, ErrorIncorrectNumberOfInputNumericTables);

    NumericTablePtr eigenValuesPtr = get(eigenvalues);
    DAAL_CHECK_EX(eigenValuesPtr.get(), ErrorNullInputNumericTable, ArgumentName, eigenvaluesStr());
    size_t nColumnsEigenValues = eigenValuesPtr->getNumberOfColumns();
    DAAL_CHECK_STATUS(s, checkNumericTable(eigenValuesPtr.get(), eigenvaluesStr(), packed_mask, 0, nColumnsEigenValues, 1));

    const Parameter* parameter = static_cast<const Parameter *>(par);
    size_t nComponents = parameter->nComponents == 0 ? nColumnsEigenValues : parameter->nComponents;
    size_t nFeatures = parameter->nFeatures == 0 ? nColumnsEigenValues : parameter->nFeatures;

    DAAL_CHECK(nComponents <= nColumnsEigenValues, services::ErrorIncorrectNComponents);
    DAAL_CHECK_EX(nFeatures == nColumnsEigenValues, services::ErrorIncorrectParameter, ArgumentName, nFeaturesStr());

    return s;
}

Result::Result() : daal::algorithms::Result(lastResultId + 1) {};

/**
* Returns the result of pca quality metrics
* \param[in] id    Identifier of the result
* \return          Result that corresponds to the given identifier
*/
data_management::NumericTablePtr Result::get(ResultId id) const
{
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>(Argument::get(id));
}

/**
* Sets the result of pca quality metrics
* \param[in] id      Identifier of the input object
* \param[in] value   %Input object
*/
void Result::set(ResultId id, const data_management::NumericTablePtr &value)
{
    Argument::set(id, value);
}


/**
 * Checks the result of pca quality metrics
 * \param[in] input   %Input object
 * \param[in] par     %Parameter of the algorithm
 * \param[in] method  Computation method
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    services::Status s;
    DAAL_CHECK(Argument::size() == 3, ErrorIncorrectNumberOfElementsInResultCollection);

    DAAL_CHECK_EX(get(explainedVariances).get(), ErrorNullOutputNumericTable, ArgumentName, explainedVariancesStr());
    DAAL_CHECK_EX(get(explainedVariancesRatios).get(), ErrorNullOutputNumericTable, ArgumentName, explainedVariancesRatiosStr());
    DAAL_CHECK_EX(get(noiseVariance).get(), ErrorNullOutputNumericTable, ArgumentName, noiseVarianceStr());

    const Parameter* parameter = (static_cast<const Parameter *>(par));
    size_t nComponents = parameter->nComponents == 0 ? static_cast<const Input *>(input)->get(eigenvalues)->getNumberOfColumns() : parameter->nComponents;

    DAAL_CHECK_STATUS(s, checkNumericTable(get(explainedVariances).get(), explainedVariancesStr(), packed_mask, 0, nComponents, 1));
    DAAL_CHECK_STATUS(s, checkNumericTable(get(explainedVariancesRatios).get(), explainedVariancesRatiosStr(), packed_mask, 0, nComponents, 1));
    DAAL_CHECK_STATUS(s, checkNumericTable(get(noiseVariance).get(), noiseVarianceStr(), packed_mask, 0, 1, 1));

    return s;
}

}//namespace interface1
}//namespace explained_variance
}//namespace quality_metric
}//namespace pca
}//namespace algorithms
}//namespace daal
