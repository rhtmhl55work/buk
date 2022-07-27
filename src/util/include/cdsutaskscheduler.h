//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsutaskscheduler.h
// Description	: Definitions for class CdSuTaskScheduler.
// Author       : 
// Created On   : April 3rd, 2002 
// Last Modified: June 18th, 2003
//============================================================================

#ifndef CDSU_TASK_SCHEDULER_H
#define CDSU_TASK_SCHEDULER_H

#include "cdsuutiltypes.h"
#include "cdsutsqueue.h"
#include "cdsutask.h"

//----------------------------------------------------------------------------
// Class	: CdSuTaskScheduler.
// Description	: Definitions for  class CdSuTaskScheduler.
//----------------------------------------------------------------------------

class CdSuTaskScheduler
{
public:
	
	CdSuTaskScheduler (CdSuTsQueue<CdTaskMessage*>* msgQ, 
				Uint32 maxTasks = DEFAULT_NO_OF_TASKS);
	CdSuTaskScheduler (const CdModuleId selfMid, 
			Uint32 maxTasks = DEFAULT_NO_OF_TASKS);
	~CdSuTaskScheduler ();

	//--------------------------------------------------------------------
	// API provided to the external world.
	//--------------------------------------------------------------------
	void reset ();
	void dispatcher ();
	CdSuTaskId addTask (CdSuTask* task);
	bool setTask(CdSuTaskId tid, CdSuTask* task);
	bool removeTask (CdSuTaskId tid);

private:
	//--------------------------------------------------------------------
	// Enumerations and typedefs.
	//--------------------------------------------------------------------
	enum 
	{
		DEFAULT_NO_OF_TASKS = 64 
	};

	//--------------------------------------------------------------------
	// Private Attributes.
	//--------------------------------------------------------------------

	bool msgQueueBased;
	CdSuTsQueue<CdTaskMessage*>* msgQueue;
	CdModuleId selfModuleId;
	CdSuTask** taskList;
	Uint32 maxNoOfTasks;

}; // class CdSuTaskScheduler 

//============================================================================
// <End of cdsutaskscheduler.h>
//============================================================================

#endif
