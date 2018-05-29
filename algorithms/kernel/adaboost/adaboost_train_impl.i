/* file: adaboost_train_impl.i */
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
//  Implementation of Freund method for Ada Boost training algorithm.
//--
*/
/*
//
//  REFERENCES
//
//  1. Robert E. Schapire. Explaining AdaBoost, Springer, 2013
//     http://rob.schapire.net/papers/explaining-adaboost.pdf
//  2. K. V. Vorontsov. Lectures about algorithm ensembles.
//     http://www.machinelearning.ru/wiki/images/0/0d/Voron-ML-Compositions.pdf
//
*/

#ifndef __ADABOOST_TRAIN_IMPL_I__
#define __ADABOOST_TRAIN_IMPL_I__

#include "algorithm.h"
#include "numeric_table.h"
#include "threading.h"
#include "daal_defines.h"
#include "service_math.h"
#include "service_memory.h"
#include "service_micro_table.h"
#include "service_numeric_table.h"

#include "weak_learner_model.h"
#include "adaboost_model.h"

using namespace daal::data_management;

namespace daal
{
namespace algorithms
{
namespace adaboost
{
namespace training
{
namespace internal
{
using namespace daal::internal;

/**
 *  \brief AdaBoost algorithm kernel
 *
 *  \param nVectors[in]               Number of observations
 *  \param weakLearnerInputTables[in] Array of 3 numeric tables [xTable, yTable, wTable] needed to train weak learner.
 *                                    xTable - holds matrix of observations
 *                                    yTable - holds array of class labels, y[i] is from {-1, 1}
 *                                    wTable - holds array of obserwations' weights
 *  \param hTable[in,out]             Table to store weak learner's classificatiion results
 *  \param y[in]                      Array of classification labels
 *  \param h[in,out]                  Array of weak learner's classificatiion results
 *  \param w[in,out]                  Array of observations' weights
 *  \param boostModel[in]             Ada Boost model
 *  \param parameter[in]              Ada Boost parameters
 *  \param nWeakLearners[out]         Number of weak learners
 *  \param modelsPtr[out]             Resulting array of weak learners' models
 *  \param alphaPtr[out]              Resulting array of boosting coefficients
 *
 */
template <Method method, typename algorithmFPType, CpuType cpu>
services::Status AdaBoostTrainKernel<method, algorithmFPType, cpu>::adaBoostFreundKernel(
    size_t nVectors, NumericTablePtr weakLearnerInputTables[],
    const HomogenNTPtr& hTable, const algorithmFPType *y,
    Model *boostModel, Parameter *parameter, size_t& nWeakLearners,
    algorithmFPType *alpha)
{
    algorithmFPType *w = static_cast<HomogenNT*>(weakLearnerInputTables[2].get())->getArray();
    algorithmFPType *h = hTable->getArray();

    /* Floating point constants */
    const algorithmFPType zero = (algorithmFPType)0.0;
    const algorithmFPType one  = (algorithmFPType)1.0;
    const algorithmFPType invNVectors = one / (algorithmFPType)nVectors;

    /* Get number of AdaBoost iterations */
    const size_t maxIter = parameter->maxIterations;
    const size_t accThr  = parameter->accuracyThreshold;

    /* Allocate memory for storing intermediate results */
    /* Vector of flags.
    errFlag[i] == -1.0, if weak classifier's classification result agrees with actual class label;
    errFlag[i] == 1.0, otherwise */
    daal::internal::TArray<algorithmFPType, cpu> aErrFlag(nVectors);
    DAAL_CHECK(aErrFlag.get(), services::ErrorMemoryAllocationFailed);

    /* Initialize weights */
    for (size_t i = 0; i < nVectors; i++)
    {
        w[i] = invNVectors;
    }

    services::SharedPtr<weak_learner::training::Batch> learnerTrain = parameter->weakLearnerTraining->clone();
    classifier::training::Input *trainInput = learnerTrain->getInput();
    DAAL_CHECK(trainInput, services::ErrorNullInput);
    trainInput->set(classifier::training::data,    weakLearnerInputTables[0]);
    trainInput->set(classifier::training::labels,  weakLearnerInputTables[1]);
    trainInput->set(classifier::training::weights, weakLearnerInputTables[2]);

    services::SharedPtr<weak_learner::prediction::Batch> learnerPredict = parameter->weakLearnerPrediction->clone();
    classifier::prediction::Input *predictInput = learnerPredict->getInput();
    DAAL_CHECK(predictInput, services::ErrorNullInput);
    predictInput->set(classifier::prediction::data, weakLearnerInputTables[0]);

    classifier::prediction::ResultPtr predictionRes(new classifier::prediction::Result());
    predictionRes->set(classifier::prediction::prediction, NumericTablePtr(hTable));
    learnerPredict->setResult(predictionRes);

    nWeakLearners = 0;
    algorithmFPType maxAlpha = zero;

    /* Clear the collection of weak learners models in the boosting model */
    boostModel->clearWeakLearnerModels();

    services::Status s;
    for (size_t m = 0; m < maxIter; m++)
    {
        nWeakLearners++;

        /* Make weak learner to allocate new memory for storing training result */
        if (m > 0)
            learnerTrain->resetResult();

        /* Train weak learner's model */
        DAAL_CHECK_STATUS(s, learnerTrain->computeNoThrow());

        classifier::training::ResultPtr trainingRes = learnerTrain->getResult();
        weak_learner::ModelPtr learnerModel =
            services::staticPointerCast<weak_learner::Model, classifier::Model>(trainingRes->get(classifier::training::model));

        /* Add new model to the collection of the boosting algorithm models */
        boostModel->addWeakLearnerModel(learnerModel);

        /* Get weak learner's classification results */
        predictInput->set(classifier::prediction::model, learnerModel);
        DAAL_CHECK_STATUS(s, learnerPredict->computeNoThrow());

        /* Calculate weighted error and errFlag: product of predicted * ground_truth */
        size_t nErr = 0;
        algorithmFPType errM = zero;
        algorithmFPType* errFlag = aErrFlag.get();
        for (size_t i = 0; i < nVectors; i++)
        {
            errFlag[i] = -one;
            if (h[i] * y[i] < zero)
            {
                errFlag[i] = one;
                nErr++;
                errM += w[i];
            }
        }

        if (nErr == 0)
        {
            /* Here if weak learner's classification error is 0 */
            alpha[m] = ((maxAlpha > zero) ? maxAlpha : one);
            break;
        }
        if (nErr == nVectors)
        {
            /* Here if weak learner's classification error is 1 */
            alpha[m] = 0.0;
            nWeakLearners--;
            break;
        }

        algorithmFPType cM = 0.5 * daal::internal::Math<algorithmFPType,cpu>::sLog((one - errM) / errM);

        /* Update weights */
        for (size_t i = 0; i < nVectors; i++)
        {
            errFlag[i] *= cM;
        }
        daal::internal::Math<algorithmFPType,cpu>::vExp(nVectors, errFlag, errFlag);
        algorithmFPType wSum = zero;
        for (size_t i = 0; i < nVectors; i++)
        {
            w[i] *= errFlag[i];
            wSum += w[i];
        }
        algorithmFPType invWSum = one / wSum;
        for (size_t i = 0; i < nVectors; i++)
        {
            w[i] *= invWSum;
        }
        alpha[m] = cM;

        if (errM < accThr) { break; }
        if (alpha[m] > maxAlpha) { maxAlpha = alpha[m]; }
    }
    return s;
}

template <Method method, typename algorithmFPType, CpuType cpu>
services::Status AdaBoostTrainKernel<method, algorithmFPType, cpu>::compute(size_t na, NumericTablePtr *a,
                                                                            Model *r, const Parameter *par)
{
    NumericTablePtr xTable = a[0];
    NumericTablePtr yTable = a[1];
    Parameter *parameter = const_cast<Parameter *>(par);
    r->setNFeatures(xTable->getNumberOfColumns());

    const size_t nVectors = xTable->getNumberOfRows();

    size_t nWeakLearners = 0;               /* Number of weak learners */

    /* Allocate memory for storing weak learners' models and boosting coefficients */
    daal::internal::TArray<algorithmFPType, cpu> alpha(parameter->maxIterations); /* AdaBoost coefficients */
    DAAL_CHECK(alpha.get(), services::ErrorMemoryAllocationFailed);

    services::Status s;
    HomogenNTPtr hTable(HomogenNT::create(1, nVectors, &s));
    DAAL_CHECK_STATUS_VAR(s);
    HomogenNTPtr wTable(HomogenNT::create(1, nVectors, &s));
    DAAL_CHECK_STATUS_VAR(s);

    NumericTablePtr weakLearnerInputTables[] = {xTable, yTable, wTable};

    /* Run AdaBoost training */
    {
        /* Get classification labels */
        ReadColumns<algorithmFPType, cpu> mtY(*yTable, 0, 0, nVectors);
        DAAL_CHECK_BLOCK_STATUS(mtY);
        DAAL_ASSERT(mtY.get());
        DAAL_CHECK_STATUS(s, adaBoostFreundKernel(nVectors, weakLearnerInputTables, hTable, mtY.get(), r, parameter, nWeakLearners, alpha.get()));
    }

    /* Update Ada Boost model with calculated results */
    NumericTablePtr alphaTable = r->getAlpha();
    DAAL_CHECK_STATUS(s, alphaTable->resize(nWeakLearners));
    WriteOnlyColumns<algorithmFPType, cpu> mtAlpha(*alphaTable, 0, 0, nWeakLearners);
    DAAL_CHECK_BLOCK_STATUS(mtAlpha);
    algorithmFPType *resAlpha = mtAlpha.get();
    DAAL_ASSERT(resAlpha);
    for(size_t i = 0; i < nWeakLearners; i++)
    {
        resAlpha[i]  = alpha[i];
    }
    return s;
}

} // namespace daal::algorithms::adaboost::training::internal
}
}
}
} // namespace daal

#endif
