/* file: CovCSRDistr.java */
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
 //  Content:
 //     Java example of variance-covariance matrix computation in the distributed
 //     processing mode
 ////////////////////////////////////////////////////////////////////////////////
 */

/**
 * <a name="DAAL-EXAMPLE-JAVA-COVARIANCECSRDISTRIBUTED">
 * @example CovCSRDistr.java
 */

package com.intel.daal.examples.covariance;

import com.intel.daal.algorithms.covariance.*;
import com.intel.daal.data_management.data.CSRNumericTable;
import com.intel.daal.data_management.data.HomogenNumericTable;
import com.intel.daal.examples.utils.Service;
import com.intel.daal.services.DaalContext;

/*
// Input data set is stored in the compressed sparse row format
*/

class CovCSRDistr {

    /* Input data set parameters */
    private static final String datasetFileNames[] = new String[] { "../data/distributed/covcormoments_csr_1.csv",
            "../data/distributed/covcormoments_csr_2.csv", "../data/distributed/covcormoments_csr_3.csv",
            "../data/distributed/covcormoments_csr_4.csv" };

    private static final int nBlocks = 4;

    private static PartialResult[] partialResult = new PartialResult[nBlocks];
    private static Result          result;

    private static DaalContext context = new DaalContext();

    public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException {

        for (int i = 0; i < nBlocks; i++) {
            computeOnLocalNode(i);
        }

        computeOnMasterNode();

        HomogenNumericTable covariance = (HomogenNumericTable) result.get(ResultId.covariance);
        HomogenNumericTable mean = (HomogenNumericTable) result.get(ResultId.mean);

        Service.printNumericTable("Covariance matrix (upper left square 10*10) :", covariance, 10, 10);
        Service.printNumericTable("Mean vector:", mean, 1, 10);

        context.dispose();
    }

    private static void computeOnLocalNode(int block) throws java.io.IOException {
        /* Read the input data from a file */
        CSRNumericTable dataTable = Service.createSparseTable(context, datasetFileNames[block]);

        /* Create algorithm objects to compute a variance-covariance matrix in the distributed processing mode using the default method */
        DistributedStep1Local algorithm = new DistributedStep1Local(context, Float.class, Method.fastCSR);

        /* Set input objects for the algorithm */
        algorithm.input.set(InputId.data, dataTable);

        /* Compute partial estimates on nodes */
        partialResult[block] = algorithm.compute();
    }

    private static void computeOnMasterNode() {
        /* Create algorithm objects to compute a variance-covariance matrix in the distributed processing mode using the default method */
        DistributedStep2Master algorithm = new DistributedStep2Master(context, Float.class, Method.fastCSR);

        /* Set input objects for the algorithm */
        for (int i = 0; i < nBlocks; i++) {
            algorithm.input.add(DistributedStep2MasterInputId.partialResults, partialResult[i]);
        }

        /* Compute a partial estimate on the master node from the partial estimates on local nodes */
        algorithm.compute();

        /* Finalize the result in the distributed processing mode */
        result = algorithm.finalizeCompute();
    }
}
