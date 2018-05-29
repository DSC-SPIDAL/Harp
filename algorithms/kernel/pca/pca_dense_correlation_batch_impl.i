/* file: pca_dense_correlation_batch_impl.i */
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
//  Functuons that are used in PCA algorithm
//--
*/

#ifndef __PCA_DENSE_CORRELATION_BATCH_IMPL_I__
#define __PCA_DENSE_CORRELATION_BATCH_IMPL_I__

#include "service_math.h"
#include "service_memory.h"
#include "service_numeric_table.h"
#include "service_error_handling.h"
#include "threading.h"

namespace daal
{
namespace algorithms
{
namespace pca
{
namespace internal
{

using namespace daal::services::internal;
using namespace daal::data_management;
using namespace daal::internal;
template <typename algorithmFPType, CpuType cpu>
services::Status PCACorrelationKernel<batch, algorithmFPType, cpu>::compute
        (bool isCorrelation,
         const data_management::NumericTable& dataTable,
         covariance::BatchImpl* covarianceAlg,
         data_management::NumericTable& eigenvectors,
         data_management::NumericTable& eigenvalues)
{
    if(isCorrelation)
        return this->computeCorrelationEigenvalues(dataTable, eigenvectors, eigenvalues);
    DAAL_CHECK(covarianceAlg, services::ErrorNullPtr);
    services::Status status;
    covarianceAlg->parameter.outputMatrixType = covariance::correlationMatrix;

    DAAL_CHECK_STATUS(status, covarianceAlg->computeNoThrow());
    return this->computeCorrelationEigenvalues(*covarianceAlg->getResult()->get(covariance::covariance), eigenvectors, eigenvalues);
}

template <typename algorithmFPType, CpuType cpu>
services::Status PCACorrelationKernel<batch, algorithmFPType, cpu>::compute
        (bool isCorrelation,
         bool isDeterministic,
         const data_management::NumericTable& dataTable,
         covariance::BatchImpl* covarianceAlg,
         DAAL_UINT64 resultsToCompute,
         data_management::NumericTable& eigenvectors,
         data_management::NumericTable& eigenvalues,
         data_management::NumericTable& means,
         data_management::NumericTable& variances)
{
    services::Status status;
    if (isCorrelation)
    {
        if (resultsToCompute & mean)
        {
            DAAL_CHECK_STATUS(status, this->fillTable(means, (algorithmFPType)0));
        }

        if (resultsToCompute & variance)
        {
            DAAL_CHECK_STATUS(status, this->fillTable(variances, (algorithmFPType)1));
        }
        DAAL_CHECK_STATUS(status, this->computeCorrelationEigenvalues(dataTable, eigenvectors, eigenvalues));
    }
    else
    {
        DAAL_CHECK(covarianceAlg, services::ErrorNullPtr);
        DAAL_CHECK_STATUS(status, covarianceAlg->computeNoThrow());
        auto pCovarianceTable = covarianceAlg->getResult()->get(covariance::covariance);
        NumericTable& covarianceTable = *pCovarianceTable;
        if (resultsToCompute & mean)
        {
            DAAL_CHECK_STATUS(status, this->copyTable(*covarianceAlg->getResult()->get(covariance::mean), means));
        }

        if (resultsToCompute & variance)
        {
            DAAL_CHECK_STATUS(status, this->copyVarianceFromCovarianceTable(covarianceTable, variances));
        }

        DAAL_CHECK_STATUS(status, this->correlationFromCovarianceTable(covarianceTable));
        DAAL_CHECK_STATUS(status, this->computeCorrelationEigenvalues(covarianceTable, eigenvectors, eigenvalues));
    }

    if (isDeterministic)
    {
        DAAL_CHECK_STATUS(status, this->signFlipEigenvectors(eigenvectors));
    }

    return status;
}


} // namespace internal
} // namespace pca
} // namespace algorithms
} // namespace daal

#endif
