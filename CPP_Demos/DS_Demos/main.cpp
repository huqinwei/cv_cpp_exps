
#include <iostream>
using namespace std;
#include<vector>
#include<string>
#include <assert.h>


#include "HashBucket.h"
template<class K>
struct _HashFun_
{
	size_t operator()(const K& key)
	{
		return key;
	}
};

template<> //模板特化
struct _HashFun_<string>
{
private:
	static size_t BKDRHash(const char* str) //key为字符串时的哈希函数
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313
		unsigned int hash = 0;
		while (*str)
		{
			hash = hash * seed + (*str++);
		}
		return (hash & 0x7FFFFFFF);
	}

public:
	size_t operator()(const string& key)
	{
		return BKDRHash(key.c_str());
	}
};




template<class K, class V>
class HashTable
{
	friend class _HashBucketIterator_<K, V, pair<K, V>&, pair<K, V>*>;
	typedef HashBucketNode<K, V> Node;
	typedef HashTable<K, V> Self;

public:
	typedef _HashBucketIterator_<K, V, pair<K, V>&, pair<K, V>*> Iterator;

public:
	HashTable(size_t bucketNum = 13)
		: _size(0)
	{
		_table.resize(_GetNextPrime(bucketNum));
	}

	~HashTable()
	{
		Clear();
	}

	Iterator Begin()
	{
		for (int index = 0; index < _table.size(); ++index)
		{
			if (_table[index] != NULL)
				return Iterator(_table[index], this);
		}
		return Iterator(NULL, this);
	}

	Iterator End()
	{
		return Iterator(NULL, this);
	}

	pair<Iterator, bool>  InsertEqual(const K& key, const V& value)
	{
		size_t Hashaddr = _HashFun(key);
		Node* pCur = _table[Hashaddr];

		while (pCur)
		{
			if (pCur->_kv.first == key)
				break;  //元素已存在
			pCur = pCur->_pNext;  //向下寻找元素是否存在
		}
		//没有在哈希表中找到该元素，插入该元素
		Node* pNewNode = new Node(key, value);
		pNewNode->_pNext = _table[Hashaddr];
		_table[Hashaddr] = pNewNode;
		_size++;
		return make_pair(Iterator(pCur, this), true);
	}

	pair<Iterator, bool> InsertUnique(const K& key, const V& value)
	{
		size_t Hashaddr = _HashFun(key);
		Node* pCur = _table[Hashaddr];

		while (pCur)
		{
			if (pCur->_kv.first == key)
				return make_pair(Iterator(pCur, this), false);   //元素已存在
			pCur = pCur->_pNext;  //向下寻找元素是否存在
		}
		//没有在哈希表中找到该元素，插入该元素
		Node* pNewNode = new Node(key, value);
		pNewNode->_pNext = _table[Hashaddr];
		_table[Hashaddr] = pNewNode;
		_size++;
		return make_pair(Iterator(pCur, this), true);
	}

	size_t RemoveUnique(const K& key)
	{
		size_t OldSize = _size;
		size_t index = _HashFun(key);
		Node* pCur = _table[index];
		if (pCur == NULL)
			return OldSize - _size;;
		if (pCur->_kv.first == key)
		{
			_table[index] = pCur->_pNext;
			delete pCur;
			pCur = NULL;
			_size--;
			return OldSize - _size;
		}
		Node* prev = NULL;
		while (pCur)
		{
			prev = pCur;
			pCur = pCur->_pNext;
			if (pCur->_kv.first == key)
			{
				prev->_pNext = pCur->_pNext;
				delete pCur;
				_size--;
				return OldSize - _size;
			}
		}
	}

	size_t RemoveEqual(const K& key)
	{
		size_t OldSize = _size;
		size_t index = _HashFun(key);
		Node* prev = NULL;
		Node* pCur = _table[index];
		while (pCur)
		{
			if (pCur->_kv.first == key)
			{
				if (prev == NULL)  //从哈希桶第一个元素开始删    
				{
					_table[index] = pCur->_pNext;
					delete pCur;
					pCur = NULL;
					_size--;
				}
				else   //从哈希桶非第一个元素开始删
				{
					prev->_pNext = pCur->_pNext;
					delete pCur;
					pCur = NULL;
					--_size;
				}
				index = _HashFun(key);  //重新更新哈希桶
				pCur = _table[index];
			}
			else
			{
				prev = pCur;
				pCur = pCur->_pNext;
			}
		}
		return OldSize - _size;
	}

	Iterator Find(const K& key)const
	{
		size_t index = _HashFun(key);
		Node* pCur = _table[index];
		while (pCur)
		{
			if (pCur->_kv.first == key)
				return Iterator(pCur, this);  //元素已存在
			pCur = pCur->_pNext;  //向下寻找元素是否存在
		}
		return Iterator(NULL, this); //没有找到该元素
	}

	bool Empty()const
	{
		return _size == 0;
	}

	size_t Size()const
	{
		return _size;
	}

	size_t Count(const K& key)const   //一个桶里有多少与这值相等的元素
	{
		size_t index = _HashFun(key);
		size_t count = 0;
		Node* pCur = _table[index];
		while (pCur)
		{
			if (pCur->_kv.first == key)
				count++;
			pCur = pCur->_pNext;
		}
		return count;
	}

	size_t BucketCount()const
	{
		size_t count = 0;
		for (size_t index = 0; index < _table.size(); ++index)
		{
			if (_table[index] != NULL)
				count++;
		}
		return count;
	}

	size_t CountInBucket(size_t bucketNo)
	{
		assert(bucketNo < _table.size());  //是否超过桶的个数
		char* str = NULL;
		size_t index = _HashFun(bucketNo);
		size_t count = 0;
		Node* pCur = _table[index];

		while (pCur)
		{
			count++;
			pCur = pCur->_pNext;
		}
		return count;
	}

	void Clear()
	{
		Node* pCur = NULL;
		Node* pDel = NULL;

		for (size_t idx = 0; idx < _table.size(); ++idx)
		{
			pCur = _table[idx];
			while (pCur)
			{
				pDel = pCur;
				pCur = pCur->_pNext;
				delete pDel;
				pDel = NULL;
			}
		}
	}

	void Swap(Self& ht)
	{
		_table.swap(ht._table);
		swap(_size, ht._size);
	}

private:
	size_t _GetNextPrime(size_t size)
	{
		const int _PrimeSize = 28;
		static const unsigned long _PrimeList[_PrimeSize] =
		{
			52ul, 97ul, 193ul, 389ul, 769ul,
			1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
			49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
			1572869ul, 3145739ul, 6291469ul, 12582917ul, 24165843ul,
			50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
			1610612741ul, 3221225473ul, 4294967291ul
		};

		for (size_t i = 0; i < _PrimeSize; ++i)
		{
			if (_PrimeList[i] > size)
			{
				return _PrimeList[i];
			}
		}
		return _PrimeList[_PrimeSize - 1];
	}

	size_t _HashFun(const K& key)const
	{
		_HashFun_<K> hf;
		size_t va = hf(key);
		return va % _table.size();
	}

	void CheckCapacity()
	{
		//负载因子为0时，扩容
		if (_size == _table.size())
		{
			size_t index = _GetNextPrime(_size);
			HashTable<K, V> temp;
			temp._table.resize(index);

			Node* pCur = NULL;
			for (; index < _table.size(); ++index)
			{
				size_t count = 0;
				pCur = _table[index];
				while (pCur)
				{
					temp.InsertEqual(pCur->_kv, count);
					pCur = pCur->_pNext;
				}
				count++;
			}
			this->Swap(temp);
		}
	}

private:
	vector<Node*> _table;
	size_t _size;   // 有效元素的个数
};


void FunTestUnique()
{
	HashTable<int, int> ht;

	ht.InsertUnique(26, 1);
	ht.InsertUnique(78, 1);
	ht.InsertUnique(67, 1);
	ht.InsertUnique(15, 2);
	ht.InsertUnique(36, 3);
	ht.InsertUnique(49, 4);
	ht.InsertUnique(68, 5);
	ht.InsertUnique(57, 6);
	ht.InsertUnique(11, 7);
	ht.InsertUnique(68, 5);
	ht.InsertUnique(68, 5);
	ht.InsertUnique(37, 8);

	cout << "哈希表中有效元素个数：" << ht.Size() << endl;
	HashTable<int, int>::Iterator it = ht.Begin();
	while (it != ht.End())
	{
		cout << (*it).first << endl;
		++it;
	}

	cout << "哈希桶中68元素的个数：" << ht.Count(68) << endl;
	cout << "哈希桶中有效桶的个数：" << ht.BucketCount() << endl;
	cout << "哈希表中5号桶元素的个数：" << ht.CountInBucket(5) << endl;
	ht.RemoveUnique(68);
	cout << "哈希桶中68元素的个数：" << ht.Count(68) << endl;
	ht.RemoveUnique(37);
	cout << "哈希表中有效元素个数：" << ht.Size() << endl << endl;
}

void FunTestEqual()
{
	HashTable<int, int> ht;

	ht.InsertEqual(26, 1);
	ht.InsertEqual(78, 1);
	ht.InsertEqual(67, 1);
	ht.InsertEqual(15, 2);
	ht.InsertEqual(36, 3);
	ht.InsertEqual(49, 4);
	ht.InsertEqual(68, 5);
	ht.InsertEqual(57, 6);
	ht.InsertEqual(11, 7);
	ht.InsertEqual(68, 5);
	ht.InsertEqual(68, 5);
	ht.InsertEqual(37, 8);

	cout << "哈希表中有效元素个数：" << ht.Size() << endl;
	HashTable<int, int>::Iterator it = ht.Begin();
	while (it != ht.End())
	{
		cout << (*it).first << endl;
		++it;
	}

	cout << "哈希桶中68元素的个数：" << ht.Count(68) << endl;
	cout << "哈希桶中有效桶的个数：" << ht.BucketCount() << endl;
	cout << "哈希表中5号桶元素的个数：" << ht.CountInBucket(5) << endl;
	ht.RemoveEqual(68);
	cout << "哈希桶中68元素的个数：" << ht.Count(68) << endl;
	ht.RemoveEqual(37);
	cout << "哈希表中有效元素个数：" << ht.Size() << endl << endl;
}

void FunTestEqual2()
{
	HashTable<string, string> ht;

	ht.InsertEqual("26", "1");
	ht.InsertEqual("78", "1");
	ht.InsertEqual("67", "1");
	ht.InsertEqual("68", "5");
	ht.InsertEqual("68", "5");
	ht.InsertEqual("68", "5");

	cout << "哈希表中有效元素个数：" << ht.Size() << endl;
	HashTable<string, string>::Iterator it = ht.Begin();
	while (it != ht.End())
	{
		cout << (*it).first << endl;
		++it;
	}

	cout << "哈希桶中68元素的个数：" << ht.Count("68") << endl;
	cout << "哈希桶中有效桶的个数：" << ht.BucketCount() << endl;
	ht.RemoveEqual("68");
	cout << "哈希桶中68元素的个数：" << ht.Count("68") << endl;
	ht.RemoveEqual("37");
	cout << "哈希表中有效元素个数：" << ht.Size() << endl << endl;
}


int main()
{
	FunTestUnique();
	FunTestUnique();
	FunTestEqual();
	FunTestEqual2();
	system("pause");
	return 0;
}
