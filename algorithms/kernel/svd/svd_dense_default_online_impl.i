/* file: svd_dense_default_online_impl.i */
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
//  Implementation of svds
//--
*/

#ifndef __SVD_KERNEL_ONLINE_IMPL_I__
#define __SVD_KERNEL_ONLINE_IMPL_I__

#include "service_memory.h"
#include "service_math.h"
#include "service_defines.h"
#include "service_numeric_table.h"

#include "svd_dense_default_kernel.h"
#include "svd_dense_default_impl.i"

#include "threading.h"

using namespace daal::internal;
using namespace daal::services::internal;

namespace daal
{
namespace algorithms
{
namespace svd
{
namespace internal
{

template <typename algorithmFPType, daal::algorithms::svd::Method method, CpuType cpu>
Status SVDOnlineKernel<algorithmFPType, method, cpu>::compute(const size_t na, const NumericTable *const *a,
    const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par)
{
    size_t i, j;
    svd::Parameter defaultParams;
    const svd::Parameter *svdPar = &defaultParams;

    if ( par != 0 )
    {
        svdPar = static_cast<const svd::Parameter *>(par);
    }

    NumericTable *ntAi    = const_cast<NumericTable *>(a[0]);
    NumericTable *ntAux2i = r[1];

    size_t  n = ntAi->getNumberOfColumns();
    size_t  m = ntAi->getNumberOfRows();

    ReadRows<algorithmFPType, cpu, NumericTable> AiBlock(ntAi, 0, m); /* Ai [m][n] */
    DAAL_CHECK_BLOCK_STATUS(AiBlock);
    const algorithmFPType *Ai = AiBlock.get();

    WriteOnlyRows<algorithmFPType, cpu, NumericTable> Aux2iBlock(ntAux2i, 0, n); /* Aux2i = Ri [n][n] */
    DAAL_CHECK_BLOCK_STATUS(Aux2iBlock);
    algorithmFPType *Aux2i = Aux2iBlock.get();

    TArray<algorithmFPType, cpu> Aux1iTPtr(n * m);
    TArray<algorithmFPType, cpu> Aux2iTPtr(n * n);
    algorithmFPType *Aux1iT = Aux1iTPtr.get();
    algorithmFPType *Aux2iT = Aux2iTPtr.get();

    DAAL_CHECK(Aux1iT && Aux2iT, ErrorMemoryAllocationFailed);

    for ( i = 0 ; i < n ; i++ )
    {
        for ( j = 0 ; j < m; j++ )
        {
            Aux1iT[i * m + j] = Ai[i + j * n];
        }
    }

    const auto ec = compute_QR_on_one_node<algorithmFPType, cpu>( m, n, Aux1iT, m, Aux2iT, n );
    if(!ec)
        return ec;

    if (svdPar->leftSingularMatrix == requiredInPackedForm)
    {
        WriteOnlyRows<algorithmFPType, cpu, NumericTable> Aux1iBlock(r[0], 0, n); /* Aux1i = Qin[m][n] */
        DAAL_CHECK_BLOCK_STATUS(Aux1iBlock);
        algorithmFPType *Aux1i = Aux1iBlock.get();

        for ( i = 0 ; i < n ; i++ )
        {
            for ( j = 0 ; j < m; j++ )
            {
                Aux1i[i + j * n] = Aux1iT[i * m + j];
            }
        }
    }

    for ( i = 0 ; i < n ; i++ )
    {
        for ( j = 0 ; j <= i; j++ )
        {
            Aux2i[i + j * n] = Aux2iT[i * n + j];
        }
        for (     ; j < n; j++ )
        {
            Aux2i[i + j * n] = 0.0;
        }
    }

    return Status();
}

template <typename algorithmFPType, daal::algorithms::svd::Method method, CpuType cpu>
Status SVDOnlineKernel<algorithmFPType, method, cpu>::finalizeCompute(const size_t na, const NumericTable *const *a,
                                                                    const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par)
{
    svd::Parameter defaultParams;
    const svd::Parameter *svdPar = &defaultParams;

    if ( par != 0 )
    {
        svdPar = static_cast<const svd::Parameter *>(par);
    }

    const NumericTable *ntAux2_0 = a[0];
    NumericTable       *ntSigma  = r[0];
    NumericTable       *ntV      = r[2];

    size_t nBlocks = na / 2;
    size_t n       = ntAux2_0->getNumberOfColumns();

    /* Step 2 */
    const NumericTable *const *step2ntIn = a;

    TArray<NumericTable*, cpu> step2ntOut(nBlocks + 2);
    DAAL_CHECK(step2ntOut.get(), ErrorMemoryAllocationFailed);

    step2ntOut[0] = ntSigma;
    step2ntOut[1] = ntV;
    Status st;
    if (svdPar->leftSingularMatrix == requiredInPackedForm)
    {
        for(size_t k = 0; k < nBlocks; k++)
        {
            step2ntOut[2 + k] = new HomogenNumericTableCPU<algorithmFPType, cpu>(n, n, st);
            DAAL_CHECK_STATUS_VAR(st);
            if(!step2ntOut[2 + k])
            {
                for(size_t j = 0; j < k; j++)
                    delete step2ntOut[2 + j];
                return Status(ErrorMemoryAllocationFailed);
            }
        }
    }

    SVDDistributedStep2Kernel<algorithmFPType, method, cpu> kernel;
    Status s = kernel.compute( nBlocks, step2ntIn, nBlocks + 2, step2ntOut.get(), par );

    /* Step 3 */

    if(s && svdPar->leftSingularMatrix == requiredInPackedForm)
    {
        WriteOnlyRows<algorithmFPType, cpu, NumericTable> QBlock; /* Aux1i = Qin[m][n] */

        size_t computedRows = 0;

        for (size_t i = 0; i < nBlocks; i++)
        {
            const NumericTable *ntAux1i = a[nBlocks + i];
            size_t m = ntAux1i->getNumberOfRows();

            algorithmFPType *Qi = QBlock.set( r[1], computedRows, m );
            s = QBlock.status();
            if(!s)
                break;

            HomogenNumericTableCPU<algorithmFPType, cpu> ntQi   (Qi, n, m, s);
            DAAL_CHECK_STATUS_VAR(s);

            const NumericTable *step3ntIn[2] = {ntAux1i, step2ntOut[2 + i]};
            NumericTable *step3ntOut[1] = {&ntQi};

            SVDDistributedStep3Kernel<algorithmFPType, method, cpu> kernelStep3;
            s = kernelStep3.compute(2, step3ntIn, 1, step3ntOut, par);

            if(!s)
                break;

            computedRows += m;
        }
    }

    if(svdPar->leftSingularMatrix == requiredInPackedForm)
    {
        for(size_t k = 0; k < nBlocks; k++)
            delete step2ntOut[2 + k];
    }

    return s;
}

} // namespace daal::internal
}
}
} // namespace daal

#endif
