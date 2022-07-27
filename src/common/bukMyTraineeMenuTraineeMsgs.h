#ifndef __BUK_MYTRAINEEMENU_TRAINEE_MSGS_H__
#define __BUK_MYTRAINEEMENU_TRAINEE_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukMyTraineeMenuTraineeMsgs.h
// Description	: Declarations for all messages between MyTraineeMenu and TRAINEE
// Ref Docs	    :
// Author	    : rohitm
// Contributors	:
// Created On	: 22.07.2022
// Last Modified:
//============================================================================

#include "cdsuutiltypes.h"

//============================================================================
//Message Opcodes
//============================================================================

enum BukMyTraineeMenuTrMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from MyTraineeMenu to Trainee
	//--------------------------------------------------------------------

	// Request Messages
//	BUK_MYTRAINEEMENU_TR_REG = 0,		// registration request
//	BUK_MYTRAINEEMENU_TR_GDNCRQST,		// guidance request to request
//	BUK_MYTRAINEEMENU_TR_EXIT,			// exit
	BUK_MYTRAINEEMENU_TR_REQ			//request from MyTraineeMenu
};

enum BukTrMyTraineeMenuMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Trainee to MyTraineeMenu
	//--------------------------------------------------------------------

	// Reeponse Messages
	BUK_TR_MYTRAINEEMENU_RES = 0,		// response message from Trainee
};


//============================================================================
//Message Structures
//============================================================================

typedef struct BukMyTraineeMenuTrMsgs : public CdMessage
{
	Uint16 reqId, resId;
}BukMyTraineeMenuTrMsgs;

//typedef struct BukMyTraineeMenuTrReg : public BukMyTraineeMenuTrMsgs
//{
//	void initMsg (const CdModuleId& dst, const CdModuleId& src);
//}BukMyTraineeMenuTrReg;
//
//typedef struct BukMyTraineeMenuTrGdncRqst : public BukMyTraineeMenuTrMsgs
//{
//	Uint16 tId;
//	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
//}BukMyTraineeMenuTrGdncRqst;
//
//typedef struct BukMyTraineeMenuTrExit : public BukMyTraineeMenuTrMsgs
//{
//	void initMsg (const CdModuleId& dst, const CdModuleId& src);
//}BukMyTraineeMenuTrExit;

typedef struct BukMyTraineeMenuTrReq : public BukMyTraineeMenuTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 reqId);
}BukMyTraineeMenuTrReq;

typedef struct BukTrMyTraineeMenuRes : public BukMyTraineeMenuTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 resId);
}BukTrMyTraineeMenuRes;


#endif

//============================================================================
// end of bukMyTraineeMenuTraineeMsgs.h
//============================================================================
