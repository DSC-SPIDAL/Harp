/* file: gbt_train_dense_default_impl.i */
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
//  Implementation of auxiliary functions for gradient boosted trees training
//  (defaultDense) method.
//--
*/

#ifndef __GBT_TRAIN_DENSE_DEFAULT_IMPL_I__
#define __GBT_TRAIN_DENSE_DEFAULT_IMPL_I__

#include "dtrees_model_impl.h"
#include "dtrees_train_data_helper.i"
#include "dtrees_predict_dense_default_impl.i"
#include "gbt_internal.h"
#include "gbt_train_aux.i"

namespace daal
{
namespace algorithms
{
namespace gbt
{
namespace training
{
namespace internal
{

using namespace daal::algorithms::dtrees::training::internal;
using namespace daal::algorithms::gbt::internal;

//////////////////////////////////////////////////////////////////////////////////////////
// Base task class. Implements general pipeline of tree building
//////////////////////////////////////////////////////////////////////////////////////////
template <typename algorithmFPType, CpuType cpu>
class TrainBatchTaskBase
{
public:
    typedef dtrees::internal::TreeImpRegression<> TreeType;
    typedef typename TreeType::NodeType NodeType;
    typedef ImpurityData<algorithmFPType, cpu> ImpurityType;
    typedef LossFunction<algorithmFPType, cpu> LossFunctionType;
    typedef OrderedRespHelper<algorithmFPType, cpu> DataHelperType;

    const LossFunctionType* lossFunc() const { DAAL_ASSERT(_loss); return _loss; }
    LossFunctionType* lossFunc() { DAAL_ASSERT(_loss); return _loss; }
    const Parameter& par() const { return _par; }
    const DataHelperType& dataHelper() const { return _dataHelper; }
    const FeatureTypes& featTypes() const { return _featHelper; }
    IndexType nFeaturesPerNode() const { return _nFeaturesPerNode; }
    bool isParallelFeatures() const { return _bParallelFeatures; }
    bool isParallelNodes() const { return _bParallelNodes; }
    bool isParallelTrees() const { return _bParallelTrees; }
    IndexType nSamples() const { return _nSamples; }
    bool isBagging() const { return !!_aSampleToF.get(); }
    const IndexType* aSampleToF() const { return _aSampleToF.get(); }
    bool isThreaded() const { return _bThreaded; }
    bool isIndexedMode() const { return !par().memorySavingMode; }
    int numAvailableThreads() const { auto n = _nParallelNodes.get(); return _nThreadsMax > n ? _nThreadsMax - n : 0; }
    size_t nFeatures() const { return _data->getNumberOfColumns(); }
    algorithmFPType accuracy() const { return _accuracy; }
    size_t nTrees() const { return _nTrees; }

    services::Status run(dtrees::internal::DecisionTreeTable** aTbl, size_t iIteration);
    virtual services::Status init();
    bool isIndirect() const { return _bIndirect; }
    double computeLeafWeightUpdateF(const IndexType* idx, size_t n, const ImpurityType& imp, size_t iTree);
    void updateOOB(size_t iTree, TreeType& t);
    bool terminateCriteria(size_t nSamples, size_t level, const ImpurityType& imp) const
    {
        return ((nSamples < 2 * _par.minObservationsInLeafNode) ||
            ((_par.maxTreeDepth > 0) && (level >= _par.maxTreeDepth)));
    }

    void featureValuesToBuf(size_t iFeature, algorithmFPType* featureVal, IndexType* aIdx, size_t n)
    {
        _dataHelper.getColumnValues(iFeature, aIdx, n, featureVal);
        daal::algorithms::internal::qSort<algorithmFPType, IndexType, cpu>(n, featureVal, aIdx);
    }

    void chooseFeatures(IndexType* featureSample)
    {
        const IndexType nFeat(nFeatures());
        {
            AUTOLOCK(_mtEngine);
            if(nFeaturesPerNode()*nFeaturesPerNode() < 2 * nFeat)
            {
                RNGs<IndexType, cpu>().uniformWithoutReplacement(nFeaturesPerNode(), featureSample, featureSample + nFeaturesPerNode(),
                    _engine.getState(), 0, nFeat);
            }
            else
            {
                for(IndexType i = 0; i < nFeat; ++i)
                    featureSample[i] = i;
                dtrees::training::internal::shuffle<cpu>(_engine.getState(), nFeat, featureSample);
            }
        }
    }

protected:
    typedef dtrees::internal::TVector<algorithmFPType, cpu> algorithmFPTypeArray;

    TrainBatchTaskBase(const NumericTable *x, const NumericTable *y, const Parameter& par,
        const dtrees::internal::FeatureTypes& featTypes,
        const dtrees::internal::IndexedFeatures* indexedFeatures,
        engines::internal::BatchBaseImpl& engine,
        size_t nClasses) :
        _data(x), _resp(y), _par(par), _engine(engine), _nClasses(nClasses),
        _nSamples(par.observationsPerTreeFraction*x->getNumberOfRows()),
        _nFeaturesPerNode(par.featuresPerNode ? par.featuresPerNode : x->getNumberOfColumns()),
        _dataHelper(indexedFeatures),
        _featHelper(featTypes),
        _accuracy(daal::services::internal::EpsilonVal<algorithmFPType>::get()),
        _nTrees(nClasses > 2 ? nClasses : 1),
        _nThreadsMax(threader_get_threads_number()),
        _nParallelNodes(0)
    {
        int internalOptions = par.internalOptions;
        if(_nTrees < 2 || par.memorySavingMode)
            internalOptions &= ~parallelTrees; //clear parallelTrees flag
        _bThreaded = ((_nThreadsMax > 1) && ((internalOptions & parallelAll) != 0));
        if(_bThreaded)
        {
            _bParallelFeatures = !!(internalOptions & parallelFeatures);
            _bParallelNodes = !!(internalOptions & parallelNodes);
            _bParallelTrees = !!(internalOptions & parallelTrees);
        }
    }
    ~TrainBatchTaskBase()
    {
        delete _loss;
    }

    virtual void initLossFunc() = 0;
    virtual services::Status buildTrees(dtrees::internal::DecisionTreeTable** aTbl) = 0;
    virtual void step(const algorithmFPType* y) = 0;
    virtual bool getInitialF(algorithmFPType& val) { return false; }

    //loss function arguments (current estimation of y)
    algorithmFPType* f() { return _aF.get(); }
    const algorithmFPType* f() const { return _aF.get(); }

    void initializeF(algorithmFPType initValue)
    {
        const auto nRows = _data->getNumberOfRows();
        const auto nF = nRows*_nTrees;
        //initialize f. TODO: input argument
        algorithmFPType* pf = f();
        PRAGMA_IVDEP
        PRAGMA_VECTOR_ALWAYS
        for(size_t i = 0; i < nF; ++i)
            pf[i] = initValue;
    }

public:
    daal::services::AtomicInt _nParallelNodes;

protected:
    DataHelperType _dataHelper;
    const FeatureTypes& _featHelper;
    TVector<algorithmFPType, cpu> _aF; //loss function arguments (f)
    //bagging, first _nSamples indices are the mapping of sample to row indices, the rest is OOB indices
    TVector<IndexType, cpu> _aSampleToF;

    daal::Mutex _mtEngine;
    engines::internal::BatchBaseImpl& _engine;

    const NumericTable *_data;
    const NumericTable *_resp;
    const Parameter& _par;
    const IndexType _nSamples;
    const IndexType _nFeaturesPerNode;
    const int _nThreadsMax;

    algorithmFPType _accuracy;
    algorithmFPType _initialF = 0.0;
    size_t _nClasses;
    size_t _nTrees; //per iteration
    LossFunctionType* _loss = nullptr;

    bool _bThreaded = false;
    bool _bParallelFeatures = false;
    bool _bParallelNodes = false;
    bool _bParallelTrees = false;
    bool _bIndirect = true;
};

template <typename algorithmFPType, CpuType cpu>
services::Status TrainBatchTaskBase<algorithmFPType, cpu>::init()
{
    delete _loss;
    _loss = nullptr;
    initLossFunc();
    const auto nRows = _data->getNumberOfRows();
    if(_nSamples < nRows)
    {
        _aSampleToF.reset(nRows);
        DAAL_CHECK_MALLOC(_aSampleToF.get());
    }
    const auto nF = nRows*_nTrees;
    _aF.reset(nF);
    DAAL_CHECK_MALLOC(_aF.get());

    //initialize _bIndirect flag
    {
        const size_t cThresholdFeatures = 130;
        const size_t cThresholdSamples = 3000000;
        _bIndirect = (par().memorySavingMode || (nSamples() * nFeaturesPerNode() < cThresholdSamples*cThresholdFeatures));
    }

    return _dataHelper.init(_data, _resp, isIndirect() ? _aSampleToF.get() : (const IndexType*)nullptr);
}

template <typename algorithmFPType, CpuType cpu>
double TrainBatchTaskBase<algorithmFPType, cpu>::computeLeafWeightUpdateF(const IndexType* idx,
    size_t n, const ImpurityType& imp, size_t iTree)
{
    double res = _initialF;
    algorithmFPType val = imp.h + _par.lambda;
    if(isZero<algorithmFPType, cpu>(val))
        return res;

    algorithmFPType* pf = f();
    const IndexType* sampleInd = _aSampleToF.get();
    val = -imp.g / val;
    const algorithmFPType inc = val*_par.shrinkage;
    if(sampleInd && isIndirect())
    {
        PRAGMA_IVDEP
        PRAGMA_VECTOR_ALWAYS
        for(size_t i = 0; i < n; ++i)
            pf[sampleInd[idx[i]] * this->_nTrees + iTree] += inc;
    }
    else
    {
        PRAGMA_IVDEP
        PRAGMA_VECTOR_ALWAYS
        for(size_t i = 0; i < n; ++i)
            pf[idx[i] * this->_nTrees + iTree] += inc;
    }
    return res + inc;
}

template <typename algorithmFPType, CpuType cpu>
services::Status TrainBatchTaskBase<algorithmFPType, cpu>::run(dtrees::internal::DecisionTreeTable** aTbl, size_t iIteration)
{
    for(size_t i = 0; i < _nTrees; ++i)
        aTbl[i] = nullptr;

    if(iIteration)
    {
        _initialF = 0;
    }
    else
    {
        if(!getInitialF(_initialF))
            _initialF = algorithmFPType(0);
        initializeF(_initialF);
    }

    const size_t nRows = _data->getNumberOfRows();
    if(isBagging())
    {
        auto aSampleToF = _aSampleToF.get();
        for(size_t i = 0; i < nRows; ++i)
            aSampleToF[i] = i;
        {
            TVector<IndexType, cpu> auxBuf(nRows);
            DAAL_CHECK_MALLOC(auxBuf.get());
            //no need to lock mutex here
            dtrees::training::internal::shuffle<cpu>(_engine.getState(), nRows, aSampleToF, auxBuf.get());
        }
        daal::algorithms::internal::qSort<IndexType, cpu>(nSamples(), aSampleToF);
        TVector<algorithmFPType, cpu> bagY(_nSamples);
        auto pBagY = bagY.get();
        DAAL_CHECK_MALLOC(pBagY);
        const auto y = this->_dataHelper.y();
        const auto aInd = aSampleToF;
        PRAGMA_IVDEP
        PRAGMA_VECTOR_ALWAYS;
        for(size_t i = 0; i < _nSamples; ++i)
            pBagY[i] = y[aInd[i]];
        step(pBagY);
    }
    else
    {
        step(this->_dataHelper.y());
    }
    _nParallelNodes.set(0);
    return buildTrees(aTbl);
}

template <typename algorithmFPType, CpuType cpu>
void TrainBatchTaskBase<algorithmFPType, cpu>::updateOOB(size_t iTree, TreeType& t)
{
    const auto aSampleToF = _aSampleToF.get();
    auto pf = f();
    const size_t n = _aSampleToF.size();
    const size_t nIt = n - _nSamples;
    daal::threader_for(nIt, nIt, [&](size_t i)
    {
        IndexType iRow = aSampleToF[i + _nSamples];
        ReadRows<algorithmFPType, cpu> x(const_cast<NumericTable*>(_dataHelper.data()), iRow, 1);
        auto pNode = dtrees::prediction::internal::findNode<algorithmFPType, TreeType, cpu>(t, x.get());
        DAAL_ASSERT(pNode);
        algorithmFPType inc = TreeType::NodeType::castLeaf(pNode)->response;
        pf[iRow*_nTrees + iTree] += inc;
    });
}

//////////////////////////////////////////////////////////////////////////////////////////
// Base task class. Implements general pipeline of tree building
//////////////////////////////////////////////////////////////////////////////////////////
template <typename algorithmFPType, CpuType cpu>
class TrainBatchTaskBaseXBoost : public TrainBatchTaskBase<algorithmFPType, cpu>
{
public:
    typedef TrainBatchTaskBase<algorithmFPType, cpu> super;
    typedef typename super::DataHelperType DataHelperType;
    typedef gh<algorithmFPType, cpu> ghType;

    TrainBatchTaskBaseXBoost(HostAppIface* hostApp, const NumericTable *x, const NumericTable *y, const Parameter& par,
        const dtrees::internal::FeatureTypes& featTypes,
        const dtrees::internal::IndexedFeatures* indexedFeatures,
        engines::internal::BatchBaseImpl& engine,
        size_t nClasses) : super(x, y, par, featTypes, indexedFeatures, engine, nClasses), _hostApp(hostApp){}

    //loss function gradient and hessian values calculated in f() points
    ghType* grad(size_t iTree) { return _aGH.get() + iTree*this->_nSamples; }
    const ghType* grad(size_t iTree) const { return _aGH.get() + iTree*this->_nSamples; }
    void step(const algorithmFPType* y) DAAL_C11_OVERRIDE
    {
        this->lossFunc()->getGradients(this->_nSamples, y, this->f(), this->aSampleToF(),
            (algorithmFPType*)_aGH.get());
    }
    virtual services::Status init() DAAL_C11_OVERRIDE
    {
        auto s = super::init();
        if(s)
        {
            _aGH.reset(this->_nSamples*this->_nTrees);
            DAAL_CHECK_MALLOC(_aGH.get());
        }
        return s;
    }

protected:
    TVector<ghType, cpu> _aGH; //loss function first and second order derivatives
    HostAppIface* _hostApp;
};

//////////////////////////////////////////////////////////////////////////////////////////
// compute() implementation
//////////////////////////////////////////////////////////////////////////////////////////
template <typename algorithmFPType, CpuType cpu, typename TaskType>
services::Status computeImpl(HostAppIface* pHostApp, const NumericTable *x, const NumericTable *y, gbt::internal::ModelImpl& md,
    const gbt::training::Parameter& par, engines::internal::BatchBaseImpl& engine, size_t nClasses)
{
    dtrees::internal::FeatureTypes featTypes;
    DAAL_CHECK_MALLOC(featTypes.init(*x));

    dtrees::internal::IndexedFeatures indexedFeatures;
    services::Status s;
    if(!par.memorySavingMode)
    {
        BinParams prm(par.maxBins, par.minBinSize);
        DAAL_CHECK_STATUS(s, (indexedFeatures.init<algorithmFPType, cpu>(*x, &featTypes, par.splitMethod == inexact ? &prm : nullptr)));
    }
    TaskType task(pHostApp, x, y, par, featTypes, par.memorySavingMode ? nullptr : &indexedFeatures, engine, nClasses);
    DAAL_CHECK_STATUS(s, task.init());
    const size_t nTrees = task.nTrees();
    DAAL_CHECK_MALLOC(md.reserve(par.maxIterations*nTrees));
    TVector<dtrees::internal::DecisionTreeTable*, cpu > aTables;
    typename dtrees::internal::DecisionTreeTable* pTbl = nullptr;
    dtrees::internal::DecisionTreeTable** aTbl = &pTbl;
    if(nTrees > 1)
    {
        aTables.reset(nTrees);
        DAAL_CHECK_MALLOC(aTables.get());
        aTbl = aTables.get();
    }
    for(size_t i = 0; (i < par.maxIterations) && !algorithms::internal::isCancelled(s, pHostApp); ++i)
    {
        s = task.run(aTbl, i);
        if(!s)
        {
            deleteTables<cpu>(aTbl, nTrees);
            break;
        }
        size_t iTree = 0;
        for(; (iTree < nTrees) && (aTbl[iTree]->getNumberOfRows() < 2); ++iTree);
        if(iTree == nTrees) //all are one level (constant response) trees
        {
            deleteTables<cpu>(aTbl, nTrees);
            break;
        }
        for(iTree = 0; iTree < nTrees; ++iTree)
            md.add(aTbl[iTree]);
        if((i + 1 < par.maxIterations) && task.done())
            break;
    }
    return s;
}

} /* namespace internal */
} /* namespace training */
} /* namespace gbt */
} /* namespace algorithms */
} /* namespace daal */

#endif
