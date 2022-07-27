#ifndef __CDSU_THREAD_H_
#define __CDSU_THREAD_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsuthread.h
// Description  : Thread class definition
// Author       : Titty Thomas
// Created On   : Tue Feb 18 14:03:05 IST 2003
// Last Modified: 
//============================================================================

#include <pthread.h>

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
const pthread_t CDSU_THREAD_ID_INVALID = 0;

//----------------------------------------------------------------------------
// Enumerations and Typedefs.
//----------------------------------------------------------------------------
enum 
{
	CDSU_THREAD_NUM_THREADS_DEF = 20
};


//----------------------------------------------------------------------------
// Class	: CdSuThread 
// Description	: Defines a thread class which provides means of controlling a
//		  separate thread of control in which the scheduling mechanism
//		  and priority may be specified. 
//		  Each instance of the class supports one thread of execution.
//		  Creating multiple threads required creating muliple CdSuThread
//		  objects.
//----------------------------------------------------------------------------

class CdSuThread
{
public:
	enum Flags  
	{
		CDSU_THREAD_SCHED_MASK = 0x0000000F,
		CDSU_THREAD_SCHED_DEF = 0x00000001,
		CDSU_THREAD_SCHED_FIFO = 0x00000002,
		CDSU_THREAD_SCHED_RR = 0x00000004,
	};

	enum Priority
	{
		CDSU_THREAD_PRIORITY_DEF = 0
	};

	enum StackSize
	{
		CDSU_THREAD_STACK_SIZE_DEF = 65536	// 2^16
	};

	enum RetVal
	{
		CDSU_ERROR = -1,
		CDSU_SUCCESS = 1
	};

	CdSuThread ();

	virtual ~CdSuThread ();

	int spawn (void *(*startFunc)(void *), 
			void *startArgs = 0, 
			unsigned long flags = 0, 
			unsigned long priority = CDSU_THREAD_PRIORITY_DEF,
			unsigned long stack_size = CDSU_THREAD_STACK_SIZE_DEF); 
	
	int join (void **status = 0);
	int getPriority () const;
	int setPriority (int priority);
	const pthread_t getId () const;
	const pthread_attr_t* getAttributes () const;
	void exit ();
	static const pthread_t selfId ();

private:
	pthread_t threadId;
	pthread_attr_t threadAttr;
};

//============================================================================
// <End of cdsuthread.h>
//============================================================================
#endif	// THREAD_H_
