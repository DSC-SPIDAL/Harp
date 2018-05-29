/* file: zscore_impl.i */
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
//  Implementation of template function that calculates zscore normalization.
//--

#ifndef __ZSCORE_IMPL_I__
#define __ZSCORE_IMPL_I__

#include "zscore_base.h"

namespace daal
{
namespace algorithms
{
namespace normalization
{
namespace zscore
{
namespace internal
{
template<typename algorithmFPType, CpuType cpu>
Status ZScoreKernelBase<algorithmFPType, cpu>::common_compute(NumericTable &inputTable,
                                                              NumericTable &resultTable,
                                                              algorithmFPType* mean_total,
                                                              algorithmFPType* variances_total,
                                                              const daal::algorithms::Parameter &parameter)
{
#define _BLOCK_SIZE_NORM_ 256

    const size_t _nVectors = inputTable.getNumberOfRows();
    const size_t _nFeatures = inputTable.getNumberOfColumns();

    /* Split rows by blocks, block size cannot be less than _nVectors */
    size_t numRowsInBlock = (_nVectors > _BLOCK_SIZE_NORM_) ? _BLOCK_SIZE_NORM_ : _nVectors;
    /* Number of blocks */
    size_t numRowsBlocks = _nVectors / numRowsInBlock;
    /* Last block can be bigger than others */
    size_t numRowsInLastBlock = numRowsInBlock + (_nVectors - numRowsBlocks * numRowsInBlock);

    /* Check if input data are already normalized */
    if (inputTable.isNormalized(NumericTableIface::standardScoreNormalized))
    {
        SafeStatus safeStat;
        /* In case of non-inplace just copy input array to output */
        if (&inputTable != &resultTable)
        {
            daal::threader_for(numRowsBlocks, numRowsBlocks, [&](int iRowsBlock)
            {
                size_t _nRows = (iRowsBlock < (numRowsBlocks - 1)) ? numRowsInBlock : numRowsInLastBlock;
                size_t _startRow = iRowsBlock * numRowsInBlock;

                ReadRows<algorithmFPType, cpu, NumericTable> dataTableBD(inputTable, _startRow, _nRows);
                DAAL_CHECK_BLOCK_STATUS_THR(dataTableBD);
                const algorithmFPType* dataArray_local = dataTableBD.get();

                WriteOnlyRows<algorithmFPType, cpu, NumericTable> normDataTableBD(resultTable, _startRow, _nRows);
                DAAL_CHECK_BLOCK_STATUS_THR(normDataTableBD);
                algorithmFPType* normDataArray_local = normDataTableBD.get();

                for (int i = 0; i < _nRows; i++)
                {
                    PRAGMA_IVDEP
                        PRAGMA_VECTOR_ALWAYS
                        for (int j = 0; j < _nFeatures; j++)
                        {
                            normDataArray_local[i * _nFeatures + j] = dataArray_local[i * _nFeatures + j];
                        }
                }
            });

            resultTable.setNormalizationFlag(NumericTableIface::standardScoreNormalized);
        }

        return safeStat.detach();
    }

    /* Call method-specific function to compute means and variances */
    Status s;
    DAAL_CHECK_STATUS(s, computeMeanVariance_thr(inputTable, mean_total, variances_total, parameter));
    TArrayCalloc<algorithmFPType, cpu> inv_sigmas(_nFeatures);
    DAAL_CHECK_MALLOC(inv_sigmas.get());
    for(size_t j = 0; j < _nFeatures; ++j)
    {
        if(variances_total[j])
            inv_sigmas[j] = algorithmFPType(1.0) / Math<algorithmFPType, cpu>::sSqrt(variances_total[j]);
    }

    SafeStatus safeStat;
    /* Final normalization threaded loop */
    daal::threader_for(numRowsBlocks, numRowsBlocks, [&](int iRowsBlock)
    {
        size_t _nRows = (iRowsBlock < (numRowsBlocks - 1)) ? numRowsInBlock : numRowsInLastBlock;
        size_t _startRow = iRowsBlock * numRowsInBlock;

        ReadRows<algorithmFPType, cpu, NumericTable> dataTableBD(inputTable, _startRow, _nRows);
        DAAL_CHECK_BLOCK_STATUS_THR(dataTableBD);
        const algorithmFPType* dataArray_local = dataTableBD.get();

        WriteOnlyRows<algorithmFPType, cpu, NumericTable> normDataTableBD(resultTable, _startRow, _nRows);
        DAAL_CHECK_BLOCK_STATUS_THR(normDataTableBD);
        algorithmFPType* normDataArray_local = normDataTableBD.get();

        for (int i = 0; i < _nRows; i++)
        {
            PRAGMA_IVDEP
                PRAGMA_VECTOR_ALWAYS
                for (int j = 0; j < _nFeatures; j++)
                {
                    normDataArray_local[i * _nFeatures + j] = (dataArray_local[i * _nFeatures + j] - mean_total[j]) * inv_sigmas[j];
                }
        }
    });

    resultTable.setNormalizationFlag(NumericTableIface::standardScoreNormalized);

    return safeStat.detach();
}

template<typename algorithmFPType, CpuType cpu>
Status ZScoreKernelBase<algorithmFPType, cpu>::compute(NumericTable &inputTable, NumericTable &resultTable,
                                                       const daal::algorithms::Parameter &parameter)
{
    const size_t _nFeatures = inputTable.getNumberOfColumns();

    /* Internal arrays for mean and variance, initialized by zeros */
    TArrayCalloc<algorithmFPType, cpu> meanTotal(_nFeatures);
    algorithmFPType* mean_total = meanTotal.get();
    DAAL_CHECK(mean_total, ErrorMemoryAllocationFailed);

    TArrayCalloc<algorithmFPType, cpu> variancesTotal(_nFeatures);
    algorithmFPType* variances_total = variancesTotal.get();
    DAAL_CHECK(variances_total, ErrorMemoryAllocationFailed);

    return common_compute(inputTable, resultTable, mean_total, variances_total, parameter);
}

template<typename algorithmFPType, CpuType cpu>
Status ZScoreKernelBase<algorithmFPType, cpu>::compute(NumericTable &inputTable,
                                                       NumericTable &resultTable,
                                                       NumericTable &resultMeans,
                                                       NumericTable &resultVariances,
                                                       const daal::algorithms::Parameter &parameter)
{
    const size_t _nFeatures = inputTable.getNumberOfColumns();
    const daal::algorithms::normalization::zscore::interface2::BaseParameter *par =
            static_cast<const daal::algorithms::normalization::zscore::interface2::BaseParameter*>(&parameter);

    bool computeMeans = par->resultsToCompute & mean;
    bool computeVariances = par->resultsToCompute & variances;

    /* Internal arrays for mean and variance, initialized by zeros */
    TArray<algorithmFPType, cpu> meansTotalArray(computeMeans ? 0 : _nFeatures);
    TArray<algorithmFPType, cpu> variancesTotalArray(computeVariances ? 0 : _nFeatures);

    WriteOnlyRows<algorithmFPType, cpu> meansTotal;
    WriteOnlyRows<algorithmFPType, cpu> variancesTotal;

    algorithmFPType* mean_total = computeMeans ?
        meansTotal.set(resultMeans, 0, _nFeatures) :
        meansTotalArray.get();

    DAAL_CHECK(mean_total, ErrorMemoryAllocationFailed);

    algorithmFPType* variances_total = computeVariances ?
        variancesTotal.set(resultVariances, 0, _nFeatures) :
        variancesTotalArray.get();

    DAAL_CHECK(variances_total, ErrorMemoryAllocationFailed);

    return common_compute(inputTable, resultTable, mean_total, variances_total, parameter);
};

} // namespace internal
} // namespace zscore
} // namespace normalization
} // namespace algorithms
} // namespace daal

#endif
