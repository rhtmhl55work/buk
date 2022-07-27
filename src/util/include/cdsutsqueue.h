#ifndef CDSU_THREAD_SAFE_QUEUE_H_
#define CDSU_THREAD_SAFE_QUEUE_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsutsqueue.h
// Description	: Thread Safe Queue template
// Author       : Titty Thomas
// Created On   : Mon Feb  3 12:10:40 IST 2003
// Last Modified: 1. Mon Aug 16 10:43:38 IST 2004
// 		: 	By : Vijaya Kumar
// 			Copy Constructor is added
//		: 2 Wed Apr 19 13:50:25 IST 2006
//			By : Vijaya Kumar
//			- Code has been changed inorder to port over Fedora 4 with backward 
//			compatibility 
//				
//============================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include "cdsutrace.h"

//----------------------------------------------------------------------------
// Class	: CdSuTsQueue
// Description	: Definitions for  class CdSuTsQueue.
//----------------------------------------------------------------------------

template <class KeyType> 
class CdSuTsQueue 
{
public:

	CdSuTsQueue (unsigned int maxqsize);
	CdSuTsQueue (CdSuTsQueue<KeyType>& queue);
	~CdSuTsQueue ();
	
	//
	bool insert(const KeyType& item);
	//
	bool add (const KeyType& item);
	bool popBlocking (KeyType& item);
	bool popNonBlocking (KeyType& item);

	bool full ();
	bool empty ();
	unsigned int getSize () { return (size); };

protected:
	bool ifull ();
	bool iempty ();

private:
	pthread_mutex_t qMutex;
	sem_t qSem;

	struct Node 
	{
		KeyType value;
		Node *next;
	};

	Node  *headNode;				
	Node  *rearNode;
	//
	Node  *rearNodePriority;
	//
	unsigned int maxsize;
	unsigned int size;

public:
	//------------------------------------------------------------------------------
	// Class	: iterator 
	// Description	: CdSuTsQueue iterator
	//------------------------------------------------------------------------------

	class iterator
	{
	public:
		iterator (Node* init = NULL);
		KeyType& operator* ();
		const KeyType& operator* () const;
		typename CdSuTsQueue<KeyType>::iterator& operator++ ();
		typename CdSuTsQueue<KeyType>::iterator operator++ (int);
		bool operator== (const CdSuTsQueue<KeyType>::iterator& iter) const;
		bool operator!= (const CdSuTsQueue<KeyType>::iterator& iter) const;
	private:
		Node* current;
		friend class CdSuTsQueue;
	}; // iterator

	typename CdSuTsQueue<KeyType>::iterator begin () const;
	typename CdSuTsQueue<KeyType>::iterator end () const;

}; // CdSuTsQueue

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::CdSuTsQueue()
// Description  : Constructor.
// Inputs       : The size of the queue.
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//		*) Initializes the mutex.
//		*) Initializes the semaphore.
//		*) Creates the headNode.
//		*) Sets the size of the queue equal to zero.
//		*) The rearNode also points to the headNode initially.
//---------------------------------------------------------------------------

template <class KeyType>
CdSuTsQueue<KeyType>::CdSuTsQueue (unsigned int maxqsize) : maxsize(maxqsize)
{
	// CDSU_TRACE (4, "CdSuTsQueue::CdSuTsQueue()\n");

	enum
	{
		CDTSQ_PSHARED = 0,
		CDTSQ_VAL = 0
	};

	pthread_mutex_init (&qMutex, NULL);
	sem_init (&qSem, CDTSQ_PSHARED, CDTSQ_VAL);
	headNode = new Node;
	headNode->next = 0;
	size = 0;
	rearNode = headNode;
	//
	rearNodePriority = headNode;
	//

} // CdSuTsQueue<KeyType>::CdSuTsQueue
	
//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::CdSuTsQueue()
// Description  : Copy Constructor.
// Inputs       : 
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
CdSuTsQueue<KeyType>::CdSuTsQueue (CdSuTsQueue<KeyType>& queue)
{
	// CDSU_TRACE (4, "CdSuTsQueue::CdSuTsQueue()\n");

	enum
	{
		CDTSQ_PSHARED = 0,
		CDTSQ_VAL = 0
	};

	pthread_mutex_init (&qMutex, NULL);
	sem_init (&qSem, CDTSQ_PSHARED, CDTSQ_VAL);
	headNode = new Node;
	headNode->next = 0;
	maxsize = queue.maxsize;
	size = 0;
	rearNode = headNode;
	//
	rearNodePriority = headNode;
	//
	for (iterator iter = queue.begin (); iter != queue.end ();
			++iter)
	{
		this->add (*iter);
	} // for

} // CdSuTsQueue<KeyType>::CdSuTsQueue
	

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::~CdSuTsQueue()
// Description  : Destructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//		*) Starting from the headNode deletes all elements in the queue.
//		*) Destroy the mutex.
//		*) Destroy the semaphore.
//---------------------------------------------------------------------------

template <class KeyType>
CdSuTsQueue<KeyType>::~CdSuTsQueue ()
{
	// CDSU_TRACE (4, "CdSuTsQueue::~CdSuTsQueue()\n");

	while (headNode != 0)
	{
		Node  *q = headNode;
		headNode = headNode->next;
		delete q;
	}
	if (pthread_mutex_destroy (&qMutex) != 0)
	{
		CDSU_TRACE (1, "CdSuTsQueue::~CdSuTsQueue(): Mutex destroy error.\n");
	}
	if (sem_destroy (&qSem) != 0)
	{
		CDSU_TRACE (1, "Semaphore destroy error.\n");
	}

} // CdSuTsQueue<KeyType>::~CdSuTsQueue ()

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::ifull()
// Description  : To find whether the queue is full or not, used internally.
// Inputs       : None.
// Return Value : true if queue is full else false.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
inline bool CdSuTsQueue<KeyType>::ifull ()
{
	if (size == maxsize)
		return 1;
	else
		return 0;

} // CdSuTsQueue<KeyType>::ifull ()

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::iempty()
// Description  : To find whether the queue is empty or not, used internally.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

template <class KeyType>
inline bool CdSuTsQueue<KeyType>::iempty ()
{
	if (size == 0) 
		return 1;
	else
		return 0;

} // CdSuTsQueue<KeyType>::ifull ()

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::full()
// Description  : To find whether the queue is full or not. This is the API 
//		  provided to check whether the queue is full or not.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :	
//		*) Lock the mutex.
//		*) Compare the current size of queue with the max size possible.
//		*) Unlock the mutex.
//---------------------------------------------------------------------------

template <class KeyType>
inline bool CdSuTsQueue<KeyType>::full ()
{
	// CDSU_TRACE (4, "CdSuTsQueue::full ()\n");

	bool retVal = false;
	if (pthread_mutex_lock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return false;
	}
	if (size == maxsize)
		retVal = true;
	else
		retVal = false;

	if (pthread_mutex_unlock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return retVal;

} // CdSuTsQueue<KeyType>::full ()

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::empty()
// Description  : To find whether the queue is empty or not
// Inputs       : 
// Return Value : 
// Side Effects : Returns true if the queue is empty, false otherwise.
// Algorithm    :
//		*) Lock the mutex.
//		*) Compare the size of the Queue with 0.
//		*) Unlock the mutex.
//---------------------------------------------------------------------------

template <class KeyType>
inline bool CdSuTsQueue<KeyType>::empty ()
{
	// CDSU_TRACE (4, "CdSuTsQueue::empty ()\n");

	bool retVal = false;
	if (pthread_mutex_lock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return false;
	}

	if (size == 0) 
		retVal = true;
	else
		retVal = false;
	
	if (pthread_mutex_unlock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return retVal;

} // CdSuTsQueue<KeyType>::empty ()

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::add()
// Description  : To add the messages in the queue.
// Inputs       : 
// Return Value : Returns true if successfully adds the element, false
// 		  otherwise.
// Side Effects : 
// Algorithm    :
//		*) Lock the mutex.
//		*) If queue is full return value is false.
//		*) Else add the node.
//		*) Increase the size of the queue.
//		*) Increment the semaphore count, return value is true.
//		*) Unlock the mutex.
//---------------------------------------------------------------------------
		
template <class KeyType>
bool CdSuTsQueue<KeyType>::add (const KeyType& x)
{
	// CDSU_TRACE (4, "CdSuTsQueue::add ()\n");

	if (pthread_mutex_lock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return false;
	}

	bool retVal = false;
	if (ifull ()) 
	{
		CDSU_TRACE1 (1, "cdqueue full %d\n", pthread_self ());
		retVal = false;
	} else
	{
		Node *n = new Node; 
		n->value = x;
		n->next = 0;
		rearNode->next = n;
		rearNode = n;
		size++;
		if (sem_post (&qSem) !=0)
		{
			CDSU_TRACE (1, "semaphore post error\n");
		}
		retVal = true;
	}
	if (pthread_mutex_unlock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuTsQueue<KeyType>::add ()


//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::insert()
// Description  : To add the priority messages in the queue.
// Inputs       : 
// Return Value : Returns true if successfully adds the element, false
// 		  otherwise.
// Side Effects : queue becomes a list
// Algorithm    :
//		*) Lock the mutex.
//		*) If queue is full return value is false.
//		*) Else add the node.
//		*) Increase the size of the queue.
//		*) Increment the semaphore count, return value is true.
//		*) Unlock the mutex.
//---------------------------------------------------------------------------

template <class KeyType>
bool CdSuTsQueue<KeyType>::insert (const KeyType& x)
{
	// CDSU_TRACE (4, "CdSuTsQueue::add ()\n");

	if (pthread_mutex_lock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return false;
	}

	bool retVal = false;
	if (ifull ()) 
	{
		CDSU_TRACE1 (1, "cdqueue full %d\n", pthread_self ());
		retVal = false;
	} else
	{
		Node *n = new Node; 
		n->value = x;
		n->next = rearNodePriority->next;
		rearNodePriority->next = n;
		rearNodePriority = n;
		size++;
	
		// To handle the case where the rearNodePriority moves forward and the rearNode stays back
		if ( rearNode->next == rearNodePriority )
			rearNode = rearNodePriority;
		//

		if (sem_post (&qSem) !=0)
		{
			CDSU_TRACE (1, "semaphore post error\n");
		}
		retVal = true;
	}
	if (pthread_mutex_unlock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuTsQueue<KeyType>::add ()
		


//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::popBlocking (KeyType& )
// Description  : Remove the message from the queue, blocking call. The call
// 		  sem_wait () suspends the calling thread until the
// 		  semaphore pointed to by qSem has non-zero count. It then
// 		  atomically decreases the semaphore count so we don't have
// 		  to decrement the size explicitly.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//		*) Call sem_wait ().
//		*) Lock the mutex.
//		*) If Queue is empty, set return value to zero.
//		*) Else remove the element, added first i.e. one attached to 
//		   the headNode.
//		*) If the Queue is empty after removing the element, set
//		   rearNode equal to headNode.  
//		*) Set the value of the item popped in the argument item.
//---------------------------------------------------------------------------

template <class KeyType>
bool CdSuTsQueue<KeyType>::popBlocking (KeyType& item)
{
	// CDSU_TRACE (4, "CdSuTsQueue::popBlocking ()\n");
	int rv;
	do 
	{
		rv = sem_wait (&qSem);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return false;
	} // if ()

	if (pthread_mutex_lock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return false;
	}
	bool retVal = false;
	if (iempty ()) 
	{
		CDSU_TRACE1 (1, "cdqueue empty %d\n", pthread_self ());
		retVal = false;
	} else
	{
		Node *t = headNode->next;
		KeyType x = t->value;
		headNode->next = t->next;
		
		// To handle the case where the rearNodePriority stays behind the headNode 
		if ( t == rearNodePriority )
			rearNodePriority = headNode;
		// 

		delete t;
		size--;
		if (iempty ())
			rearNode = headNode;
		item = x;
		retVal = true;
	}
	if (pthread_mutex_unlock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuTsQueue<KeyType>::popBlocking (KeyType& item)

//---------------------------------------------------------------------------
// Name		: CdSuTsQueue::popNonBlocking ()
// Description  : Remove the message from the queue, non-blocking call. 
//		  by sem_trywait call if the semaphore pointed to by qSem has 
//		  non-zero count, the count is atomicaaly decreased and
//		  sem_trywait immediately returns 0. If sem count is zero, it
//		  returns immediately with error EAGAIN.
// Inputs       : 
// Return Value : Returns true if popNonBlocking successfully pops something
// 		  else returns false.
// Side Effects : 
// Algorithm    :
//		*) Call sem_trywait.
//		*) Lock the mutex.
//		*) If Queue is empty, set return value to false.
//		*) Else remove the element which was inserted first, i.e.
//		one next to the headNode, set return value to true.
//		*) If queue becomes empty after the removal then set
//		rearNode equal to headNode.
//		*) Set the value of the item popped in the argument item.
//----------------------------------------------------------------------------

template <class KeyType>
bool CdSuTsQueue<KeyType>::popNonBlocking (KeyType& item)
{
	// CDSU_TRACE (4, "CdSuTsQueue::popNonBlocking ()\n");

	if (sem_trywait (&qSem) !=0)
	{
		// if (errno != EAGAIN)
		// {
		//	CDSU_TRACE (1, "semaphore wait error\n");
		//}
		return false;
	}
	if (pthread_mutex_lock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return false;
	}
	bool retVal = false;
	if (iempty ()) 
	{
		retVal = false;
	} else
	{
		Node *t = headNode->next;
		KeyType x = t->value;
		headNode->next = t->next;

		// To handle the case where the rearNodePriority stays behind the headNode 
		if ( t == rearNodePriority )
			rearNodePriority = headNode;
		// 

		delete t;
		size--;
		if (iempty ())
			rearNode = headNode;
		item = x;
		retVal = true;
	}
	if (pthread_mutex_unlock (&qMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return retVal;

} // CdSuTsQueue<KeyType>::popNonBlocking (KeyType& item)

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
typename CdSuTsQueue<KeyType>::iterator CdSuTsQueue<KeyType>::begin () const 
{
	return typename CdSuTsQueue<KeyType>::iterator (headNode->next);

} // CdSuTsQueue::begin ()

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
typename CdSuTsQueue<KeyType>::iterator CdSuTsQueue<KeyType>::end () const 
{
	return typename CdSuTsQueue<KeyType>::iterator (rearNode->next);

} // CdSuTsQueue::end ()

//------------------------------------------------------------------------------
// Function	: iterator 
// Description	: CdSuTsQueue iterator constructor.
// Inputs	: queue element pointer.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise the current queue pointer
//------------------------------------------------------------------------------

template <class KeyType>
CdSuTsQueue<KeyType>::iterator::iterator (Node* init) : current (init) 
{ 
	// Do Nothing here.	

} // CdSuTsQueue::iterator::iterator ()

//------------------------------------------------------------------------------
// Function	: operator* 
// Description	: Deference operator.
// Inputs	: None.
// Return Value	: Reference of element value at the current position,
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//------------------------------------------------------------------------------

template <class KeyType>
KeyType& CdSuTsQueue<KeyType>::iterator::operator* ()
{
	return current->value;

} // CdSuTsQueue::iterator::operator*  ()

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
const KeyType& CdSuTsQueue<KeyType>::iterator::operator* () const
{
	return current->value;

} // CdSuTsQueue::iterator::operator* ()

//------------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Prefix operator
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Change the current position to the next element in the queue.
//------------------------------------------------------------------------------

template <class KeyType>
typename CdSuTsQueue<KeyType>::iterator& CdSuTsQueue<KeyType>::iterator::operator++ ()
{
	current = current->next;
	return *this;

} // CdSuTsQueue::iterator::operator++

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
typename CdSuTsQueue<KeyType>::iterator CdSuTsQueue<KeyType>::iterator::operator++ (int)
{
	typename CdSuTsQueue<KeyType>::iterator temp = *this;
	++*this;
	return temp;

} // CdSuTsQueue::iterator::operator++

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
bool CdSuTsQueue<KeyType>::iterator::operator== (const CdSuTsQueue<KeyType>::iterator& iter) const
{
	return current == iter.current;

} // CdSuTsQueue::iterator::operator==

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
bool CdSuTsQueue<KeyType>::iterator::operator!= (const CdSuTsQueue<KeyType>::iterator& iter) const
{
	return current != iter.current;

} // CdSuTsQueue::iterator::operator!=

//============================================================================
// <End of tsqueue.h>
//============================================================================
#endif // THREAD_SAFE_QUEUE_H_
