/* file: java_batch.h */
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
//  Implementation of the class that connects Java Batch
//  to C++ algorithm
//--
*/
#ifndef __JAVA_BATCH_H__
#define __JAVA_BATCH_H__

#include <jni.h>
#include <tbb/tbb.h>

#include "algorithms/optimization_solver/objective_function/sum_of_functions_types.h"
#include "algorithms/optimization_solver/objective_function/sum_of_functions_batch.h"
#include "java_callback.h"
#include "java_batch_container.h"

namespace daal
{
namespace algorithms
{
namespace optimization_solver
{

using namespace daal::data_management;
using namespace daal::services;

namespace sum_of_functions
{

/*
 * \brief Class that specifies the default method for partial results initialization
 */
class JavaBatch : public sum_of_functions::Batch
{
public:
    /** Default constructor */
    JavaBatch(size_t numberOfTerms, JavaVM *_jvm, jobject _javaObject) : sum_of_functions::Batch(numberOfTerms, 0, 0)
    {
        JavaBatchContainer *_container = new JavaBatchContainer(_jvm, _javaObject);
        _container->setJavaResult(_result);
        _container->setEnvironment(&_env);

        this->_ac = _container;
    }

    JavaBatch(const JavaBatch &other) :
        sum_of_functions::Batch(other.sumOfFunctionsParameter->numberOfTerms, 0, 0)
    {
        JavaBatchContainer *_container = (static_cast<JavaBatchContainer *>(other._ac))->cloneImpl();
        sumOfFunctionsParameter = static_cast<sum_of_functions::Parameter *>(_container->_parameter);
        sumOfFunctionsInput = static_cast<sum_of_functions::Input *>(_container->_input);
        this->_par = sumOfFunctionsParameter;
        this->_in = sumOfFunctionsInput;
        _container->setJavaResult(_result);

        this->_ac = _container;
    }

    void setPointersToContainer(sum_of_functions::Input *ptrInput, sum_of_functions::Parameter *ptrParameter)
    {
        sumOfFunctionsParameter = ptrParameter;
        sumOfFunctionsInput = ptrInput;
        _in = ptrInput;
        _par = ptrParameter;
    }

    virtual ~JavaBatch() {}

    virtual int getMethod() const DAAL_C11_OVERRIDE { return 0; } // To make the class non-abstract

    virtual services::Status setResult(const objective_function::ResultPtr &result) DAAL_C11_OVERRIDE
    {
        _result = result;
        (static_cast<JavaBatchContainer *>(this->_ac))->setJavaResult(_result);
        _res = _result.get();
        return services::Status();
    }

protected:
    virtual services::Status allocateResult() DAAL_C11_OVERRIDE // To make the class non-abstract
    {
        services::Status s = _result->allocate<double>(_in, _par, 0);
        _res = _result.get();
        return s;
    }

    virtual JavaBatch *cloneImpl() const DAAL_C11_OVERRIDE
    {
        return new JavaBatch(*this);
    }
};

} // namespace daal::algorithms::optimization_solver::sum_of_functions
} // namespace daal::algorithms::optimization_solver
} // namespace daal::algorithms
} // namespace daal

#endif
