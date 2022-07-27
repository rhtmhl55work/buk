#ifndef CDSU_MBHASH_H_
#define CDSU_MBHASH_H_

//============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsumbhash.h
// Description	: Defines a hash with a multibyte key.
// Ref Docs	:
// Author       : Seema Chandak 
// Created On   : 
// Last Modified: 1. Fri Jun 20 13:22:54 IST 2003
// 		: 2. Mon Aug 16 10:40:19 IST 2004
// 		:	By : Vijaya Kumar
// 			Copy Constructor is Added
//============================================================================

#ifdef _UTIL_STAND_ALONE
#include "cdsuutiltypes.h"
#endif

#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------
// Class	: CdSuMultiByteHash 
// Description	: Hash with a multibyte key.
//----------------------------------------------------------------------------

template <class ValType> 
class CdSuMultiByteHash
{
public:
	// size (num of buckets) of hash, must be prime numbers.
	enum TableSize
	{
		TZ_251 = 251,
		TZ_509 = 509,
		TZ_1021	 = 1021,
		TZ_2039 = 2039,
		TZ_4093 = 4093,
		TZ_8191 = 8191,
		TZ_16381 = 16381
	};

	enum HashFunctionType
	{
		HFT_DEFAULT,
		HFT_ONE,
		HFT_TWO
	};

	CdSuMultiByteHash (TableSize numBuckets, int keylen,
				HashFunctionType hft = HFT_DEFAULT);
	CdSuMultiByteHash (CdSuMultiByteHash<ValType>& mbHash);
	~CdSuMultiByteHash ();

	bool insert (void* key, const ValType& value);
	bool lookup (void* key, ValType& value);
	bool remove ( void* key);

	int size () { return numElements; }
	void clear ();
	void print ();
private:
	// Hash element
	struct HashElement
	{
		// key, value and next element in the same bucket.
		void* key;
		ValType value;
		HashElement* next;
	};

	// Hash bucket
	struct HashBucket
	{
		HashElement* first;
		// chain of non-empty buckets
		HashBucket* next;	
	};

	int numBuckets;

	// One entry cache remembering the last successful lookup or insert
	// operation.
	HashElement* cache;	
	
	// Buckets.
	HashBucket* buckets;

	// Chain of non-empty buckets.
	HashBucket chain;

	// Number of elements in hash table.
	int numElements;

	// Internal functions.
	void freeBucketList (HashElement* el);
	bool keyMatch (HashElement* el, void* key); 
	HashElement* getElement (void);
	void freeElement (HashElement* el);
	
	// Hash functions.
	unsigned int hash (void* key);
	unsigned int hashFunctionDefault (void* key);
	unsigned int hashFunction1 (void* key);
	unsigned int hashFunction2 (void* key);
	unsigned int hashFunction3 (void* key);

	HashFunctionType hashFunctionType;
	int keyLen;

public:
	//--------------------------------------------------------------------
	// Class	: iterator 
	// Description	: CdSuMultiByteHash iterator
	//--------------------------------------------------------------------

	class iterator
	{
	public:
		iterator (HashBucket* bucket = 0, HashElement* elem = 0,
				const HashBucket* end = 0);
		ValType& operator* ();
		const ValType& operator* () const;
		iterator& operator++ ();
		iterator operator++ (int);
		bool operator== (const iterator& iter) const;
		bool operator!= (const iterator& iter) const;
	private:
		// pointer to bucket, element and end of chain
		HashBucket* curBucket;
		HashElement* curElem;
		const HashBucket* endOfChain;
		friend class CdSuMultiByteHash;
	}; // iterator

	iterator begin () const;
	iterator end () const;

}; // CdSuMultiByteHash

//----------------------------------------------------------------------------
// Function	: CdSuMultiByteHash 
// Description	: CdSuMultiByteHash constructor.
// Inputs	: TableSize, size (number of buckets) of hash table.
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: Allocate memory for the buckets and initialise the variables.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuMultiByteHash<ValType>::CdSuMultiByteHash (CdSuMultiByteHash::TableSize numBuckets,
			 int keylen, CdSuMultiByteHash::HashFunctionType hft)
{
	buckets = new HashBucket [numBuckets];
	this->numBuckets = numBuckets;
	memset (buckets, 0, numBuckets * sizeof (HashBucket)); 
	chain.next = &chain;
	cache = NULL;
	numElements = 0;
	hashFunctionType = hft;
	keyLen = keylen;

} // CdSuMultiByteHash::CdSuMultiByteHash ()

//----------------------------------------------------------------------------
// Function	: CdSuMultiByteHash 
// Description	: Copy constructor.
// Inputs	: 
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: 
//----------------------------------------------------------------------------

template <class ValType> 
CdSuMultiByteHash<ValType>::CdSuMultiByteHash (CdSuMultiByteHash<ValType>& mbHash)
{
	buckets = new HashBucket [mbHash.numBuckets];
	this->numBuckets = mbHash.numBuckets;
	memset (buckets, 0, numBuckets * sizeof (HashBucket)); 
	chain.next = &chain;
	cache = NULL;
	numElements = 0;
	hashFunctionType = mbHash.hashFunctionType;
	keyLen = mbHash.keyLen;
	for (iterator iter = mbHash.begin (); iter != mbHash.end ();
			++iter)
	{
		this->insert (iter.curElem->key, iter.curElem->value);
	} // for 
} // CdSuMultiByteHash::CdSuMultiByteHash ()


//----------------------------------------------------------------------------
// Function	: ~CdSuMultiByteHash 
// Description	: CdSuMultiByteHash destructor.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Delete the elements and the buckets.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuMultiByteHash<ValType>::~CdSuMultiByteHash ()
{
	/*for (int i = 0; i < numBuckets; ++i)
	{
		freeBucketList (buckets[i].first);
	}*/
	this->clear ();
	delete [] buckets;

} // CdSuMultiByteHash::~CdSuMultiByteHash ()

//----------------------------------------------------------------------------
// Function	: keyMatch
// Description	: Compares keys.
// Inputs	: HashElement and key.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Compare the key of the element and the given key.
//----------------------------------------------------------------------------

template <class ValType> 
bool CdSuMultiByteHash<ValType>::keyMatch (HashElement* el, void* key)
{
	if ((memcmp ((void*) el->key, (void*) key, this->keyLen)) == 0)
	{
		return true;
	} else
	{
		return false;
	}

} // CdSuMultiByteHash::keyMatch ()

//----------------------------------------------------------------------------
// Function	: getElement
// Description	: Gets a hash element.
// Inputs	: None.
// Return Value	: HashElement
// Side Effects	: None.
// Algorithm	: 
//----------------------------------------------------------------------------

template <class ValType> 
typename CdSuMultiByteHash<ValType>::HashElement* CdSuMultiByteHash<ValType>::getElement (void)
{
	HashElement* el = new HashElement;
	return (el);

} // CdSuMultiByteHash::getElement ()

//----------------------------------------------------------------------------
// Function	: freeElement
// Description	: Free an element.
// Inputs	: HashElement.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	 
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuMultiByteHash<ValType>::freeElement (HashElement* el)
{
	char* key = (char*) el->key;
	delete [] key;
	delete el;

} // CdSuMultiByteHash::freeElement ();

//----------------------------------------------------------------------------
// Function	: insert 
// Description	: Insert a (key, value)
// Inputs	: Key, value.
// Return Value	: boolean
// Side Effects	: None.
// Algorithm	: 1. Get the bucket by hashing the key.
//		  2. Search the elements in the bucket to check whether the 
//		     key already exits.
//		  3. If the key is found, return false.
//		  4. If the key is not found, get a new element and add to the
//		     bucket.
//		  5. Update the number of elements and cache.
//----------------------------------------------------------------------------

template <class ValType> 
bool CdSuMultiByteHash<ValType>::insert (void* key, const ValType& value)
{
	unsigned int hv = hash (key);
	// printf ("hv = %d\n", hv);
	HashBucket* bucket = &buckets [hv];
	HashElement* elem = bucket->first;

	while (elem)
	{
		if (keyMatch (elem, key))
		{
			return false;
		}
		elem = elem->next;
	}
	
	// Get a new element and update
	HashElement* newElem = getElement ();
	if (newElem == NULL)
	{
		return false;
	}
	newElem->key = new char [keyLen];
	memcpy (newElem->key, key, keyLen);
	newElem->value = value;
	newElem->next = bucket->first;
	bucket->first = newElem;

	// Update cache
	cache = newElem;

	++numElements;

	// Chain the bucket.
	if (!bucket->next)
	{
		bucket->next = chain.next;
		chain.next = bucket;
	}

	return (true);

} // CdSuMultiByteHash::insert ()

//----------------------------------------------------------------------------
// Function	: lookup
// Description	: Lookup a key
// Inputs	: Key
// Return Value	: boolean, value corresponding to the key.
// Side Effects	: None.
// Algorithm	: 1. Compare the key and the key in cache. If equal return the
//		     value and true.
//		  2. Else get the bucket by hashing the key.
//		  3. Search the key in the elements of the bucket.
//		  4. If found, update cache, return the value and true, 
//		     Else return value=0 and false.
//----------------------------------------------------------------------------

template <class ValType> 
bool CdSuMultiByteHash<ValType>::lookup (void* key, ValType& value)
{
	bool retVal = false;
	// value = 0;
	HashElement* elem = cache;
	if (elem && keyMatch (elem, key))
	{
		// Cache hit.
		value = elem->value;
		retVal = true;
	} else
	{
		elem = buckets [hash (key)].first;
		while (elem)
		{
			if (keyMatch (elem, key))
			{
				value = elem->value;
				// update cache
				cache = elem;
				retVal = true;
				break;
			}
			elem = elem->next;
		}
	}

	return (retVal);

} // CdSuMultiByteHash::lookup ()

//----------------------------------------------------------------------------
// Function	: remove
// Description	: Remove (key, value).
// Inputs	: Key.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: 1. Get the bucket by hashing the key.
//		  2. Search the key in the elements of the bucket.
//		  3. If found, free the element, update number of elements and
//		     set cache to 0. Return true.
//		  4. Else return false.	
//----------------------------------------------------------------------------

template <class ValType> 
bool CdSuMultiByteHash<ValType>::remove (void* key)
{
	HashElement* elem, **prevElem;
	HashBucket* bucket = &buckets [hash (key)];
	prevElem = &bucket->first;
	elem = bucket->first;
	while (elem)
	{
		if (keyMatch (elem, key))
		{
			*prevElem = elem->next;
			freeElement (elem);
			--numElements;
			// Update cache.
			cache = NULL;
			return true;
		}
		prevElem = &elem->next;
		elem = elem->next;
	}

	return false;

} // CdSuMultiByteHash::remove

//----------------------------------------------------------------------------
// Function	: clear
// Description	: Deletes all the elements of the hash.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: For all the buckets.
//		  1. Delete the elements in the bucket. Set bucket->first to
//		  NULL.
//		  2. Reset the chain.
//		  3. Delete the free bucket list.
//		  4. Set the number of elements to zero.
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuMultiByteHash<ValType>::clear ()
{
	for (int i = 0; i < numBuckets; ++i)
	{
		freeBucketList (buckets[i].first);
		buckets[i].first = NULL;
	}
	chain.next = &chain;
	numElements = 0;

} // CdSuMultiByteHash<ValType>::clear ()

//----------------------------------------------------------------------------
// Function	: freeBucketList
// Description	: Free the elements of a bucket.
// Inputs	: Beginning of the list.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Deletes all the elements of the list
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuMultiByteHash<ValType>::freeBucketList (HashElement* el)
{
	HashElement* next;
	while (el)
	{
		next = el->next;
		char* key = (char*) el->key;
		delete [] key;
		delete el;
		el = next;
	}

} // CdSuMultiByteHash::freeBucketList

//----------------------------------------------------------------------------
// Function	: print 
// Description	: Print the elements.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Print all the elements. 
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuMultiByteHash<ValType>::print ()
{
	for (int i = 0; i < numBuckets; ++i)
	{
		HashElement* elem = buckets [i].first;
		printf ("[%d]", i);
		while (elem)
		{
			printf (".");
			elem = elem->next;
		}
		printf ("\n");
	}

} // CdSuMultiByteHash::print ()

//----------------------------------------------------------------------------
// Function	: hash
// Description	: Hash function
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: Uses modular hash function - (k mod M)
//		  k is the key, M is the table size, a prime number.
//----------------------------------------------------------------------------

template <class ValType> 
unsigned int CdSuMultiByteHash<ValType>::hash (void* key)
{
	switch (hashFunctionType)
	{
	case HFT_DEFAULT:
		return (hashFunctionDefault (key));
		break;

	case HFT_ONE:
		return (hashFunction1 (key));
		break;

	case HFT_TWO:
		return (hashFunction2 (key));
		break;

	default:
		return (0);
		break;
	}

} // CdSuMultiByteHash::hash ()

//----------------------------------------------------------------------------
// Function	: hashFunctionDefault
// Description	: Default hash function used when the user does not specify 
// 		  any particular preference for the hash function.
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: Algorthms in C++, Robert Sedgewick, Program 14.1
//----------------------------------------------------------------------------

template <class ValType>
unsigned int CdSuMultiByteHash<ValType>::hashFunctionDefault (void* p)
{
	char* key = (char*) p;
	unsigned int h = 0;
	unsigned int a = 127;
	for (int i = 0; i < keyLen; i++)
	{
		h = ((a*h + key [i]) & 0x7fffffff) % numBuckets;
	}

	return (h);

} // CdSuMultiByteHash::hashFunctionDefault ()

//----------------------------------------------------------------------------
// Function	: hashFunction1
// Description	: 
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: This algorithm is taken from the book:
//		  "Introduction to Compiling Techniques"
//		  "A First Course using ANSI C, LEX and YACC"
//			-J.P.Bennett.
//----------------------------------------------------------------------------

template <class ValType>
unsigned int CdSuMultiByteHash<ValType>::hashFunction1 (void* p)
{
	char* key = (char*) p;
	unsigned int hv = 0;
	
	for (int i = 0; i < keyLen; i++)
	{	
		int v = (hv >> 28) ^ (key[i] & 0xf);
		hv = (hv << 4) | v;
	}
	hv = hv & 0x7fffffff;
	
	return (hv % numBuckets);

} // CdSuMultiByteHash::hashFunction1 ()

//----------------------------------------------------------------------------
// Function	: hashFunction2
// Description	: 
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: Algorthms in C++, Robert Sedgewick, Program 14.2
//----------------------------------------------------------------------------

template <class ValType>
unsigned int CdSuMultiByteHash<ValType>::hashFunction2 (void* p)
{
	char* key = (char*) p;
	unsigned int h = 0;
	unsigned int a = 31415;
	unsigned int b = 27183;
	for (int i = 0; i < keyLen; i++, a = a*b % (numBuckets - 1))
	{
		h = (a*h + key [i]) % numBuckets;
	}
	return (h < 0) ? (h + numBuckets) : h;

} // CdSuMultiByteHash::hashFunction2 ()

//----------------------------------------------------------------------------
// Function	: hashFunction3
// Description	: Default hash function used when the user does not specify 
// 		  any particular preference for the hash function.
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: This algorithm is taken from gcc compiler source.
//----------------------------------------------------------------------------

template <class ValType> 
unsigned int CdSuMultiByteHash<ValType>::hashFunction3 (void* p)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	unsigned char* key = (unsigned char*) p;
	unsigned int hash = 0;
	unsigned char c;
	unsigned int len = 0;

	for (int i = 0; i < keyLen; i++)
	{	
		c = key [i];
		hash += c + (c << 17);
		hash ^= hash >> 2;
	} // for.

	hash += len + (len << 17);
	hash ^= hash >> 2;

	return (hash % numBuckets);

} // CdSuCharHash::hashFunction3 ()


//----------------------------------------------------------------------------
// Function	: begin
// Description	: Returns an iterator pointing to the first hash element.
// Inputs	: None. 
// Return Value	: Iterator
// Side Effects	: None.
// Algorithm	: Creates and returns an iterator pointing to the first hash
//		  element. 
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuMultiByteHash<ValType>::iterator CdSuMultiByteHash<ValType>::begin () const 
{
	HashBucket* prevBucket = (HashBucket*) &chain;
	HashBucket* curBucket = chain.next;
	HashElement* elem = NULL;

	while (curBucket != &chain)
	{
		elem = curBucket->first;
		if (elem)
		{
			// Non empty bucket.
			return iterator (curBucket, elem, (HashBucket*)&chain);
		} else
		{
			// Empty bucket, remove from chain.
			prevBucket->next = curBucket->next;
			curBucket->next = NULL;
			curBucket = prevBucket->next;
		}
	}

	// All buckets empty.
	return iterator ((HashBucket*) &chain);

} // CdSuMultiByteHash::begin ()

//----------------------------------------------------------------------------
// Function	: end
// Description	: Returns an iterator pointing to the end of the list.
// Inputs	: None. 
// Return Value	: Iterator
// Side Effects	: None.
// Algorithm	: Creates and returns an iterator pointing to the end of
//		  the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuMultiByteHash<ValType>::iterator CdSuMultiByteHash<ValType>::end () const
{

	return iterator ((HashBucket*) &chain);

} // CdSuMultiByteHash::end ()

//----------------------------------------------------------------------------
// Function	: iterator 
// Description	: CdSuMultiByteHash iterator constructor.
// Inputs	: list element pointer.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise the current list pointer
//----------------------------------------------------------------------------

template <class ValType>
CdSuMultiByteHash<ValType>::iterator::iterator (HashBucket* bucket, HashElement*
		elem, const HashBucket* end)
		: curBucket (bucket), curElem (elem), endOfChain (end)
{ 
	// Nothing to do.

} // CdSuMultiByteHash::iterator::iterator ()

//----------------------------------------------------------------------------
// Function	: operator* 
// Description	: Deference operator.
// Inputs	: None.
// Return Value	: Reference of element value at the current position,
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//----------------------------------------------------------------------------

template <class ValType>
ValType& CdSuMultiByteHash<ValType>::iterator::operator* ()
{
	return curElem->value;

} // CdSuMultiByteHash::iterator::operator*  ()
 
//----------------------------------------------------------------------------
// Function	: operator* 
// Description	: Constant dereference operator. 
// Inputs	: None.
// Return Value	: Constant reference of the element value at the current
//		  position. 
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//----------------------------------------------------------------------------

template <class ValType>
const ValType& CdSuMultiByteHash<ValType>::iterator::operator* () const
{
	return curElem->value;

} // CdSuMultiByteHash::iterator::operator* ()

//----------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Prefix operator
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Change the current position to the next element in the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuMultiByteHash<ValType>::iterator& CdSuMultiByteHash<ValType>::iterator::operator++ ()
{
	curElem = curElem->next;
	if (curElem)
	{
		return *this;
	}

	// Search for the next free element.
	HashBucket* prevBucket = curBucket;
	curBucket = curBucket->next;
	while (curBucket != endOfChain)
	{
		curElem = curBucket->first;
		if (curElem)
		{
			// Non empty bucket.
			return *this;
		} else
		{
			// Empty bucket, remove from chain.
			prevBucket->next = curBucket->next;
			curBucket->next = NULL;
			curBucket = prevBucket->next;
		}
	}

	// All buckets empty.
	return *this;

} // CdSuMultiByteHash::iterator::operator++

//----------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Postfix operator 
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Save the current position. Go to the next element. Return the
//		  saved position.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuMultiByteHash<ValType>::iterator CdSuMultiByteHash<ValType>::iterator::operator++ (int)
{
	iterator temp = *this;
	++*this;

	return temp;

} // CdSuMultiByteHash::iterator::operator++

//----------------------------------------------------------------------------
// Function	: operator== 
// Description	: Tests equality of two iterators.
// Inputs	: Iterator.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Return true if iterators point to the same bucket and element, else
//		  false.
//----------------------------------------------------------------------------

template <class ValType>
bool CdSuMultiByteHash<ValType>::iterator::operator== (const iterator& iter) const
{
	return ((curBucket == iter.curBucket) && (curElem == iter.curElem));

} // CdSuMultiByteHash::iterator::operator==

//----------------------------------------------------------------------------
// Function	: operator!= 
// Description	: Tests the inequality of two iterators.
// Inputs	: Iterator.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Return true if iterators point to differnet bucket or element, else
//		  false
//----------------------------------------------------------------------------

template <class ValType>
bool CdSuMultiByteHash<ValType>::iterator::operator!= (const iterator& iter) const
{
	return ((curBucket != iter.curBucket) || (curElem != iter.curElem));

} // CdSuMultiByteHash::iterator::operator!=

//============================================================================
// <End of cdsuinthash.h>
//============================================================================
#endif // CDSU_INTHASH_H_
