/* file: df_cls_traverse_model.cpp */
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
!    C++ example of decision forest classification model traversal.
!
!    The program trains the decision forest classification model on a training
!    datasetFileName and prints the trained model by its depth-first traversing.
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-DF_CLS_TRAVERSE_MODEL"></a>
 * \example df_cls_traverse_model.cpp
 */

#include "daal.h"
#include "service.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::algorithms::decision_forest::classification;

/* Input data set parameters */
const string trainDatasetFileName = "../data/batch/df_classification_train.csv";
const size_t categoricalFeaturesIndices[] = { 2 };
const size_t nFeatures  = 3;  /* Number of features in training and testing data sets */

/* Decision forest parameters */
const size_t nTrees = 2;
const size_t minObservationsInLeafNode = 8;
const size_t maxTreeDepth = 15;

const size_t nClasses = 5;  /* Number of classes */

training::ResultPtr trainModel();
void loadData(const std::string& fileName, NumericTablePtr& pData, NumericTablePtr& pDependentVar);
void printModel(const daal::algorithms::decision_forest::classification::Model& m);

int main(int argc, char *argv[])
{
    checkArguments(argc, argv, 1, &trainDatasetFileName);
    training::ResultPtr trainingResult = trainModel();
    printModel(*trainingResult->get(classifier::training::model));
    return 0;
}

training::ResultPtr trainModel()
{
    /* Create Numeric Tables for training data and dependent variables */
    NumericTablePtr trainData;
    NumericTablePtr trainDependentVariable;

    loadData(trainDatasetFileName, trainData, trainDependentVariable);

    /* Create an algorithm object to train the decision forest classification model */
    training::Batch<> algorithm(nClasses);

    /* Pass a training data set and dependent values to the algorithm */
    algorithm.input.set(classifier::training::data, trainData);
    algorithm.input.set(classifier::training::labels, trainDependentVariable);

    algorithm.parameter.nTrees = nTrees;
    algorithm.parameter.featuresPerNode = nFeatures;
    algorithm.parameter.minObservationsInLeafNode = minObservationsInLeafNode;
    algorithm.parameter.maxTreeDepth = maxTreeDepth;

    /* Build the decision forest classification model */
    algorithm.compute();

    /* Retrieve the algorithm results */
    return algorithm.getResult();
}

void loadData(const std::string& fileName, NumericTablePtr& pData, NumericTablePtr& pDependentVar)
{
    /* Initialize FileDataSource<CSVFeatureManager> to retrieve the input data from a .csv file */
    FileDataSource<CSVFeatureManager> trainDataSource(fileName,
        DataSource::notAllocateNumericTable,
        DataSource::doDictionaryFromContext);

    /* Create Numeric Tables for training data and dependent variables */
    pData.reset(new HomogenNumericTable<double>(nFeatures, 0, NumericTable::notAllocate));
    pDependentVar.reset(new HomogenNumericTable<double>(1, 0, NumericTable::notAllocate));
    NumericTablePtr mergedData(new MergedNumericTable(pData, pDependentVar));

    /* Retrieve the data from input file */
    trainDataSource.loadDataBlock(mergedData.get());

    NumericTableDictionaryPtr pDictionary = pData->getDictionarySharedPtr();
    for(size_t i = 0, n = sizeof(categoricalFeaturesIndices) / sizeof(categoricalFeaturesIndices[0]); i < n; ++i)
        (*pDictionary)[categoricalFeaturesIndices[i]].featureType = data_feature_utils::DAAL_CATEGORICAL;
}

/** Visitor class implementing TreeNodeVisitor interface, prints out tree nodes of the model when it is called back by model traversal method */
class PrintNodeVisitor : public daal::algorithms::classifier::TreeNodeVisitor
{
public:
    virtual bool onLeafNode(size_t level, size_t response)
    {
        for(size_t i = 0; i < level; ++i)
            std::cout << "  ";
        std::cout << "Level " << level << ", leaf node. Response value = " << response << std::endl;
        return true;
    }

    virtual bool onSplitNode(size_t level, size_t featureIndex, double featureValue)
    {
        for(size_t i = 0; i < level; ++i)
            std::cout << "  ";
        std::cout << "Level " << level << ", split node. Feature index = " << featureIndex <<
            ", feature value = " << featureValue << std::endl;
        return true;
    }
};

void printModel(const daal::algorithms::decision_forest::classification::Model& m)
{
    PrintNodeVisitor visitor;
    std::cout << "Number of trees: " << m.numberOfTrees() << std::endl;
    for(size_t i = 0, n = m.numberOfTrees(); i < n; ++i)
    {
        std::cout << "Tree #" << i << std::endl;
        m.traverseDF(i, visitor);
    }
}
