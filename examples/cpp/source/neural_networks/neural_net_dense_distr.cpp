/* file: neural_net_dense_distr.cpp */
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
!    C++ example of neural network training and scoring in the distributed processing mode
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-NEURAL_NET_DENSE_DISTR"></a>
 * \example neural_net_dense_distr.cpp
 */

#include "daal.h"
#include "service.h"
#include "neural_net_dense_distr.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::algorithms::neural_networks;
using namespace daal::services;

/* Input data set parameters */
const string trainDatasetFileNames[4] =
{
    "../data/distributed/neural_network_train_dense_1.csv", "../data/distributed/neural_network_train_dense_2.csv",
    "../data/distributed/neural_network_train_dense_3.csv", "../data/distributed/neural_network_train_dense_4.csv"
};
const string trainGroundTruthFileNames[4] =
{
    "../data/distributed/neural_network_train_ground_truth_1.csv", "../data/distributed/neural_network_train_ground_truth_2.csv",
    "../data/distributed/neural_network_train_ground_truth_3.csv", "../data/distributed/neural_network_train_ground_truth_4.csv"
};
string testDatasetFile     = "../data/batch/neural_network_test.csv";
string testGroundTruthFile = "../data/batch/neural_network_test_ground_truth.csv";

const size_t nNodes = 4;
const size_t batchSize = 100;
const size_t batchSizeLocal = batchSize / nNodes;

TensorPtr trainingData[nNodes];
TensorPtr trainingGroundTruth[nNodes];
prediction::ModelPtr predictionModel;
prediction::ResultPtr predictionResult;
training::ModelPtr trainingModel;
training::TopologyPtr topology[nNodes];
training::TopologyPtr topologyMaster;

/* Algorithms to train neural network */
SharedPtr<training::Distributed<step2Master> > net;
SharedPtr<training::Distributed<step1Local> > netLocal[nNodes];

void initializeNetwork();
void trainModel();
void testModel();
void printResults();

int main(int argc, char *argv[])
{
    checkArguments(argc, argv, 10, &trainDatasetFileNames[0], &trainDatasetFileNames[1], &trainDatasetFileNames[2], &trainDatasetFileNames[3],
                  &trainGroundTruthFileNames[0], &trainGroundTruthFileNames[1], &trainGroundTruthFileNames[2], &trainGroundTruthFileNames[3],
                  &testDatasetFile, &testGroundTruthFile);

    initializeNetwork();
    trainModel();
    testModel();
    printResults();

    return 0;
}

void initializeNetwork()
{
    /* Create stochastic gradient descent (SGD) optimization solver algorithm */
    SharedPtr<optimization_solver::sgd::Batch<> > sgdAlgorithm(new optimization_solver::sgd::Batch<>());
    sgdAlgorithm->parameter.batchSize = batchSizeLocal;
    /* Read training data set from a .csv file and create tensors to store input data */
    for (size_t node = 0; node < nNodes; node++)
    {
        trainingData[node] = readTensorFromCSV(trainDatasetFileNames[node]);
        trainingGroundTruth[node] = readTensorFromCSV(trainGroundTruthFileNames[node], true);
    }
    Collection<size_t> dataDims = trainingData[0]->getDimensions();

    /* Configure the neural network */
    topologyMaster = configureNet();
    net = SharedPtr<training::Distributed<step2Master> >(new training::Distributed<step2Master>(sgdAlgorithm));

    /* Initialize the neural network on master node */
    services::Collection<size_t> oneBatchDimensions = dataDims;
    oneBatchDimensions[0] = batchSizeLocal;
    net->initialize(oneBatchDimensions, *topologyMaster);

    for(size_t node = 0; node < nNodes; node++)
    {
        /* Configure the neural network */
        topology[node] = configureNet();

        /* Pass a model from master node to the algorithms on local nodes */
        training::ModelPtr trainingModel(new training::Model());
        trainingModel->initialize<float>(oneBatchDimensions, *(topology[node]));
        netLocal[node] = SharedPtr<training::Distributed<step1Local> >(new training::Distributed<step1Local>());
        netLocal[node]->input.set(training::inputModel, trainingModel);
    }
}

void trainModel()
{
    /* Create stochastic gradient descent (SGD) optimization solver algorithm */
    SharedPtr<optimization_solver::sgd::Batch<> > sgdAlgorithm(new optimization_solver::sgd::Batch<>());

    /* Set learning rate for the optimization solver used in the neural network */
    float learningRate = 0.001f;
    sgdAlgorithm->parameter.learningRateSequence = NumericTablePtr(new HomogenNumericTable<>(1, 1, NumericTable::doAllocate, learningRate));
    sgdAlgorithm->parameter.batchSize = batchSizeLocal;

    /* Set the optimization solver for the neural network training */
    net->parameter.optimizationSolver = sgdAlgorithm;

    /* Run the neural network training */
    size_t nSamples = trainingData[0]->getDimensions().get(0);
    for (size_t i = 0; i < nSamples - batchSizeLocal + 1; i += batchSizeLocal)
    {
        /* Compute weights and biases for the batch of inputs on local nodes */
        for (size_t node = 0; node < nNodes; node++)
        {
            /* Pass a training data set and dependent values to the algorithm */
            netLocal[node]->input.set(training::data, getNextSubtensor(trainingData[node], i, batchSizeLocal) );
            netLocal[node]->input.set(training::groundTruth, getNextSubtensor(trainingGroundTruth[node], i, batchSizeLocal));

            /* Compute weights and biases on local node */
            netLocal[node]->compute();

            /* Pass computed weights and biases to the master algorithm */
            net->input.add(training::partialResults, node, netLocal[node]->getPartialResult());
        }

        /* Update weights and biases on master node */
        net->compute();
        training::ModelPtr wbModel = net->getPartialResult()->get(training::resultFromMaster)->get(training::model);
        checkPtr((void *)wbModel.get());
        NumericTablePtr wb = wbModel->getWeightsAndBiases();

        /* Update weights and biases on local nodes */
        for (size_t node = 0; node < nNodes; node++)
        {
            netLocal[node]->input.get(training::inputModel)->setWeightsAndBiases(wb);
        }
    }
    /* Finalize neural network training on the master node */
    net->finalizeCompute();

    /* Retrieve training and prediction models of the neural network */
    training::ModelPtr trModel = net->getResult()->get(training::model);
    checkPtr((void *)trModel.get());
    predictionModel = trModel->getPredictionModel<float>();
}

void testModel()
{
    /* Read testing data set from a .csv file and create a tensor to store input data */
    TensorPtr predictionData = readTensorFromCSV(testDatasetFile);

    /* Create an algorithm to compute the neural network predictions */
    prediction::Batch<> net;

    /* Set the batch size for the neural network prediction */
    net.parameter.batchSize = predictionData->getDimensionSize(0);

    /* Set input objects for the prediction neural network */
    net.input.set(prediction::model, predictionModel);
    net.input.set(prediction::data, predictionData);

    /* Run the neural network prediction */
    net.compute();

    /* Print results of the neural network prediction */
    predictionResult = net.getResult();
}

void printResults()
{
    /* Read testing ground truth from a .csv file and create a tensor to store the data */
    TensorPtr predictionGroundTruth = readTensorFromCSV(testGroundTruthFile);

    printTensors<int, float>(predictionGroundTruth, predictionResult->get(prediction::prediction),
                             "Ground truth", "Neural network predictions: each class probability",
                             "Neural network classification results (first 20 observations):", 20);
}
