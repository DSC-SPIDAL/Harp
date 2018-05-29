/* file: logitboost_impl.i */
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
//  Functuons that are used in Logit Boost algorithm at training
//  and prediction stage
//--
*/

#ifndef __LOGITBOOST_IMPL_I__
#define __LOGITBOOST_IMPL_I__

#include <cmath>
#include "service_math.h"
#include "service_data_utils.h"

namespace daal
{
namespace algorithms
{
namespace logitboost
{
namespace internal
{

/**
 *  \brief Update additive function's F values.
 *         Step 2.b) of the Algorithm 6 from [1] (page 356).
 *
 *  \param dim[in]          Number of features
 *  \param n[in]            Number of observations
 *  \param nc[in]           Number of classes
 *  \param x[in]            Input dataset
 *  \param xbm[in]          Buffer manager associated with input dataset
 *  \param splitFeature[in] Indices of the split features
 *  \param splitPoint[in]   Split points
 *  \param lMean[in]        "left" average sof weighted responses
 *  \param rMean[in]        "right" averages of weighted responses
 *  \param F[out]           Additive function's values (column-major format:
 *                          values for the first sample come first,
 *                          for the second - second, etc)
 */
template<typename algorithmFPType, CpuType cpu>
void UpdateF(size_t dim, size_t n, size_t nc, const algorithmFPType *pred, algorithmFPType *F)
{
    const algorithmFPType inv_nc = 1.0 / (algorithmFPType)nc;
    const algorithmFPType coef = (algorithmFPType)(nc - 1) / (algorithmFPType)nc;

    for ( size_t i = 0; i < n; i++ )
    {
        for ( size_t j = 0; j < nc; j++ )
        {
            algorithmFPType rj = pred[j * n + i];
            algorithmFPType s = rj;

            for( size_t k = 0; k < j; k++ )
            {
                algorithmFPType r = pred[k * n + i];
                s += r;
            }

            for( size_t k = j + 1; k < nc; k++ )
            {
                algorithmFPType r = pred[k * n + i];
                s += r;
            }

            F[i * nc + j] += coef * ( rj - s * inv_nc );
        }
    }
}


/* infToBigValue routine "converts" input NaNs\Infs to output positive "big" value */
template<typename algorithmFPType> static inline algorithmFPType infToBigValue(algorithmFPType arg){ return arg; }
template<> inline float infToBigValue<float>(float arg)
{
    uint32_t iBigValue = 0x7e7fffff;

    if(((_daal_sp_union_t*)&arg)->bits.exponent == 0xFF) // infinite number (inf or nan)
    {
        return *(float*)&iBigValue;
    }
    else
    {
        return arg;
    }
}
template<> inline double infToBigValue<double>(double arg)
{
    uint64_t lBigValue = 0x7fefffffffffffff;

    if(((_daal_dp_union_t*)&arg)->bits.exponent == 0x7FF)   // infinite number (inf or nan)
    {
        return *(double*)&lBigValue;
    }
    else
    {
        return arg;
    }
}


/**
 *  \brief Update probailities matrix
 *
 *  \param nc[in]   Number of classes
 *  \param n[in]    Number of observations
 *  \param F[in]    Values of additive function
 *  \param P[out]   Probailities matrix of size nc x n
 */
template<typename algorithmFPType, CpuType cpu>
void UpdateP( size_t nc, size_t n, algorithmFPType *F, algorithmFPType *P, algorithmFPType *Fbuf )
{
    const algorithmFPType overflowThreshold = daal::services::internal::MaxVal<algorithmFPType>::get();

    for ( size_t i = 0; i < n; i++ )
    {
        daal::internal::Math<algorithmFPType,cpu>::vExp(nc, F + i * nc, Fbuf);
        algorithmFPType s = 0.0;

        for ( size_t j = 0; j < nc; j++ )
        {
            // if low accuracy exp() returns NaN\Inf - convert it to some positive big value
            Fbuf[j] = infToBigValue<algorithmFPType>(Fbuf[j]);
            s += Fbuf[j];
        }

        algorithmFPType invs = (algorithmFPType)1.0 / s;

       PRAGMA_IVDEP
       PRAGMA_VECTOR_ALWAYS
        for ( size_t j = 0; j < nc; j++ )
        {
            // Normalize probabilities
            P[j * n + i] = Fbuf[j] * invs;
        }

    }
}

} // namespace daal::algorithms::logitboost::internal
}
}
} // namespace daal

#endif
