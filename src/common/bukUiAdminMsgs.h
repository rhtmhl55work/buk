#ifndef __BUK_UI_ADMIN_MSGS_H__
#define __BUK_UI_ADMIN_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukUiAdminMsgs.h
// Description	: Declarations for all messages between UI and ADMIN
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Message Opcodes
//============================================================================

enum BukUiAdminMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Ui to Admin
	//--------------------------------------------------------------------

	// Request Messages
	BUK_UI_AD_STRTRNG = 0,	// start training
	BUK_UI_AD_STPTRNG,		// stop training
	BUK_UI_AD_WRKTOMT,		// allocate work to mentor
	BUK_UI_AD_EXIT,			// exit
};

//============================================================================
//Message Structures
//============================================================================

typedef struct BukUiAdMsg
{
	int opcode;
}BukUiAdMsg;

typedef struct BukUiAdWrk : public BukUiAdMsg
{
	int mId;
	void initMsg (int mId);
}BukUiAdWrk;

typedef struct BukUiAdStrTrng : public BukUiAdMsg
{
	void initMsg ();
}BukUiAdStrTrng;

typedef struct BukUiAdStpTrng : public BukUiAdMsg
{
	void initMsg ();
}BukUiAdStpTrng;

typedef struct BukUiAdExit : public BukUiAdMsg
{
	void initMsg ();
}BukUiAdExit;

#endif

//============================================================================
// end of bukUiAdminMsgs.h
//============================================================================
