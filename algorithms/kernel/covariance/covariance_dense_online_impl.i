/* file: covariance_dense_online_impl.i */
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
//  Covariance matrix computation algorithm implementation in distributed mode
//--
*/

#ifndef __COVARIANCE_DENSE_ONLINE_IMPL_I__
#define __COVARIANCE_DENSE_ONLINE_IMPL_I__

#include "covariance_kernel.h"
#include "covariance_impl.i"

namespace daal
{
namespace algorithms
{
namespace covariance
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu, typename SumsArrayType>
services::Status prepareSums(NumericTable *dataTable,
                             algorithmFPType *&userSums,
                             ReadRows<algorithmFPType, cpu> &userSumsBlock,
                             SumsArrayType &userSumsArray)
{
    if (method == defaultDense)
    {
        userSumsArray.reset(dataTable->getNumberOfColumns());
        DAAL_CHECK_MALLOC(userSumsArray.get());

        userSums = userSumsArray.get();
    }
    else /* method == sumDense */
    {
        NumericTable *userSumsTable = dataTable->basicStatistics.get(NumericTable::sum).get();
        userSumsBlock.set(userSumsTable, 0, userSumsTable->getNumberOfRows());
        DAAL_CHECK_BLOCK_STATUS(userSumsBlock);

        userSums = const_cast<algorithmFPType *>(userSumsBlock.get());
    }

    return services::Status();
}


template<typename algorithmFPType, Method method, CpuType cpu>
services::Status CovarianceDenseOnlineKernel<algorithmFPType, method, cpu>::compute(
    NumericTable *dataTable,
    NumericTable *nObservationsTable,
    NumericTable *crossProductTable,
    NumericTable *sumTable,
    const Parameter *parameter)
{
    const size_t nFeatures  = dataTable->getNumberOfColumns();
    const size_t nVectors   = dataTable->getNumberOfRows();
    const bool isNormalized = dataTable->isNormalized(NumericTableIface::standardScoreNormalized);

    DEFINE_TABLE_BLOCK( WriteRows, sumBlock,           sumTable           );
    DEFINE_TABLE_BLOCK( WriteRows, crossProductBlock,  crossProductTable  );
    DEFINE_TABLE_BLOCK( WriteRows, nObservationsBlock, nObservationsTable );
    DEFINE_TABLE_BLOCK( ReadRows,  dataBlock,          dataTable          );

    algorithmFPType *sums          = sumBlock.get();
    algorithmFPType *crossProduct  = crossProductBlock.get();
    algorithmFPType *nObservations = nObservationsBlock.get();
    algorithmFPType *data          = const_cast<algorithmFPType*>(dataBlock.get());

    services::Status status;
    if (method == singlePassDense)
    {
        status |= updateDenseCrossProductAndSums<algorithmFPType, method, cpu>(
            isNormalized, nFeatures, nVectors, data, crossProduct, sums, nObservations);
        DAAL_CHECK_STATUS_VAR(status);
    }
    else
    {
        algorithmFPType partialNObservations = 0.0;

        algorithmFPType                   *userSums;
        ReadRows<algorithmFPType, cpu>     userSumsBlock;
        TArrayCalloc<algorithmFPType, cpu> userSumsArray;

        status |= prepareSums<algorithmFPType, method, cpu>(dataTable, userSums, userSumsBlock, userSumsArray);
        DAAL_CHECK_STATUS_VAR(status);

        TArrayCalloc<algorithmFPType, cpu> partialCrossProductArray(nFeatures * nFeatures);
        DAAL_CHECK_MALLOC(partialCrossProductArray.get());
        algorithmFPType *partialCrossProduct = partialCrossProductArray.get();

        status |= updateDenseCrossProductAndSums<algorithmFPType, method, cpu>(
            isNormalized, nFeatures, nVectors, data, partialCrossProduct, userSums, &partialNObservations);
        DAAL_CHECK_STATUS_VAR(status);

        mergeCrossProductAndSums<algorithmFPType, cpu>(nFeatures, partialCrossProduct, userSums,
            &partialNObservations, crossProduct, sums, nObservations);
    }

    return status;
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status CovarianceDenseOnlineKernel<algorithmFPType, method, cpu>::finalizeCompute(
    NumericTable *nObservationsTable,
    NumericTable *crossProductTable,
    NumericTable *sumTable,
    NumericTable *covTable,
    NumericTable *meanTable,
    const Parameter *parameter)
{
    return finalizeCovariance<algorithmFPType, cpu>(nObservationsTable,
        crossProductTable, sumTable, covTable, meanTable, parameter);
}

} // internal
} // covariance
} // algorithms
} // daal

#endif
