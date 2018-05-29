/* file: pca_cor_csr_distr.cpp */
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
!    C++ example of principal component analysis (PCA) using the correlation
!    method in the distributed processing mode
!
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-PCA_CORRELATION_CSR_DISTRIBUTED"></a>
 * \example pca_cor_csr_distr.cpp
 */

#include "daal.h"
#include "service.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms;

typedef float algorithmFPType;     /* Algorithm floating-point type */

/* Input data set parameters */
const size_t nBlocks         = 4;

const string datasetFileNames[] =
{
    "../data/distributed/covcormoments_csr_1.csv",
    "../data/distributed/covcormoments_csr_2.csv",
    "../data/distributed/covcormoments_csr_3.csv",
    "../data/distributed/covcormoments_csr_4.csv"
};

int main(int argc, char *argv[])
{
    checkArguments(argc, argv, 4, &datasetFileNames[0], &datasetFileNames[1], &datasetFileNames[2], &datasetFileNames[3]);

    /* Create an algorithm for principal component analysis using the correlation method on the master node */
    pca::Distributed<step2Master> masterAlgorithm;

    for (size_t i = 0; i < nBlocks; i++)
    {
        CSRNumericTable *dataTable = createSparseTable<float>(datasetFileNames[i]);

        /* Create an algorithm to compute a variance-covariance matrix in the distributed processing mode using the default method */
        pca::Distributed<step1Local> localAlgorithm;

        /* Create an algorithm for principal component analysis using the correlation method on the local node */
        localAlgorithm.parameter.covariance = services::SharedPtr<covariance::Distributed<step1Local, algorithmFPType, covariance::fastCSR> >
                                              (new covariance::Distributed<step1Local, algorithmFPType, covariance::fastCSR>());

        /* Set input objects for the algorithm */
        localAlgorithm.input.set(pca::data, CSRNumericTablePtr(dataTable));

        /* Compute partial estimates on local nodes */
        localAlgorithm.compute();

        /* Set local partial results as input for the master-node algorithm */
        masterAlgorithm.input.add(pca::partialResults, localAlgorithm.getPartialResult());
    }

    /* Use covariance algorithm for sparse data inside the PCA algorithm */
    masterAlgorithm.parameter.covariance = services::SharedPtr<covariance::Distributed<step2Master, algorithmFPType, covariance::fastCSR> >
                                           (new covariance::Distributed<step2Master, algorithmFPType, covariance::fastCSR>());

    /* Merge and finalize PCA decomposition on the master node */
    masterAlgorithm.compute();

    masterAlgorithm.finalizeCompute();

    /* Retrieve the algorithm results */
    pca::ResultPtr result = masterAlgorithm.getResult();

    /* Print the results */
    printNumericTable(result->get(pca::eigenvalues), "Eigenvalues:");
    printNumericTable(result->get(pca::eigenvectors), "Eigenvectors:");

    return 0;
}
