#ifndef CDSU_CHAR_HASH_H_
#define CDSU_CHAR_HASH_H_

//============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsuinthash.h
// Description	: Defines a hash with char* key.
// Ref Docs	:
// Author       : Titty Thomas
// Created On   : Mon Mar 17 10:22:31 IST 2003
// Last Modified: 1. Fri Jul 30 12:04:07 IST 2004
// 		: 	Modified By Vijaya Kumar
//		: 	warning messages are solved.
//		: 2. Mon Aug 16 10:35:42 IST 2004
//		:	By : Vijaya Kumar
//		:	Copy Constructor is added
//		: 3. Wed Dec 16 15:03:40 IST 2009
//			By : Anish, Arun
//============================================================================

//----------------------------------------------------------------------------
// Include Files.
//----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "cdsutrace.h"

//----------------------------------------------------------------------------
// Class	: CdSuCharHash 
// Description	: Hash with char* as key.
//----------------------------------------------------------------------------

template <class ValType> 
class CdSuCharHash
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

	enum 
	{
		DEFAULT_KEY_LEN = 128			

	};

	CdSuCharHash (TableSize numBuckets, HashFunctionType hft = HFT_DEFAULT,
			int mkl = DEFAULT_KEY_LEN);
	CdSuCharHash (CdSuCharHash <ValType>& charHash);
	~CdSuCharHash ();

	bool insert (char* key, int keylen, const ValType& value);
	bool lookup (char* key, int keylen, ValType& value);
	bool remove (char* key, int keylen);

	int size () { return numElements; }
	void clear ();
	void print ();

private:

	// Hash element
	struct HashElement
	{
		// key, value and next element in the same bucket.
		char* key;
		int keyLen;
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
	bool keyMatch (HashElement* el, char* key, int keylen); 
	HashElement* getElement (void);
	void freeElement (HashElement* el);
	
	// Hash functions.
	unsigned int hash (char* key, int keylen);
	unsigned int hashFunctionDefault (char* key, int keylen);	
	unsigned int hashFunction1 (char* key, int keylen);
	
	HashFunctionType hashFunctionType;
	int maxKeyLen;

public:
	//--------------------------------------------------------------------
	// Class	: iterator 
	// Description	: CdSuCharHash iterator
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
		friend class CdSuCharHash;
	}; // iterator

	iterator begin () const;
	iterator end () const;

}; // CdSuCharHash

//----------------------------------------------------------------------------
// Function	: CdSuCharHash 
// Description	: CdSuCharHash constructor.
// Inputs	: TableSize, size (number of buckets) of hash table.
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: Allocate memory for the buckets and initialise the variables.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuCharHash<ValType>::CdSuCharHash (CdSuCharHash::TableSize numBuckets,
		CdSuCharHash::HashFunctionType hft, int mkl)
{
	buckets = new HashBucket [numBuckets];
	this->numBuckets = numBuckets;
	memset (buckets, 0, numBuckets * sizeof (HashBucket)); 
	chain.first = NULL;
	chain.next = &chain;
	cache = NULL;
	numElements = 0;
	hashFunctionType = hft;
	maxKeyLen = mkl;

} // CdSuCharHash::CdSuCharHash ()

//----------------------------------------------------------------------------
// Function	: CdSuCharHash 
// Description	: CdSuCharHash Copy Constructor constructor.
// Inputs	: 
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: Allocate memory for the buckets and initialise the variables.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuCharHash<ValType>::CdSuCharHash (CdSuCharHash<ValType>& charHash)
{
	buckets = new HashBucket [charHash.numBuckets];
	this->numBuckets = charHash.numBuckets;
	memset (buckets, 0, charHash.numBuckets * sizeof (HashBucket)); 
	chain.first = NULL;
	chain.next = &chain;
	cache = NULL; // charHash.cache;
	numElements = 0; // charHash.numElements;
	hashFunctionType = charHash.hashFunctionType;
	maxKeyLen = charHash.maxKeyLen;
	for (iterator iter = charHash.begin (); iter != charHash.end (); ++iter)
	{
		
		this->insert (iter.curElem->key, iter.curElem->keyLen, 
				iter.curElem->value);
	}

} // CdSuCharHash::CdSuCharHash ()
 

//----------------------------------------------------------------------------
// Function	: ~CdSuCharHash 
// Description	: CdSuCharHash destructor.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Delete the elements and the buckets.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuCharHash<ValType>::~CdSuCharHash ()
{
	/*
	for (int i = 0; i < numBuckets; ++i)
	{
		freeBucketList (buckets[i].first);
	}*/
	
	clear ();
	delete [] buckets;
	
} // CdSuCharHash::~CdSuCharHash ()

//----------------------------------------------------------------------------
// Function	: keyMatch
// Description	: Compares keys.
// Inputs	: HashElement and key.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Compare the key of the element and the given key.
//----------------------------------------------------------------------------

template <class ValType> 
bool CdSuCharHash<ValType>::keyMatch (HashElement* el, char* p, int keylen)
{
	if (el->keyLen != keylen)
		return false;

	if ((memcmp (el->key, p, keylen)) == 0)
	{
		// If Key matches with the Key of the element.
		return true;
	} else
	{
		// If Key does not match with the Key of the element.
		return false;
	}

} // CdSuCharHash::keyMatch ()

//----------------------------------------------------------------------------
// Function	: getElement
// Description	: Gets a hash element.
// Inputs	: None.
// Return Value	: HashElement
// Side Effects	: None.
// Algorithm	: 
//----------------------------------------------------------------------------

template <class ValType> 
typename CdSuCharHash<ValType>::HashElement* CdSuCharHash<ValType>::getElement (void)
{
	HashElement* el = new HashElement;
	return (el);

} // CdSuCharHash::getElement ()

//----------------------------------------------------------------------------
// Function	: freeElement
// Description	: Free an element.
// Inputs	: HashElement.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	 
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuCharHash<ValType>::freeElement (HashElement* el)
{
	delete [] el->key;
	delete el;

} // CdSuCharHash::freeElement ();

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
bool CdSuCharHash<ValType>::insert (char* key, int keylen, const ValType& value)
{
	if (keylen > maxKeyLen)
	{
		return false;
	}

	HashBucket* bucket = &buckets [hash (key, keylen)];
	HashElement* elem = bucket->first;

	while (elem)
	{
		if (keyMatch (elem, key, keylen))
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
	
	newElem->keyLen = keylen;
	newElem->key = new char [keylen];
	memcpy (newElem->key, key, keylen);
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

} // CdSuCharHash::insert ()

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
bool CdSuCharHash<ValType>::lookup (char* key, int keylen, ValType& value)
{
	if (keylen > maxKeyLen)
	{
		return false;
	}

	bool retVal = false;
	HashElement* elem = cache;
	if (elem && keyMatch (elem, key, keylen))
	{
		// Cache hit.
		value = elem->value;
		retVal = true;
	} else
	{
		elem = buckets [hash (key, keylen)].first;
		while (elem)
		{
			if (keyMatch (elem, key, keylen))
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

} // CdSuCharHash::lookup ()

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
bool CdSuCharHash<ValType>::remove (char* key, int keylen)
{
	if (keylen > maxKeyLen)
	{
		return false;
	}
	HashElement* elem, **prevElem;
	HashBucket* bucket = &buckets [hash (key, keylen)];
	prevElem = &bucket->first;
	elem = bucket->first;
	while (elem)
	{
		if (keyMatch (elem, key, keylen))
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

} // CdSuCharHash::remove

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
void CdSuCharHash<ValType>::clear ()
{
	for (int i = 0; i < numBuckets; ++i)
	{
		freeBucketList (buckets[i].first);
		buckets[i].first = NULL;
	}
	chain.next = &chain;
	numElements = 0;

} // CdSuCharHash<ValType>::clear ()

//----------------------------------------------------------------------------
// Function	: freeBucketList
// Description	: Free the elements of a bucket.
// Inputs	: Beginning of the list.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Deletes all the elements of the list
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuCharHash<ValType>::freeBucketList (HashElement* el)
{
	HashElement* next;
	while (el)
	{
		next = el->next;
		delete [] el->key;
		delete el;
		el = next;
	}

} // CdSuCharHash::freeBucketList

//----------------------------------------------------------------------------
// Function	: print 
// Description	: Print the elements.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Print all the elements. 
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuCharHash<ValType>::print ()
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

} // CdSuCharHash::print ()

//----------------------------------------------------------------------------
// Function	: hash
// Description	: Hash function
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: User has the choice to specify which particular Hash
// 		  Function is to be used.
//----------------------------------------------------------------------------

template <class ValType> 
unsigned int CdSuCharHash<ValType>::hash (char* key, int keylen )
{
	switch (hashFunctionType)
	{
	case HFT_DEFAULT:
		{
		unsigned int hash = hashFunctionDefault (key, keylen);
		return (hash);
		}
		break;
	
	case HFT_ONE:
		{
		unsigned int hash = hashFunction1 (key, keylen);
		return (hash);
		}
		break;
	default:
		return -1;// This is added to just to remove warning message
		break;
	}

} // CdSuCharHash::hash ()

//----------------------------------------------------------------------------
// Function	: hashFunctionDefault
// Description	: Default hash function used when the user does not specify 
// 		  any particular preference for the hash function.
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: This algorithm is taken from gcc compiler source.
//----------------------------------------------------------------------------

template <class ValType> 
unsigned int CdSuCharHash<ValType>::hashFunctionDefault (char* s, int keylen)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	unsigned int hash = 0;
	unsigned char c;
	unsigned int len = 0;

	for (int i = 0; i < keylen; i++)
	{	
		c = s[i];
		hash += c + (c << 17);
		hash ^= hash >> 2;
	} // for.

	hash += len + (len << 17);
	hash ^= hash >> 2;

	return (hash % numBuckets);

} // CdSuCharHash::hashFunctionDefault

//----------------------------------------------------------------------------
// Function	: hashFunction1
// Description	: Default hash function used when the user does not specify 
// 		  any particular preference for the hash function.
// Inputs	: Key
// Return Value	: Hash value
// Side Effects	: None.
// Algorithm	: This algorithm is taken from the book:
//		  "Introduction to Compiling Techniques"
//		  "A First Course using ANSI C, LEX and YACC"
//			-J.P.Bennett.
//----------------------------------------------------------------------------

template <class ValType> 
unsigned int CdSuCharHash<ValType>::hashFunction1 (char* key, int keylen)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	int hv = 0;
	
	char buf [keylen];
	memcpy (buf, key, keylen);

	for (int i = 0; i < keylen ; ++i)
	{	
		int v = (hv >> 28) ^ (buf[i] & 0xf);
		hv = (hv << 4) | v;
	}
	hv = hv & 0x7fffffff;
	
	return (hv % numBuckets);

} // CdSuCharHash::hashFunction1 ()

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
typename CdSuCharHash<ValType>::iterator CdSuCharHash<ValType>::begin () const 
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

} // CdSuCharHash::begin ()

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
typename CdSuCharHash<ValType>::iterator CdSuCharHash<ValType>::end () const
{

	return iterator ((HashBucket*) &chain);

} // CdSuCharHash::end ()

//----------------------------------------------------------------------------
// Function	: iterator 
// Description	: CdSuCharHash iterator constructor.
// Inputs	: list element pointer.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise the current list pointer
//----------------------------------------------------------------------------

template <class ValType>
CdSuCharHash<ValType>::iterator::iterator (HashBucket* bucket, HashElement*
		elem , const HashBucket* end)
		: curBucket (bucket), curElem (elem), endOfChain (end)
{ 
	// Nothing to do.

} // CdSuCharHash::iterator::iterator ()

//----------------------------------------------------------------------------
// Function	: operator* 
// Description	: Deference operator.
// Inputs	: None.
// Return Value	: Reference of element value at the current position,
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//----------------------------------------------------------------------------

template <class ValType>
ValType& CdSuCharHash<ValType>::iterator::operator* ()
{
	return curElem->value;

} // CdSuCharHash::iterator::operator*  ()
 
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
const ValType& CdSuCharHash<ValType>::iterator::operator* () const
{
	return curElem->value;

} // CdSuCharHash::iterator::operator* ()

//----------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Prefix operator
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Change the current position to the next element in the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuCharHash<ValType>::iterator& CdSuCharHash<ValType>::iterator::operator++ ()
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

} // CdSuCharHash::iterator::operator++

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
typename CdSuCharHash<ValType>::iterator CdSuCharHash<ValType>::iterator::operator++ (int)
{
	iterator temp = *this;
	++*this;

	return temp;

} // CdSuCharHash::iterator::operator++

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
bool CdSuCharHash<ValType>::iterator::operator== (const iterator& iter) const
{
	return ((curBucket == iter.curBucket) && (curElem == iter.curElem));

} // CdSuCharHash::iterator::operator==

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
bool CdSuCharHash<ValType>::iterator::operator!= (const iterator& iter) const
{
	return ((curBucket != iter.curBucket) || (curElem != iter.curElem));

} // CdSuCharHash::iterator::operator!=

//============================================================================
// <End of cdsuinthash.h>
//============================================================================
#endif // CDSU_INTHASH_H_

