#ifndef __BUK_UI_MENTOR_MSGS_H__
#define __BUK_UI_MENTOR_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukUiMentorMsgs.h
// Description	: Declarations for all messages between UI and MENTOR
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

#include "cdsuutiltypes.h"

//============================================================================
//Message Opcodes
//============================================================================

enum BukUiMentorMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Ui to Mentor
	//--------------------------------------------------------------------

	// Request Messages
	BUK_UI_MT_REG = 0,		// registration request
	BUK_UI_MT_RPRTRQST,		// request for report from trainee
	BUK_UI_MT_EXIT,			// exit
};

//============================================================================
//Message Structures
//============================================================================

typedef struct BukUiMtMsg
{
	int opcode;
}BukUiMtMsg;

typedef struct BukUiMtReg : public BukUiMtMsg
{
	void initMsg ();
}BukUiMtReg;

typedef struct BukUiMtRprtRqst : public BukUiMtMsg
{
	int mId;
	void initMsg (int mId);
}BukUiMtRprtRqst;

typedef struct BukUiMtExit : public BukUiMtMsg
{
	void initMsg ();
}BukUiMtExit;

#endif

//============================================================================
// end of bukUiMentorMsgs.h
//============================================================================
