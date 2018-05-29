/* file: svm_predict_impl.i */
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
//  SVM prediction algorithm implementation
//--
*/

#ifndef __SVM_PREDICT_IMPL_I__
#define __SVM_PREDICT_IMPL_I__

#include "service_memory.h"
#include "service_numeric_table.h"
#include "service_blas.h"
#include "service_memory.h"

namespace daal
{
namespace algorithms
{
namespace svm
{
namespace prediction
{
namespace internal
{

using namespace daal::internal;
using namespace daal::services;
using namespace daal::services::internal;

template <typename algorithmFPType, CpuType cpu>
struct SVMPredictImpl<defaultDense, algorithmFPType, cpu> : public Kernel
{
    services::Status compute(const NumericTablePtr& xTable, const daal::algorithms::Model *m, NumericTable& r,
                             const daal::algorithms::Parameter *par)
    {
        const size_t nVectors = xTable->getNumberOfRows();
        WriteOnlyColumns<algorithmFPType, cpu> mtR(r, 0, 0, nVectors);
        DAAL_CHECK_BLOCK_STATUS(mtR);
        algorithmFPType *distance = mtR.get();

        Model *model = static_cast<Model *>(const_cast<daal::algorithms::Model *>(m));
        Parameter *parameter = static_cast<Parameter *>(const_cast<daal::algorithms::Parameter *>(par));

        NumericTablePtr svCoeffTable  = model->getClassificationCoefficients();
        const size_t nSV = svCoeffTable->getNumberOfRows();
        if(nSV == 0)
        {
            const algorithmFPType zero(0.0);
            for(size_t i = 0; i < nVectors; i++)
            {
                distance[i] = zero;
            }
            return Status();
        }

        const algorithmFPType bias(model->getBias());
        kernel_function::KernelIfacePtr kernel = parameter->kernel->clone();
        NumericTablePtr svTable = model->getSupportVectors();

        ReadColumns<algorithmFPType, cpu> mtSVCoeff(*svCoeffTable, 0, 0, nSV);
        DAAL_CHECK_BLOCK_STATUS(mtSVCoeff);
        const algorithmFPType *svCoeff = mtSVCoeff.get();

        TArray<algorithmFPType, cpu> aBuf(nSV * nVectors);
        DAAL_CHECK(aBuf.get(), ErrorMemoryAllocationFailed);
        algorithmFPType *buf = aBuf.get();

        Status s;
        NumericTablePtr shResNT = HomogenNumericTableCPU<algorithmFPType, cpu>::create(buf, nSV, nVectors, &s);
        DAAL_CHECK_STATUS_VAR(s);

        kernel_function::ResultPtr shRes(new kernel_function::Result());
        shRes->set(kernel_function::values, shResNT);
        kernel->setResult(shRes);
        kernel->getInput()->set(kernel_function::X, xTable);
        kernel->getInput()->set(kernel_function::Y, svTable);
        kernel->getParameter()->computationMode = kernel_function::matrixMatrix;
        s = kernel->computeNoThrow();
        if(!s)
            return Status(services::ErrorSVMPredictKernerFunctionCall).add(s);//this order is expected by test system

        char trans = 'T';
        DAAL_INT m_ = nSV;
        DAAL_INT n_ = nVectors;
        algorithmFPType alpha(1.0);
        DAAL_INT lda = m_;
        DAAL_INT incx(1);
        algorithmFPType beta(1.0);
        DAAL_INT incy(1);

        service_memset<algorithmFPType, cpu>(distance, bias, nVectors);

        Blas<algorithmFPType, cpu>::xgemv(&trans, &m_, &n_, &alpha, buf, &lda, svCoeff, &incx, &beta, distance, &incy);

        return s;
    }
};

} // namespace internal

} // namespace prediction

} // namespace svm

} // namespace algorithms

} // namespace daal

#endif
