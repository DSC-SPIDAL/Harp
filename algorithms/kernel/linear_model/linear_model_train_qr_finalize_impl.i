/* file: linear_model_train_qr_finalize_impl.i */
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
//  Implementation of common base classes for normal equations model training.
//--
*/

#include "linear_model_train_qr_kernel.h"
#include "service_lapack.h"

namespace daal
{
namespace algorithms
{
namespace linear_model
{
namespace qr
{
namespace training
{
namespace internal
{
using namespace daal::services;
using namespace daal::data_management;
using namespace daal::internal;
using namespace daal::services::internal;

template <typename algorithmFPType, CpuType cpu>
Status FinalizeKernel<algorithmFPType, cpu>::compute(const NumericTable &rTable,
                                                     const NumericTable &qtyTable,
                                                     NumericTable &rFinalTable, NumericTable &qtyFinalTable,
                                                     NumericTable &betaTable, bool interceptFlag)
{
    const DAAL_INT nBetas    (betaTable.getNumberOfColumns());
    const DAAL_INT nResponses(betaTable.getNumberOfRows());
    const DAAL_INT nBetasIntercept = (interceptFlag ? nBetas : (nBetas - 1));

    TArrayScalable<algorithmFPType, cpu> betaBufferArray(nResponses * nBetasIntercept);
    algorithmFPType *betaBuffer = betaBufferArray.get();
    DAAL_CHECK_MALLOC(betaBuffer);

    {
        ReadRowsType rBlock(const_cast<NumericTable &>(rTable), 0, nBetasIntercept);
        DAAL_CHECK_BLOCK_STATUS(rBlock);
        const algorithmFPType *r = rBlock.get();

        {
            ReadRowsType qtyBlock(const_cast<NumericTable &>(qtyTable), 0, nResponses);
            DAAL_CHECK_BLOCK_STATUS(qtyBlock);
            const algorithmFPType *qty = qtyBlock.get();

            if (&rTable != &rFinalTable || &qtyTable != &qtyFinalTable)
            {
                WriteOnlyRowsType rFinalBlock(rFinalTable, 0, nBetasIntercept);
                DAAL_CHECK_BLOCK_STATUS(rFinalBlock);
                algorithmFPType *rFinal = rFinalBlock.get();

                WriteOnlyRowsType qtyFinalBlock(qtyFinalTable, 0, nResponses);
                DAAL_CHECK_BLOCK_STATUS(qtyFinalBlock);
                algorithmFPType *qtyFinal = qtyFinalBlock.get();

                const size_t   rSizeInBytes(sizeof(algorithmFPType) * nBetasIntercept * nBetasIntercept);
                const size_t qtySizeInBytes(sizeof(algorithmFPType) * nBetasIntercept * nResponses);

                daal_memcpy_s(  rFinal,   rSizeInBytes,   r,   rSizeInBytes);
                daal_memcpy_s(qtyFinal, qtySizeInBytes, qty, qtySizeInBytes);
            }

            for (size_t i = 0; i < nResponses; i++)
            {
              PRAGMA_IVDEP
              PRAGMA_VECTOR_ALWAYS
                for (size_t j = 0; j < nBetasIntercept; j++)
                {
                    betaBuffer[i * nBetasIntercept + j] = qty[j * nResponses + i];
                }
            }
        }

        /* Solve triangular linear system R'*beta = Y*Q' */
        DAAL_INT info(0);
        char up = 'U';
        char trans = 'T';
        char nodiag = 'N';
        Lapack<algorithmFPType, cpu>::xtrtrs(&up, &trans, &nodiag, const_cast<DAAL_INT *>(&nBetasIntercept),
                                             const_cast<DAAL_INT *>(&nResponses),
                                             const_cast<algorithmFPType *>(r),
                                             const_cast<DAAL_INT *>(&nBetasIntercept), betaBuffer,
                                             const_cast<DAAL_INT *>(&nBetasIntercept), &info);
        DAAL_CHECK(info == 0, services::ErrorLinearRegressionInternal);
    }

    WriteOnlyRowsType betaBlock(betaTable, 0, nResponses);
    DAAL_CHECK_BLOCK_STATUS(betaBlock);
    algorithmFPType *beta = betaBlock.get();

    if (nBetasIntercept == nBetas)
    {
        for(size_t i = 0; i < nResponses; i++)
        {
          PRAGMA_IVDEP
          PRAGMA_VECTOR_ALWAYS
            for(size_t j = 1; j < nBetas; j++)
            {
                beta[i * nBetas + j] = betaBuffer[i * nBetas + j - 1];
            }
            beta[i * nBetas] = betaBuffer[i * nBetas + nBetas - 1];
        }
    }
    else
    {
        for(size_t i = 0; i < nResponses; i++)
        {
          PRAGMA_IVDEP
          PRAGMA_VECTOR_ALWAYS
            for(size_t j = 0; j < nBetas - 1; j++)
            {
                beta[i * nBetas + j + 1] = betaBuffer[i * nBetasIntercept + j];
            }
            beta[i * nBetas] = 0.0;
        }
    }

    return Status();
}

}
}
}
}
}
}
