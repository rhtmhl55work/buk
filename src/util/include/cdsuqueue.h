
#ifndef __CDSUQUEUE_H_
#define __CDSUQUEUE_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsuqueue.h
// Description	: Definations for class CdSuQueue.
// Author       : 
// Created On   : Jun 3, 2003
// Last Modified: 1. Jun 3, 2003
// 		: 2. Mon Aug 16 10:41:09 IST 2004
// 		: 	By Vijaya Kumar
// 			Copy constructor is added.
//============================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "cdsutrace.h"
#include <stdio.h>

//----------------------------------------------------------------------------
// class	: CdSuQueue
// Description	: Definitions for  class CdSuQueue.
//----------------------------------------------------------------------------

template <class KeyType> 
class CdSuQueue 
{
public:
	CdSuQueue (unsigned long maxqsize);
	CdSuQueue (CdSuQueue<KeyType>& queue);
	~CdSuQueue ();

	bool full ();
	bool empty ();

	bool add (const KeyType& item);

	unsigned int getSize () { return (size); };
	bool pop (KeyType&);

protected:
	// nothing

private:
	struct Node 
	{
		KeyType value;
		Node *next;
	};
	Node* headNode;				
	Node* rearNode;
	unsigned long maxsize;
	unsigned long size;

public:

	//------------------------------------------------------------------------------
	// Class	: iterator 
	// Description	: CdSuQueue iterator
	//------------------------------------------------------------------------------

	class iterator
	{
	public:
		iterator (Node* init = NULL);
		KeyType& operator* ();
		const KeyType& operator* () const;
		iterator& operator++ ();
		iterator operator++ (int);
		bool operator== (const iterator& iter) const;
		bool operator!= (const iterator& iter) const;
	private:
		Node* current;
		friend class CdSuQueue;
	}; // iterator

	iterator begin () const;
	iterator end () const;

}; // class CdSuQueue 

//---------------------------------------------------------------------------
// Name		: CdSuQueue::CdSuQueue()
// Description  : Constructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
CdSuQueue<KeyType>::CdSuQueue (unsigned long maxqsize) : maxsize(maxqsize)
{
	size = 0;
	headNode = new Node;
	if (headNode != 0)
	{
		headNode->next = 0;
	} 
	rearNode = headNode;

} // CdSuQueue<KeyType>::CdSuQueue ()

//---------------------------------------------------------------------------
// Name		: CdSuQueue::CdSuQueue()
// Description  : Constructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
CdSuQueue<KeyType>::CdSuQueue (CdSuQueue<KeyType>& queue) 
{
	maxsize = queue.maxsize;
	size = 0;
	headNode = new Node;
	if (headNode != 0)
	{
		headNode->next = 0;
	} 
	rearNode = headNode;
	
	for (iterator iter = queue.begin (); iter != queue.end ();
			++iter)
	{
		this->add (*iter);
	} // for
} // CdSuQueue<KeyType>::CdSuQueue ()


//---------------------------------------------------------------------------
// Name		: CdSuQueue::~CdSuQueue()
// Description  : Destructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
CdSuQueue<KeyType>::~CdSuQueue ()
{
	while (headNode != 0)
	{
		Node* q = headNode;
		headNode = headNode->next;
		delete q;
	}

} // CdSuQueue<KeyType>::~CdSuQueue ()

//---------------------------------------------------------------------------
// Name		: CdSuQueue::full()
// Description  : To find whether the queue is full or not.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
inline bool CdSuQueue<KeyType>::full ()
{
	if (size == maxsize)
	{
		return 1;
	} else
	{
		return 0;
	}

} // CdSuQueue<KeyType>::full ()

//---------------------------------------------------------------------------
// Name		: CdSuQueue::empty()
// Description  : To find whether the queue is empty or not.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
inline bool CdSuQueue<KeyType>::empty ()
{
	if (size == 0) 
	{
		return 1;
	} else
	{
		return 0;
	}

} // CdSuQueue<KeyType>::empty ()

//---------------------------------------------------------------------------
// Name		: CdSuQueue::add()
// Description  : To add the messages in the queue.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------
		
template <class KeyType>
bool CdSuQueue<KeyType>::add (const KeyType& x)
{
	
	if (full ()) 
	{
		CDSU_TRACE (1, "CdSuQueue::add (): Error: Queue size exceeded! Not added.\n");
		return false;
	} else
	{
		Node *n = new Node; 
		if (n != NULL)
		{
			n->value = x;
			n->next = 0;
			rearNode->next = n;
			rearNode = n;
			size++;
		} else
		{
			CDSU_TRACE (1, "CdSuQueue::add (): Error: new failed! Not added. \n");
			return false;
		}
	} // if

	return true;

} // CdSuQueue<KeyType>::add ()

//---------------------------------------------------------------------------
// Name		: CdSuQueue::add()
// Description  : Remove the message from the queue. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
bool CdSuQueue<KeyType>::pop (KeyType& value)
{
	bool retVal = false;
	if (!empty ()) 
	{
		Node *t = headNode->next;
		KeyType x = t->value;
		headNode->next = t->next;
		delete t;
		size--;
		if (empty ())
		{
			rearNode = headNode;
		}
		value = x;
		retVal = true;
	}

	return retVal;

} // CdSuQueue<KeyType>::pop ()

//------------------------------------------------------------------------------
// Function	: begin
// Description	: Returns an iterator pointing to the beginning of the queue.
// Inputs	: None. 
// Return Value	: Iterator
// Side Effects	: None.
// Algorithm	: Creates and returns an iterator pointing to the beginning of
//		  the queue.
//------------------------------------------------------------------------------

template <class KeyType>
typename CdSuQueue<KeyType>::iterator CdSuQueue<KeyType>::begin () const 
{
	return iterator (headNode->next);

} // CdSuQueue::begin ()

//------------------------------------------------------------------------------
// Function	: end
// Description	: Returns an iterator pointing to the end of the queue.
// Inputs	: None. 
// Return Value	: Iterator
// Side Effects	: None.
// Algorithm	: Creates and returns an iterator pointing to the end of
//		  the queue.
//------------------------------------------------------------------------------

template <class KeyType>
typename CdSuQueue<KeyType>::iterator CdSuQueue<KeyType>::end () const 
{
	return iterator (rearNode->next);

} // CdSuQueue::end ()

//------------------------------------------------------------------------------
// Function	: iterator 
// Description	: CdSuQueue iterator constructor.
// Inputs	: queue element pointer.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise the current queue pointer
//------------------------------------------------------------------------------

template <class KeyType>
CdSuQueue<KeyType>::iterator::iterator (Node* init) : current (init) 
{ 
	// Do Nothing here.	

} // CdSuQueue::iterator::iterator ()

//------------------------------------------------------------------------------
// Function	: operator* 
// Description	: Deference operator.
// Inputs	: None.
// Return Value	: Reference of element value at the current position,
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//------------------------------------------------------------------------------

template <class KeyType>
KeyType& CdSuQueue<KeyType>::iterator::operator* ()
{
	return current->value;

} // CdSuQueue::iterator::operator*  ()

//------------------------------------------------------------------------------
// Function	: operator* 
// Description	: Constant dereference operator. 
// Inputs	: None.
// Return Value	: Constant reference of the element value at the current
//		  position. 
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//------------------------------------------------------------------------------

template <class KeyType>
const KeyType& CdSuQueue<KeyType>::iterator::operator* () const
{
	return current->value;

} // CdSuQueue::iterator::operator* ()

//------------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Prefix operator
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Change the current position to the next element in the queue.
//------------------------------------------------------------------------------

template <class KeyType>
typename CdSuQueue<KeyType>::iterator& CdSuQueue<KeyType>::iterator::operator++ ()
{
	current = current->next;
	return *this;

} // CdSuQueue::iterator::operator++

//------------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Postfix operator 
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Save the current position. Go to the next element. Return the
//		  saved position.
//------------------------------------------------------------------------------

template <class KeyType>
typename CdSuQueue<KeyType>::iterator CdSuQueue<KeyType>::iterator::operator++ (int)
{
	iterator temp = *this;
	++*this;
	return temp;

} // CdSuQueue::iterator::operator++

//------------------------------------------------------------------------------
// Function	: operator== 
// Description	: Tests equality of two iterators.
// Inputs	: Iterator.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Return true if iterators point to the same position, else
//		  false.
//------------------------------------------------------------------------------

template <class KeyType>
bool CdSuQueue<KeyType>::iterator::operator== (const iterator& iter) const
{
	return current == iter.current;

} // CdSuQueue::iterator::operator==

//------------------------------------------------------------------------------
// Function	: operator!= 
// Description	: Tests the inequality of two iterators.
// Inputs	: Iterator.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Return true if iterators point to differnet positions, else
//		  false
//------------------------------------------------------------------------------

template <class KeyType>
bool CdSuQueue<KeyType>::iterator::operator!= (const iterator& iter) const
{
	return current != iter.current;

} // CdSuQueue::iterator::operator!=

//============================================================================
// <end of CdSuQueue.h>
//============================================================================
#endif
