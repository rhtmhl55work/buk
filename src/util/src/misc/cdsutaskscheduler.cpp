//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsutaskscheduler.cpp
// Description	: CPP file for class CdSuTaskScheduler.
// Author       : 
// Created On   :  
// Last Modified: 1. June 19th, 2003
// 		: 2. Mon Aug 16 10:45:19 IST 2004
// 			By : Vijaya Kumar
// 			removeTask Function Is Modified To Fix Bugs bs/32 
//		: 3.  Wed Apr 19 19:15:05 IST 2006
//			By : Vijaya Kumar
// 			Code has been changed inorder to port over Fedora Core 4
//============================================================================

//----------------------------------------------------------------------------
// Include Files.
//----------------------------------------------------------------------------

#include <stdio.h>
#include "cdsutrace.h"
#include "cdsutaskscheduler.h"
#include "cdsumdapi.h"

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::CdSuTaskScheduler ()
// Description  : Constructor of CdSuTaskSchedkuler class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuTaskScheduler::CdSuTaskScheduler (CdSuTsQueue<CdTaskMessage*>* msgQ, 
			Uint32 maxTasks)
{
	// CDSU_TRACE1 (4, "CdSuTaskScheduler::CdSuTaskScheduler\n");

	if (maxTasks == CDSU_TID_INVALID)
	{
		maxTasks = DEFAULT_NO_OF_TASKS;
	}

	msgQueueBased = true;
	msgQueue = msgQ;

	maxNoOfTasks = maxTasks;
	taskList = new CdSuTask* [maxNoOfTasks];

	reset ();

} // CdSuTaskScheduler::CdSuTaskScheduler ()

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::CdSuTaskScheduler ()
// Description  : Constructor of CdSuTaskSchedkuler class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuTaskScheduler::CdSuTaskScheduler (const CdModuleId selfMid,
			Uint32 maxTasks)
{
	// CDSU_TRACE1 (4, "CdSuTaskScheduler::CdSuTaskScheduler\n");

	if (maxTasks == CDSU_TID_INVALID)
	{
		maxTasks = DEFAULT_NO_OF_TASKS;
	}

	selfModuleId = selfMid;

	msgQueueBased = false;
	msgQueue = NULL;

	maxNoOfTasks = maxTasks;

	taskList = new CdSuTask* [maxNoOfTasks];

	reset ();

} // CdSuTaskScheduler::CdSuTaskScheduler ()


//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::~CdSuTaskScheduler ()
// Description  : Destructor of CdSuTaskScheduler class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    : Deletion of Tasks is  not done here.
//---------------------------------------------------------------------------

CdSuTaskScheduler::~CdSuTaskScheduler ()
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	delete [] taskList;

} // CdSuTaskScheduler::~CdSuTaskScheduler ()

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::reset ()
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuTaskScheduler::reset ()
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);

	for (Uint32 i = 0; i < maxNoOfTasks; ++i)
	{
		taskList [i] = (CdSuTask*) NULL;
	}

} // CdSuTaskScheduler::reset ()

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::dispatcher ()
// Description  : The task scheduler listens on its queue and dispatches the
// 		  received message to the appropriate Task based on the
// 		  taskId.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuTaskScheduler::dispatcher ()
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);

	while (1)
	{
		CdTaskMessage* taskMsg = NULL; 
		bool retVal = false;
		
		// Depending on the TaskSchedule type, recv msg accordingly.
		if (msgQueueBased == true)
		{
			retVal = msgQueue->popBlocking (taskMsg);
		} else
		{
			retVal = cdSuMdRecvMsg (((CdMessage**) &taskMsg), selfModuleId);
		}

		// If a proper message is received, find a task and post it to
		// the task.
		if (retVal != false)
		{
			Uint32 destId = taskMsg->subMid;

			CDSU_TRACE1 (5, "CdSuTask Scheduler has got some Msg to despatch () for DestId %d\n", destId);

			if (destId < maxNoOfTasks)
			{
				if (taskList [destId] != (CdSuTask*) NULL)
				{
					// If able to despatch msg then it
					// is the functionality of the
					// respective Task's handleMsg
					// function to delete the msg.
					taskList [destId]->handleMsg (taskMsg);
				}
				else
				{
					CDSU_TRACE (1, "Invalid destId\n");
					// If not able to despatch msg delete it.
					delete taskMsg;
				}
			} else
			{
				delete taskMsg;
			}
		} // if (retVal != false)
	} // while (1)

} // CdSuTaskScheduler::dispatcher ()

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::addTask ()
// Description  : It adds tasks to the TaskList.
// Inputs       : Task*.
// Return Value : Returns the taskId of the Task.
// Side Effects : 
// Algorithm    : A Task cannot be added if 
//			*) The Task to be added is a NULL task.
//			*) There are no free slots available to add the task 
//			   i.e. we have to increase the maxNoOfTasks to
//			   accomodate the task.
//---------------------------------------------------------------------------

CdSuTaskId CdSuTaskScheduler::addTask (CdSuTask* task)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	if (task == NULL)
		return (CDSU_TID_INVALID);

	Uint32 i = 0;
	while (i < maxNoOfTasks)
	{
		if (taskList [i] == (CdSuTask*) NULL)
		{
			taskList [i] = task;
			task->setTaskId (CdSuTaskId (i));
			return (CdSuTaskId (i));
		}
		++i;
	}
	return (CDSU_TID_INVALID);

} // CdSuTaskScheduler::addTask ()

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::setTask ()
// Description  : Set the taskId.
// Inputs       : CdSuTaskId, Task*.
// Return Value : void.
// Side Effects : 
// Algorithm    : Setting the Task is not allowed if :
//			*) TaskId is greater than or equal to max no of	tasks.
//			*) There is already some elem present for the TaskId.
//			*) The TaskId is invalid.
//			*) The task is NULL.
//---------------------------------------------------------------------------

bool CdSuTaskScheduler::setTask (CdSuTaskId taskId, CdSuTask* task)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);

	if ((taskId >= maxNoOfTasks) || (taskList [taskId] != NULL) ||
				(task == NULL))
		return false;

	taskList [taskId] = task;
	return true;

} // CdSuTaskScheduler::setTask ()

//---------------------------------------------------------------------------
// Name		: CdSuTaskScheduler::removeTask ()
// Description  : Removes the task with the given TaskId from the list.
// Inputs       : CdSuTaskId.
// Return Value : void.
// Side Effects : 
// Algorithm    : Removal of a task is not allowed if :
//			*) TaskId is greater than or equal to max no of tasks.
//			*) There is no Task for that taskId.
//---------------------------------------------------------------------------

bool CdSuTaskScheduler::removeTask (CdSuTaskId tid)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);

	if (tid >= maxNoOfTasks || (taskList [tid] == (CdSuTask*) NULL))
		return false;
	
	taskList [tid] = (CdSuTask*) NULL;
	return true;

} // CdSuTaskScheduler::removeTask ()

//============================================================================
// <End of cdsutaskscheduler.cpp>
//============================================================================
