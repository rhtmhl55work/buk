//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsuthread.cpp
// Description  : Thread class implementation 
// Author       : Titty Thomas
// Created On   : Tue Feb 18 14:09:17 IST 2003
// Last Modified:
//============================================================================

#include <string.h>
#include "cdsuthread.h"
#include "cdsutrace.h"

#if (defined __ECOS || defined __sun) 
static char defThreadsStack
	[CDSU_THREAD_NUM_THREADS_DEF * CdSuThread::CDSU_THREAD_STACK_SIZE_DEF];
static int threadNo = 1;
#endif


//----------------------------------------------------------------------------
// Function	: CdSuThread 
// Description	: Constructor, does NOT start a new thread for control
//----------------------------------------------------------------------------

CdSuThread::CdSuThread() : threadId (CDSU_THREAD_ID_INVALID)
{
	memset (&threadAttr, 0, sizeof (threadAttr));

} // CdSuThread::CdSuThread ()

//----------------------------------------------------------------------------
// Function	: ~CdSuThread 
// Description	: Destructor, does NOT stop thre thread for control
//----------------------------------------------------------------------------

CdSuThread::~CdSuThread ()
{
	if (threadId != CDSU_THREAD_ID_INVALID)
	{
		// warn user but don't kill thread
		CDSU_TRACE1 (1, "Active thread: %d going out of scope\n", threadId);
	}
	threadId = CDSU_THREAD_ID_INVALID;
	pthread_attr_destroy (&threadAttr);

} // CdSuThread::~CdSuThread ()

//----------------------------------------------------------------------------
// Function	: spawn 
// Description	: Creates a new thread of control, with the specified
//		  attributes, running startFunc with startArgs. The stackSize is
//		  not used now.
//----------------------------------------------------------------------------

int CdSuThread::spawn (void* (*startFunc) (void *), void *startArgs, 
		unsigned long flags, unsigned long priority, 
		unsigned long stackSize)
{
	int retVal;
	if (threadId != CDSU_THREAD_ID_INVALID)
	{
		// return without spawning another thread
		CDSU_TRACE1 (1, "CdSuThread: %d is already active\n", threadId);
		return (CDSU_ERROR); 
	}

	retVal = pthread_attr_init (&threadAttr);
	if (retVal != 0)
	{
		CDSU_TRACE1 (1, "CdSuThread: %d attr_init error\n", threadId);
		return (CDSU_ERROR);
	}

	// extract scheduling preference from flags and set scheduling
	// policy accordingly
	switch (flags & CDSU_THREAD_SCHED_MASK)
	{
	case CDSU_THREAD_SCHED_FIFO:
		{
		retVal = pthread_attr_setinheritsched (&threadAttr,
			PTHREAD_EXPLICIT_SCHED);
		if (retVal != 0)
		{
			CDSU_TRACE1 (1, "CdSuThread: %d attr_setinheritsched error\n", threadId);
			return (CDSU_ERROR);
		}
		
		retVal = pthread_attr_setschedpolicy (&threadAttr,
			SCHED_FIFO);
		if (retVal != 0)
		{
			CDSU_TRACE1 (1, "CdSuThread: %d attr_setschedpolicy error\n", threadId);
			return (CDSU_ERROR);
		}
		}
		break;

	case CDSU_THREAD_SCHED_RR:
		{
		retVal = pthread_attr_setinheritsched (&threadAttr,
			PTHREAD_EXPLICIT_SCHED);
		if (retVal != 0)
		{
			CDSU_TRACE1 (1, "CdSuThread: %d attr_setinheritsched error\n", threadId);
			return (CDSU_ERROR);
		}
		retVal = pthread_attr_setschedpolicy (&threadAttr, SCHED_RR);
		if (retVal != 0)
		{
			CDSU_TRACE1 (1, "CdSuThread: %d attr_setschedpolicy error\n", threadId);
			return (CDSU_ERROR); 
		}
		}
		break;

	case CDSU_THREAD_SCHED_DEF:
	default:
		{
		// inherit scheduling policy of parent as default
		}
		break;

	}

	// if anything except default, set scheduling priority explicitly;
	// note that by default the priority of the parent thread is inherited
	if (priority != CDSU_THREAD_PRIORITY_DEF)
	{
		struct sched_param priorityParams;
		priorityParams.sched_priority = priority;
		retVal = pthread_attr_setschedparam (&threadAttr,
					     &priorityParams);
		if (retVal !=0)
		{
			CDSU_TRACE1 (1, "CdSuThread: %d attr_setschedparam error\n", threadId);
			return (CDSU_ERROR);
		}
	}
	
	// set the stack addr and size
#if (defined __ECOS || defined __sun) 
	    retVal = pthread_attr_setstackaddr (&threadAttr, defThreadsStack +
		    (threadNo * CDSU_THREAD_STACK_SIZE_DEF));
	    if (retVal != 0)
	    {
		    CDSU_TRACE1 (1, "CdSuThread: %d pthread_attr_setstackaddr error\n", threadId);
		    return (CDSU_ERROR);
	    }
	    retVal = pthread_attr_setstacksize 
	    		(&threadAttr, CDSU_THREAD_STACK_SIZE_DEF); 
	    if (retVal != 0)
	    {
		    CDSU_TRACE1 (1, "CdSuThread: %d pthread_attr_setstacksize error\n", threadId);
		    return (CDSU_ERROR);
	    }
	    ++threadNo;
#endif

#ifdef __sun
	    // set the scope to system wide.
	    pthread_attr_setscope(&threadAttr, PTHREAD_SCOPE_SYSTEM);
#endif


	// spawn the thread
	retVal = pthread_create (&threadId, &threadAttr, startFunc, startArgs);
	if (retVal != 0)
	{
		CDSU_TRACE1 (1, "CdSuThread: %d pthread_create error\n", threadId);
		return (CDSU_ERROR);
	}
	return (CDSU_SUCCESS);

} // CdSuThread::spawn ()

//----------------------------------------------------------------------------
// Function	: join 
// Description	: wait for the thread to exit 
//----------------------------------------------------------------------------

int CdSuThread::join(void **status)
{
	int retVal = pthread_join (threadId, status);
	threadId = CDSU_THREAD_ID_INVALID;
	if (retVal != 0)
	{
		CDSU_TRACE1 (1, "CdSuThread: %d pthread_join error\n", threadId);
		return (CDSU_ERROR);
	}
	return (CDSU_SUCCESS);

} // CdSuThread::join ()

//----------------------------------------------------------------------------
// Function	: getPriority 
// Description	: returns the priority of the thread
//----------------------------------------------------------------------------

int CdSuThread::getPriority () const
{
	struct sched_param priorityParams;
	int retVal = pthread_attr_getschedparam( &threadAttr, &priorityParams );
	if (retVal)
	{
		CDSU_TRACE1 (1, "CdSuThread: %d attr_getshedparam error\n", threadId);
		return (CDSU_ERROR);
	}
	return (priorityParams.sched_priority);

} // CdSuThread::getPriority ()

//----------------------------------------------------------------------------
// Function	: setPriority 
// Description	: sets the priority of the thread
//----------------------------------------------------------------------------

int CdSuThread::setPriority (int priority)
{
	// TODO: 
	return (CDSU_SUCCESS);

} // CdSuThread::setPriority ()

//----------------------------------------------------------------------------
// Function	: getId
// Description	: returns the unique id of the thread
//----------------------------------------------------------------------------

const pthread_t CdSuThread::getId () const
{
	return (threadId);

} // CdSuThread::getId ()


//----------------------------------------------------------------------------
// Function	: getAttributes
// Description	: returns the attributes of the thread
//----------------------------------------------------------------------------

const pthread_attr_t* CdSuThread::getAttributes() const
{
	return (&threadAttr);

} // CdSuThread::getAttributes ()

//----------------------------------------------------------------------------
// Function	: exit 
// Description	: exit the current thread of control associated with the thread
//----------------------------------------------------------------------------

void CdSuThread::exit()
{
	if (threadId != CDSU_THREAD_ID_INVALID)
	{
		int retVal = pthread_cancel (threadId);
		if (retVal != 0)
		{
			CDSU_TRACE1 (1, "CdSuThread: %d pthread_cancel error\n", threadId);
		}
		threadId = CDSU_THREAD_ID_INVALID;
	}

} // CdSuThread::exit ()

//----------------------------------------------------------------------------
// Function	: selfId 
// Description	: returns the id of the current thread
//----------------------------------------------------------------------------
const pthread_t CdSuThread::selfId()
{
	return (pthread_self ());

} // CdSuThread::selfId ()

//============================================================================
// <End of cdsuthread.cpp>
//============================================================================
