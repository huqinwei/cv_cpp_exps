#include<iostream>
#include<vector>
#include<assert.h>
using namespace std;
template<class K, class V>
struct HashBucketNode
{
    pair<K, V> _kv;
    HashBucketNode<K, V>* _pNext;

    HashBucketNode(const K& key, const V& value)
        : _kv(pair<K, V>(key, value))
        , _pNext(NULL)
    {}
};

template<class K, class V>
class HashTable;

template<class K, class V, class Ref, class Ptr>
class _HashBucketIterator_
{
public:
    typedef _HashBucketIterator_<K, V, Ref, Ptr> Self;
    typedef HashBucketNode<K, V> Node;

public:
    _HashBucketIterator_()
        : _pNode(NULL)
        , _ht(NULL)
    {}

    _HashBucketIterator_(Node* pNode, HashTable<K, V>* ht)
        : _pNode(pNode)
        , _ht(ht)
    {}

    _HashBucketIterator_(const Self& it)
        : _pNode(it._pNode)
        , _ht(it._ht)
    {}

    Ref operator*()
    {
        return _pNode->_kv;
    }

    Ptr operator->()
    {
        return &(operator*());
    }

    Self& operator++()
    {
        _pNode = Next(_pNode);
        return *this;
    }

    Self operator++(int)
    {
        Self temp(*this);
        Next();
        return temp;
    }

    bool operator==(const Self& it)
    {
        return _pNode == it._pNode && _ht == it._ht;
    }

    bool operator!=(const Self& it)
    {
        return !(*this == it);
    }

private:
    Node* Next(Node* pNode)
    {
        Node* pNext = pNode->_pNext;
        if (pNext)//如果下一个结点存在则返回它
        {
            return pNext;
        }
        else //否则找下一个不为空的桶
        {
            size_t BucketNo = _ht->_HashFun(_pNode->_kv.first) + 1;
            size_t i = BucketNo;
            for (; i < _ht->_table.size(); ++i)
            {
                pNext = _ht->_table[i];
                if (pNext)
                {
                    return pNext;
                }
            }
        }
        return NULL;//没找到
    }

private:
    Node* _pNode;
    HashTable<K, V> *_ht;
};