/* file: EmGmmDenseBatch.java */
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
 //     Java example of the expectation-maximization (EM) algorithm for the
 //     Gaussian mixture model (GMM)
 ////////////////////////////////////////////////////////////////////////////////
 */

/**
 * <a name="DAAL-EXAMPLE-JAVA-EMGMMBATCH">
 * @example EmGmmDenseBatch.java
 */

package com.intel.daal.examples.em;

import com.intel.daal.algorithms.em_gmm.*;
import com.intel.daal.algorithms.em_gmm.init.*;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.data_management.data_source.DataSource;
import com.intel.daal.data_management.data_source.FileDataSource;
import com.intel.daal.examples.utils.Service;
import com.intel.daal.services.DaalContext;

class EmGmmDenseBatch {
    /* Input data set parameters */
    private static final String dataset       = "../data/batch/em_gmm.csv";
    private static final int    nComponents   = 2;

    private static DaalContext context = new DaalContext();

    public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException {
        /* Retrieve the input data */
        FileDataSource dataSource = new FileDataSource(context, dataset,
                DataSource.DictionaryCreationFlag.DoDictionaryFromContext,
                DataSource.NumericTableAllocationFlag.DoAllocateNumericTable);
        dataSource.loadDataBlock();
        NumericTable input = dataSource.getNumericTable();

        /* Create an algorithm to initialize the EM algorithm for the GMM */
        InitBatch initAlgorithm = new InitBatch(context, Float.class, InitMethod.defaultDense, nComponents);

        /* Set an input object for the initialization algorithm */
        initAlgorithm.input.set(InitInputId.data, input);

        InitResult initResult = initAlgorithm.compute();

        /* Create an algorithm for EM clustering */
        Batch algorithm = new Batch(context, Float.class, Method.defaultDense, nComponents);

        /* Set an input object for the algorithm */
        algorithm.input.set(InputId.data, input);
        algorithm.input.set(InputValuesId.inputValues, initResult);

        /* Clusterize the data */
        Result result = algorithm.compute();

        NumericTable means = result.get(ResultId.means);
        NumericTable weights = result.get(ResultId.weights);

        /* Print the results */
        Service.printNumericTable("Weights", weights);
        Service.printNumericTable("Means", means);

        for (int i = 0; i < nComponents; i++) {
            NumericTable covariance = result.get(ResultCovariancesId.covariances, i);
            Service.printNumericTable("Covariance", covariance);
        }

        context.dispose();
    }
}
