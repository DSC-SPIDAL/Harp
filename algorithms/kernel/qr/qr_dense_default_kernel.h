/* file: qr_dense_default_kernel.h */
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
//  Declaration of template function that calculate qrs.
//--
*/

#ifndef __QR_FPK_H__
#define __QR_FPK_H__

#include "qr_batch.h"
#include "kernel.h"
#include "numeric_table.h"

using namespace daal::data_management;

namespace daal
{
namespace algorithms
{
namespace qr
{
namespace internal
{

template<typename algorithmFPType, daal::algorithms::qr::Method method, CpuType cpu>
class QRBatchKernel : public Kernel
{
public:
    services::Status compute(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);

    services::Status compute_seq(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);

    services::Status compute_thr(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);

    services::Status compute_pcl(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);

};

template<typename algorithmFPType, daal::algorithms::qr::Method method, CpuType cpu>
class QROnlineKernel : public Kernel
{
public:
    services::Status compute(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);
    services::Status finalizeCompute(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);
};

template<typename algorithmFPType, daal::algorithms::qr::Method method, CpuType cpu>
class QRDistributedStep2Kernel : public Kernel
{
public:
    services::Status compute(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r0, NumericTable *r[], const daal::algorithms::Parameter *par = 0, data_management::KeyValueDataCollection *inCollection = NULL);

};

template<typename algorithmFPType, daal::algorithms::qr::Method method, CpuType cpu>
class QRDistributedStep3Kernel : public Kernel
{
public:
    services::Status compute(const size_t na, const NumericTable *const *a,
                        const size_t nr, NumericTable *r[], const daal::algorithms::Parameter *par = 0);

};

} // namespace daal::internal
}
}
} // namespace daal

#endif
