/* file: kernel_func_lin_csr_batch.cpp */
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
!  Content:
!    C++ example of computing a linear kernel function in the batch processing mode
!
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-KERNEL_FUNCTION_LINEAR_CSR_BATCH"></a>
 * \example kernel_func_lin_csr_batch.cpp
 */

#include "daal.h"
#include "service.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms;

/* Input data set parameters */
string leftDatasetFileName  = "../data/batch/kernel_function_csr.csv";
string rightDatasetFileName = "../data/batch/kernel_function_csr.csv";

/* Kernel algorithm parameters */
const double k = 1.0;    /* Linear kernel coefficient in the k(X,Y) + b model */
const double b = 0.0;    /* Linear kernel coefficient in the k(X,Y) + b model */

int main(int argc, char *argv[])
{
    checkArguments(argc, argv, 1, &leftDatasetFileName);
    checkArguments(argc, argv, 1, &rightDatasetFileName);

    /* Read datasetFileName from a file and create a numeric tables to store input data */
    CSRNumericTablePtr leftData(createSparseTable<float>(leftDatasetFileName));
    CSRNumericTablePtr rightData(createSparseTable<float>(rightDatasetFileName));

    /* Create algorithm objects for the kernel algorithm using the default method */
    kernel_function::linear::Batch<float, kernel_function::linear::fastCSR> algorithm;

    /* Set the kernel algorithm parameter */
    algorithm.parameter.k = k;
    algorithm.parameter.b = b;
    algorithm.parameter.computationMode = kernel_function::matrixMatrix;

    /* Set an input data table for the algorithm */
    algorithm.input.set(kernel_function::X, leftData);
    algorithm.input.set(kernel_function::Y, rightData);

    /* Compute the linear kernel function */
    algorithm.compute();

    /* Get the computed results */
    kernel_function::ResultPtr result = algorithm.getResult();

    /* Print the results */
    printNumericTable(result->get(kernel_function::values), "Values");

    return 0;
}
