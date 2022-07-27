#ifndef __BUK_UI_TRAINEE_MSGS_H__
#define __BUK_UI_TRAINEE_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukUiTraineeMsgs.h
// Description	: Declarations for all messages between UI and TRAINEE
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

enum BukUiTraineeMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Ui to Trainee
	//--------------------------------------------------------------------

	// Request Messages
	BUK_UI_TR_REG = 0,		// registration request
	BUK_UI_TR_GDNCRQST,		// guidance request to request
	BUK_UI_TR_EXIT,			// exit
};

//============================================================================
//Message Structures
//============================================================================

typedef struct BukUiTrMsg
{
	int opcode;
}BukUiTrMsg;

typedef struct BukUiTrReg : public BukUiTrMsg
{
	void initMsg ();
}BukUiTrReg;

typedef struct BukUiTrGdncRqst : public BukUiTrMsg
{
	int tId;
	void initMsg (int tId);
}BukUiTrGdncRqst;

typedef struct BukUiTrExit : public BukUiTrMsg
{
	void initMsg ();
}BukUiTrExit;

#endif

//============================================================================
// end of bukUiTraineeMsgs.h
//============================================================================
