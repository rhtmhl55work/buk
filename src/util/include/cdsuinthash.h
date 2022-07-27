#ifndef CDSU_INTHASH_H_
#define CDSU_INTHASH_H_
//============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsuinthash.h
// Description	: Defines a hash with integer key. 
// Ref Docs	:
// Author       : Titty Thomas
// Created On   : Mon Mar 17 10:22:31 IST 2003
// Last Modified: 1. Mon Aug 16 10:36:39 IST 2004
// 		: 	By: Vijaya Kumar
// 		:	Copy Constructor is Added
// 		: 2. Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------
// Class	: CdSuIntHash 
// Description	: Hash with integer key 
//----------------------------------------------------------------------------

template <class ValType> 
class CdSuIntHash
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

	CdSuIntHash (TableSize numBuckets);
	CdSuIntHash (CdSuIntHash<ValType>& intHash);
	~CdSuIntHash ();

	bool insert (unsigned int key, const ValType& value);
	bool lookup (unsigned int key, ValType& value);
	bool remove (unsigned int key);

	int size () { return numElements; }
	void clear ();
	void print ();
private:
	// Hash element
	struct HashElement
	{
		// key, value and next element in the same bucket.
		unsigned int key;
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
	bool keyMatch (HashElement* el, unsigned int key); 
	HashElement* getElement (void);
	void freeElement (HashElement* el);
	
	// Hash functions.
	unsigned int hash (unsigned int key);

public:
	//--------------------------------------------------------------------
	// Class	: iterator 
	// Description	: CdSuIntHash iterator
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
		friend class CdSuIntHash;
	}; // iterator

	iterator begin () const;
	iterator end () const;

}; // CdSuIntHash

//----------------------------------------------------------------------------
// Function	: CdSuIntHash 
// Description	: CdSuIntHash constructor.
// Inputs	: TableSize, size (number of buckets) of hash table.
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: Allocate memory for the buckets and initialise the variables.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuIntHash<ValType>::CdSuIntHash (CdSuIntHash::TableSize numBuckets)
{
	buckets = new HashBucket [numBuckets];
	this->numBuckets = numBuckets;
	memset (buckets, 0, numBuckets * sizeof (HashBucket)); 
	chain.next = &chain;
	cache = NULL;
	numElements = 0;

} // CdSuIntHash::CdSuIntHash ()

//----------------------------------------------------------------------------
// Function	: CdSuIntHash 
// Description	: Copy Constructor
// Inputs	: 
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: Allocate memory for the buckets and initialise the variables.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuIntHash<ValType>::CdSuIntHash (CdSuIntHash<ValType>& intHash)
{
	buckets = new HashBucket [intHash.numBuckets];
	this->numBuckets = intHash.numBuckets;
	memset (buckets, 0, this->numBuckets * sizeof (HashBucket)); 
	chain.next = &chain;
	cache = NULL;
	numElements = 0;
	for (iterator iter = intHash.begin (); iter != intHash.end (); ++iter)
	{
		
		this->insert (iter.curElem->key, iter.curElem->value);
	} // for

} // CdSuIntHash::CdSuIntHash ()


//----------------------------------------------------------------------------
// Function	: ~CdSuIntHash 
// Description	: CdSuIntHash destructor.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Delete the elements and the buckets.
//----------------------------------------------------------------------------

template <class ValType> 
CdSuIntHash<ValType>::~CdSuIntHash ()
{
	/*for (int i = 0; i < numBuckets; ++i)
	{
		freeBucketList (buckets[i].first);
	}*/
	this->clear ();
	delete [] buckets;

} // CdSuIntHash::~CdSuIntHash ()

//----------------------------------------------------------------------------
// Function	: keyMatch
// Description	: Compares keys.
// Inputs	: HashElement and key.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Compare the key of the element and the given key.
//----------------------------------------------------------------------------

template <class ValType> 
bool CdSuIntHash<ValType>::keyMatch (HashElement* el, unsigned int key)
{
	if (el->key == key)
	{
		return true;
	} else
	{
		return false;
	}

} // CdSuIntHash::keyMatch ()

//----------------------------------------------------------------------------
// Function	: getElement
// Description	: Gets a hash element.
// Inputs	: None.
// Return Value	: HashElement
// Side Effects	: None.
// Algorithm	: 
//----------------------------------------------------------------------------

template <class ValType> 
typename CdSuIntHash<ValType>::HashElement* CdSuIntHash<ValType>::getElement (void)
{
	HashElement* el = new HashElement;
	return (el);

} // CdSuIntHash::getElement ()

//----------------------------------------------------------------------------
// Function	: freeElement
// Description	: Free an element.
// Inputs	: HashElement.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	 
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuIntHash<ValType>::freeElement (HashElement* el)
{
	delete el;

} // CdSuIntHash::freeElement ();

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
bool CdSuIntHash<ValType>::insert (unsigned int key, const ValType& value)
{
	HashBucket* bucket = &buckets [hash (key)];
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

	newElem->key = key;
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

} // CdSuIntHash::insert ()

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
bool CdSuIntHash<ValType>::lookup (unsigned int key, ValType& value)
{
	bool retVal = false;
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

} // CdSuIntHash::lookup ()

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
bool CdSuIntHash<ValType>::remove (unsigned int key)
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

} // CdSuIntHash::remove

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
void CdSuIntHash<ValType>::clear ()
{
	for (int i = 0; i < numBuckets; ++i)
	{
		freeBucketList (buckets[i].first);
		buckets[i].first = NULL;
	}
	chain.next = &chain;
	numElements = 0;

} // CdSuIntHash<ValType>::clear ()

//----------------------------------------------------------------------------
// Function	: freeBucketList
// Description	: Free the elements of a bucket.
// Inputs	: Beginning of the list.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Deletes all the elements of the list
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuIntHash<ValType>::freeBucketList (HashElement* el)
{
	HashElement* next;
	while (el)
	{
		next = el->next;
		delete el;
		el = next;
	}

} // CdSuIntHash::freeBucketList

//----------------------------------------------------------------------------
// Function	: print 
// Description	: Print the elements.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Print all the elements. 
//----------------------------------------------------------------------------

template <class ValType> 
void CdSuIntHash<ValType>::print ()
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

} // CdSuIntHash::print ()

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
unsigned int CdSuIntHash<ValType>::hash (unsigned int key)
{

	return (key % numBuckets);

} // CdSuIntHash::hash ()

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
typename CdSuIntHash<ValType>::iterator CdSuIntHash<ValType>::begin () const 
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

} // CdSuIntHash::begin ()

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
typename CdSuIntHash<ValType>::iterator CdSuIntHash<ValType>::end () const
{

	return iterator ((HashBucket*) &chain);

} // CdSuIntHash::end ()

//----------------------------------------------------------------------------
// Function	: iterator 
// Description	: CdSuIntHash iterator constructor.
// Inputs	: list element pointer.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise the current list pointer
//----------------------------------------------------------------------------

template <class ValType>
CdSuIntHash<ValType>::iterator::iterator (HashBucket* bucket, HashElement*
		elem, const HashBucket* end)
		: curBucket (bucket), curElem (elem), endOfChain (end)
{ 
	// Nothing to do.

} // CdSuIntHash::iterator::iterator ()

//----------------------------------------------------------------------------
// Function	: operator* 
// Description	: Deference operator.
// Inputs	: None.
// Return Value	: Reference of element value at the current position,
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//----------------------------------------------------------------------------

template <class ValType>
ValType& CdSuIntHash<ValType>::iterator::operator* ()
{
	return curElem->value;

} // CdSuIntHash::iterator::operator*  ()
 
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
const ValType& CdSuIntHash<ValType>::iterator::operator* () const
{
	return curElem->value;

} // CdSuIntHash::iterator::operator* ()

//----------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Prefix operator
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Change the current position to the next element in the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuIntHash<ValType>::iterator& CdSuIntHash<ValType>::iterator::operator++ ()
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

} // CdSuIntHash::iterator::operator++

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
typename CdSuIntHash<ValType>::iterator CdSuIntHash<ValType>::iterator::operator++ (int)
{
	iterator temp = *this;
	++*this;

	return temp;

} // CdSuIntHash::iterator::operator++

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
bool CdSuIntHash<ValType>::iterator::operator== (const iterator& iter) const
{
	return ((curBucket == iter.curBucket) && (curElem == iter.curElem));

} // CdSuIntHash::iterator::operator==

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
bool CdSuIntHash<ValType>::iterator::operator!= (const iterator& iter) const
{
	return ((curBucket != iter.curBucket) || (curElem != iter.curElem));

} // CdSuIntHash::iterator::operator!=

//============================================================================
// <End of cdsuinthash.h>
//============================================================================
#endif // CDSU_INTHASH_H_

