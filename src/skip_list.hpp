////////////////////////////////////////////////////////////////////////////////
// Module Name:  skip_list.h/hpp
// Authors:      Leonid Dworzanski, Sergey Shershakov
// Version:      2.0.0
// Date:         28.10.2018
//
// This is a part of the course "Algorithms and Data Structures"
// provided by  the School of Software Engineering of the Faculty
// of Computer Science at the Higher School of Economics.
////////////////////////////////////////////////////////////////////////////////

// !!! DO NOT include skip_list.h here, 'cause it leads to circular refs. !!!

#include <cstdlib>
#include "skip_list.h"


//==============================================================================
// class NodeSkipList
//==============================================================================

template <class Value, class Key, int numLevels>
void NodeSkipList<Value, Key, numLevels>::clear(void)
{
    for (int i = 0; i < numLevels; ++i)
        Base::nextJump[i] = 0;

    Base::levelHighest = -1;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(void)
{
    clear();
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey)
{
    clear();

    Base::Base::key = tkey;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey, const Value& val)
{
    clear();

    Base::Base::key = tkey;
    Base::Base::value = val;
}


//==============================================================================
// class SkipList
//==============================================================================

template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::SkipList(double probability)
{
    _probability = probability;
    srand(time(NULL));
    // Lets use m_pPreHead as a final sentinel element
    for (int i = 0; i < numLevels; ++i)
        Base::_preHead->nextJump[i] = Base::_preHead;

    Base::_preHead->levelHighest = numLevels - 1;
}

template<class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::~SkipList()
{
    Node *run = Base::_preHead->next;

    while (run != nullptr)
    {
        for (int i = 0; i <= run->levelHighest; ++i)
            run->nextJump[i] = nullptr;

        if (run->next == Base::_preHead && run == Base::_preHead)
        {
            Base::_preHead = nullptr;
            delete run;
            break;
        }

        if (run == Base::_preHead)
            Base::_preHead = nullptr;

        if (run->next == Base::_preHead)
        {
            delete run;
            delete Base::_preHead;
            break;
        }
        Node *tmp = run->next;
        delete run;
        run = tmp;
    }
    Base::_preHead = nullptr;
}

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::insert(const Value &val, const Key &key)
{
    int level = -1;
    for (int i = 0; i < numLevels; ++i)
    {
        int randProbability = rand() % 11;

        if (randProbability <= _probability * 10)
            ++level;
        else
            break;
    }

    int currentLevel = numLevels - 1;
    Node* run = Base::_preHead;

    Node* tmp = new Node;
    tmp->key = key;
    tmp->value = val;
    tmp->levelHighest = level;

    while(currentLevel > -1)
    {
        while (run->nextJump[currentLevel] != Base::_preHead && key >= run->nextJump[currentLevel]->key)
            run = run->nextJump[currentLevel];

        if (currentLevel <= level)
        {
            tmp->nextJump[currentLevel] = run->nextJump[currentLevel];
            run->nextJump[currentLevel] = tmp;
        }

        //if (currentLevel - 1 > -1)
            //run = run->nextJump[currentLevel - 1];

        --currentLevel;
    }

    while (run->next != Base::_preHead && key >= run->next->key)
        run = run->next;

    tmp->next = run->next;
    run->next = tmp;
}

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::removeNext(SkipList::Node *nodeBefore)
{
    if (nodeBefore == nullptr || nodeBefore->next == nullptr || nodeBefore->next == Base::_preHead)
        throw std::invalid_argument("node is not found");

    int currentLevel = numLevels - 1;
    Node *run = Base::_preHead;

    while (currentLevel > -1)
    {
        while (run->nextJump[currentLevel] != Base::_preHead
               && run->nextJump[currentLevel]->key < nodeBefore->next->key)
            run = run->nextJump[currentLevel];

        while (run->nextJump[currentLevel] != Base::_preHead
               && run->nextJump[currentLevel]->key == nodeBefore->next->key)
        {
            if (run->nextJump[currentLevel] == nodeBefore->next)
            {
                Node *tmp = run->nextJump[currentLevel]->nextJump[currentLevel];
                run->nextJump[currentLevel] = tmp;
            }

            run = run->nextJump[currentLevel];
        }

        --currentLevel;
    }

    Node *tmp = nodeBefore->next;
    nodeBefore->next = tmp->next;
    delete tmp;

}

template<class Value, class Key, int numLevels>
typename SkipList<Value,Key,numLevels>::Node *SkipList<Value, Key, numLevels>::findLastLessThan(const Key &key) const
{
    int currentLevel = numLevels - 1;
    Node* run = Base::_preHead;

    while(currentLevel > -1)
    {
        while (run->nextJump[currentLevel] != Base::_preHead && key > run->nextJump[currentLevel]->key)
            run = run->nextJump[currentLevel];

        --currentLevel;
    }

    while (run->next != Base::_preHead && key > run->next->key)
        run = run->next;

    return run;
}

template<class Value, class Key, int numLevels>
typename SkipList<Value,Key,numLevels>::Node *SkipList<Value, Key, numLevels>::findFirst(const Key &key) const
{
    int currentLevel = numLevels - 1;
    Node* run = Base::_preHead;

    while(currentLevel > -1)
    {
        while (run->nextJump[currentLevel] != Base::_preHead && key > run->nextJump[currentLevel]->key)
            run = run->nextJump[currentLevel];

        --currentLevel;
    }

    while (run->next != Base::_preHead && key > run->next->key)
        run = run->next;

    if (run->next != Base::_preHead && run->next->key == key)
        return run->next;

    return nullptr;
}
