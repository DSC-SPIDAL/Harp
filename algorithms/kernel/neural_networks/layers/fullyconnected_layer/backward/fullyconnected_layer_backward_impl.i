/* file: fullyconnected_layer_backward_impl.i */
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
//  Implementation of fullyconnected algorithm
//--
*/

#ifndef __FULLYCONNECTED_LAYER_BACKWARD_IMPL_I__
#define __FULLYCONNECTED_LAYER_BACKWARD_IMPL_I__

#include "threading.h"
#include "service_blas.h"
#include "service_tensor.h"

#define _DEFAULT_BLOCKSIZE 256
#define _SMALL_BLOCKSIZE   128

using namespace daal::internal;
using namespace daal::services;
using namespace daal::data_management;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace fullyconnected
{
namespace backward
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
Status FullyconnectedKernel<algorithmFPType, method, cpu>::compute(const Tensor &inGradTensor, const Tensor &xTensor, const Tensor &wTensor, Tensor &wDerTensor,
                                                                 Tensor &bDerTensor, Tensor &resultTensor, const fullyconnected::Parameter &parameter)
{
    typedef typename Blas<algorithmFPType, cpu>::SizeType BlasSize;

    size_t outs_num   = parameter.nOutputs;

    const services::Collection<size_t>& xDims = xTensor.getDimensions();
    const services::Collection<size_t>& wDims = wDerTensor.getDimensions();

    ReadSubtensor<algorithmFPType, cpu> inGradBlock(const_cast<Tensor &>(inGradTensor), 0, 0, 0, xDims[0]);
    DAAL_CHECK_BLOCK_STATUS(inGradBlock);
    const algorithmFPType *inGradArray = inGradBlock.get();

    ReadSubtensor<algorithmFPType, cpu> xBlock(const_cast<Tensor &>(xTensor), 0, 0, 0, xDims[0]);
    DAAL_CHECK_BLOCK_STATUS(xBlock);
    const algorithmFPType *xArray = xBlock.get();

    WriteOnlySubtensor<algorithmFPType, cpu> wDerBlock(wDerTensor, 0, 0, 0, wDims[0]);
    DAAL_CHECK_BLOCK_STATUS(wDerBlock);
    algorithmFPType *wDerArray = wDerBlock.get();

    WriteOnlySubtensor<algorithmFPType, cpu> bDerBlock(bDerTensor, 0, 0, 0, outs_num);
    DAAL_CHECK_BLOCK_STATUS(bDerBlock);
    algorithmFPType *bDerArray = bDerBlock.get();

    size_t nDims      = xDims.size();
    size_t batch_size = xDims[0];
    size_t data_size  = 1;
    for(size_t i=1; i<nDims; i++)
    {
        data_size *= xDims[i];
    }

    bool do_parallel;

    /* Filter out dimensions where manual threading is slower */
    if(
        ((outs_num >= 512) && (batch_size > 128 ) && (data_size <= 10000))
#if __CPUID__(DAAL_CPU) >= __avx512_mic__
        || ((outs_num > 2048) && (batch_size > 128 ) && (data_size > 50000))
#endif
      )
    {
        do_parallel   = 0;
    }
    else
    {
        do_parallel   = 1;
    }

    for(size_t i=0; i<outs_num; i++)
    {
        bDerArray[i] = (algorithmFPType)0.0;
    }

    algorithmFPType invBatchSize = 1.0 / batch_size;

    if( batch_size>10 && outs_num>100 )
    {
        daal::threader_for( batch_size, batch_size, [=](int j)
        {
            for(size_t i=0; i<outs_num; i++)
            {
                bDerArray[i] += inGradArray[j*outs_num + i] * invBatchSize;
            }
        } );
    }
    else
    {
        for(size_t j=0; j<batch_size; j++)
        {
            for(size_t i=0; i<outs_num; i++)
            {
                bDerArray[i] += inGradArray[j*outs_num + i] * invBatchSize;
            }
        }
    }

    if( do_parallel ) /* Manual threading for gemm calls */
    {
        algorithmFPType* _gArray = const_cast<algorithmFPType*>(inGradArray);
        algorithmFPType* _xArray = const_cast<algorithmFPType*>(xArray);

        BlasSize lda         = data_size;
        BlasSize ldb         = outs_num;
        BlasSize ldc         = data_size;
        algorithmFPType beta = 0.0;

        /* Calculate size of blocks */
        BlasSize blocksize     = (data_size > 10000)?_DEFAULT_BLOCKSIZE:_SMALL_BLOCKSIZE;
                 blocksize     = (blocksize < data_size)? blocksize:data_size;
        BlasSize blocknum      = data_size / blocksize;
        BlasSize lastblocksize = data_size - blocknum*blocksize;
        if( lastblocksize != 0 ) { blocknum++; }
        else                     { lastblocksize = blocksize; }

        /* Result gradients computation */
        if (parameter.propagateGradient)
        {
            ReadSubtensor<algorithmFPType, cpu> wBlock(const_cast<Tensor &>(wTensor), 0, 0, 0, wDims[0]);
            DAAL_CHECK_BLOCK_STATUS(wBlock);
            algorithmFPType *wArray = const_cast<algorithmFPType*>(wBlock.get());

            WriteOnlySubtensor<algorithmFPType, cpu> resultBlock(resultTensor, 0, 0, 0, batch_size);
            DAAL_CHECK_BLOCK_STATUS(resultBlock);
            algorithmFPType *resultArray = resultBlock.get();

            char transa           = 'n';
            char transb           = 'n';
            BlasSize _n           = batch_size;
            BlasSize _k           = outs_num;
            algorithmFPType alpha = 1.0;

            daal::threader_for( blocknum, blocknum, [ & ](size_t b)
            {
                BlasSize cursize_local = (b < (blocknum-1))?blocksize:lastblocksize;
                Blas<algorithmFPType, cpu>::xxgemm( &transa, &transb,
                                                    &cursize_local, &_n, &_k,
                                                    &alpha, wArray + b*blocksize, &lda,
                                                    _gArray, &ldb,
                                                    &beta, resultArray + b*blocksize, &ldc );
            } );
        }

        /* Weight derivatives computation */
        {
            char transa           = 'n';
            char transb           = 't';
            BlasSize _n           = outs_num;
            BlasSize _k           = batch_size;
            algorithmFPType alpha = invBatchSize;

            daal::threader_for( blocknum, blocknum, [ & ](size_t b)
            {
                BlasSize cursize_local = (b < (blocknum-1))?blocksize:lastblocksize;
                Blas<algorithmFPType, cpu>::xxgemm( &transa, &transb,
                                                    &cursize_local, &_n, &_k,
                                                    &alpha, _xArray + b*blocksize, &lda,
                                                    _gArray, &ldb,
                                                    &beta, wDerArray + b*blocksize, &ldc );
            } );
        }
    }  /* if( do_parallel ) */
    else
    {
        /* Result gradients computation */
        if (parameter.propagateGradient)
        {
            ReadSubtensor<algorithmFPType, cpu> wBlock(const_cast<Tensor &>(wTensor), 0, 0, 0, wDims[0]);
            DAAL_CHECK_BLOCK_STATUS(wBlock);
            algorithmFPType *wArray = const_cast<algorithmFPType*>(wBlock.get());

            WriteOnlySubtensor<algorithmFPType, cpu> resultBlock(resultTensor, 0, 0, 0, batch_size);
            DAAL_CHECK_BLOCK_STATUS(resultBlock);
            algorithmFPType *resultArray = resultBlock.get();

            char transa           = 'n';
            char transb           = 'n';
            BlasSize _m           = data_size;
            BlasSize _n           = batch_size;
            BlasSize _k           = outs_num;

            algorithmFPType alpha = 1.0;
            BlasSize lda          = data_size;
            BlasSize ldb          = outs_num;
            algorithmFPType beta  = 0.0;
            BlasSize ldc          = data_size;

            Blas<algorithmFPType, cpu>::xgemm( &transa, &transb,
                                               &_m, &_n, &_k,
                                               &alpha, wArray, &lda,
                                               const_cast<algorithmFPType*>(inGradArray), &ldb,
                                               &beta, resultArray, &ldc );
        }

        /* Weight derivatives computation */
        {
            char transa           = 'n';
            char transb           = 't';
            BlasSize _m           = data_size;
            BlasSize _n           = outs_num;
            BlasSize _k           = batch_size;

            algorithmFPType alpha = invBatchSize;
            BlasSize lda          = data_size;
            BlasSize ldb          = outs_num;
            algorithmFPType beta  = 0.0;
            BlasSize ldc          = data_size;

            Blas<algorithmFPType, cpu>::xgemm( &transa, &transb,
                                               &_m, &_n, &_k,
                                               &alpha, const_cast<algorithmFPType*>(xArray), &lda,
                                               const_cast<algorithmFPType*>(inGradArray), &ldb,
                                               &beta, wDerArray, &ldc );
        }
    }
    return Status();
}

} // internal
} // backward
} // namespace fullyconnected
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
