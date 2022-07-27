#ifndef CDSU_TASK_H_
#define CDSU_TASK_H_

//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : CdSuTask.h
// Description	: Definitions for the CdSuTask Scheduler.
// Author       : 
// Created On   : April 3rd, 2002 
// Last Modified: June 18th, 2002
//============================================================================

#ifdef _UTIL_STAND_ALONE
#include "cdsuutiltypes.h"
#endif

typedef Uint16 CdSuTaskId;
enum 
{
	CDSU_TID_INVALID = 0xff 
};	

//----------------------------------------------------------------------------
// Structure    : CdTaskMessage Message
// Description  : This Msg is sent by various modules in the stack internally.
//----------------------------------------------------------------------------

struct CdTaskMessage : CdMessage
{
	void initMsg (CdModuleId dest,
			CdModuleId src,
			Uint16 opcode,
			Uint16 len, 
			CdSuTaskId taskId)
	{
		((CdMessage*) this)->initMsg (dest, src, opcode, len);
		subMid = taskId;
	}
	
        CdSuTaskId subMid;

}; // CdTaskMessage

//----------------------------------------------------------------------------
// class	: CdSuTask
// Description	: Definitions for class CdSuTask.
// Notes	: 
//----------------------------------------------------------------------------

class CdSuTask
{
public:

	CdSuTask (){};
	virtual ~CdSuTask (){};
	
	void setTaskId (CdSuTaskId tid) { taskId = tid;};
	CdSuTaskId getTaskId (){ return (taskId);};
	
	virtual void handleMsg (CdTaskMessage*) = 0;

private:
	CdSuTaskId taskId;

}; // class CdSuTask

//============================================================================
// <End of Task.h>
//============================================================================
#endif
