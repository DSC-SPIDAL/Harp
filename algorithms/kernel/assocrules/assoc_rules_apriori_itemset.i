/* file: assoc_rules_apriori_itemset.i */
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
//  Definition of data strustures describing "large" item sets.
//--
*/

#ifndef __ASSOC_RULES_APRIORI_ITEMSET_I__
#define __ASSOC_RULES_APRIORI_ITEMSET_I__

#include "service_memory.h"
#include "daal_atomic_int.h"

using namespace daal::services;
using namespace daal::services::internal;

namespace daal
{
namespace algorithms
{
namespace association_rules
{
namespace internal
{

/**
 *  \brief Structure describing itemset
 */
template<CpuType cpu>
struct assocrules_itemset
{
    DAAL_NEW_DELETE();
    /** \brief Construct itemset of size 1 from item value */
    assocrules_itemset(size_t item0, size_t _support = 0) : items(nullptr), size(0), support(_support)
    {
        allocItems(1);
        items[0] = item0;
    }

    /**
     *  \brief Construct itemset of size (iset_size) from itemset of size (iset_size - 1) and item
     */
    assocrules_itemset(const size_t iset_size, const size_t *first_items,
                       const size_t second_item, const size_t _support = 0) :
                       support(_support), items(nullptr), size(0)
    {
        allocItems(iset_size);
        daal::services::daal_memcpy_s(items, iset_size*sizeof(size_t), first_items, (iset_size - 1)*sizeof(size_t));
        items[iset_size - 1] = second_item;
    }

    ~assocrules_itemset()
    {
        daal::services::daal_free(items);
    }

    /** \brief Copy constructor */
    assocrules_itemset(const assocrules_itemset &iset): items(nullptr), size(0)
    {
        allocItems(iset.size);
        support.set(iset.support.get());
        daal::services::daal_memcpy_s(items, size * sizeof(size_t), iset.items, size * sizeof(size_t));
    }

    assocrules_itemset &operator=(const assocrules_itemset &iset)
    {
        if (this != &iset)
        {
            daal::services::daal_free(items);
            items = nullptr;
            allocItems(iset.size);
            support.set(iset.support.get());
            daal::services::daal_memcpy_s(items, size * sizeof(size_t), iset.items, size * sizeof(size_t));
        }
        return *this;
    }

    Atomic<size_t> support;
    size_t *items;              /*<! Array of items */
    size_t size;                /*<! Itemset size */

protected:
    void allocItems(size_t n)
    {
        items = (size_t*)daal::services::daal_malloc(sizeof(size_t)*n);
        size = n;
    }
};

/** \brief Structure describing an itemset list */
template<CpuType cpu>
struct ItemSetList
{
    DAAL_NEW_DELETE();
    struct Node
    {
        DAAL_NEW_DELETE();
        Node() : _next(NULL), _itemSet(NULL) {}
        Node(assocrules_itemset<cpu>* s) : _next(NULL), _itemSet(s) {}

        Node* next() { return _next; }
        const Node* next() const { return _next; }
        void setNext(Node* n) { _next = n; }

        const assocrules_itemset<cpu>* itemSet() const { return _itemSet; }
        assocrules_itemset<cpu>* itemSet() { return _itemSet; }

    protected:
        Node* _next;
        assocrules_itemset<cpu>* _itemSet;
    };

    /* Create list of zero length */
    ItemSetList() : start(NULL), end(NULL), current(NULL), size(0), _bDataOwner(){}
    void setDataOwner(bool bOn) { _bDataOwner = bOn; }

    /* Destructor */
    virtual ~ItemSetList()
    {
        while(start)
        {
            auto next = start->next();
            deleteNode(start);
            start = next;
        }
    }

    /* Add new Node to the end of the list */
    bool insert(assocrules_itemset<cpu> *itemSet)
    {
        Node *newNode = new Node(itemSet);
        if(!newNode)
            return false;
        if (size > 0)
            end->setNext(newNode);
        else
            start = newNode;
        end = newNode;
        size++;
        return true;
    }

    /* Removes current Node and its content */
    void removeNode(Node* node, Node* prev)
    {
        DAAL_ASSERT(node);
        DAAL_ASSERT(!prev || prev->next() == node);
        if(prev)
            prev->setNext(node->next());
        if(node == start)
            start = start->next();
            size--;
        deleteNode(node);
        }

protected:
    void deleteNode(Node* node)
        {
        if(_bDataOwner)
            delete node->itemSet();
        delete node;
        }

public:
    Node *start;
    Node *end;
    Node *current;
    size_t size;
    bool _bDataOwner;
};

} // namespace internal

} // namespace association_rules

} // namespace algorithms

} // namespace daal

#endif
