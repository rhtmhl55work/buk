//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukMyTraineeMenuTraineeMsgs.cpp
// Description	: Definitions for all messages between MyTraineeMenu and TRAINEE
// Ref Docs	    :
// Author	    : rohitm 
// Contributors	:
// Created On	: 22.07.2022
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bukMyTraineeMenuTraineeMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukMyTraineeMenuTrReg
// Description  :
//------------------------------------------------------------------------

//void BukMyTraineeMenuTrReg::initMsg (const CdModuleId& dst, const CdModuleId& src)
//{
//	msgHdr.opcode = BUK_MYTRAINEEMENU_TR_REG;
//	msgHdr.dst = dst;
//	msgHdr.src = src;
//	msgHdr.msgLen = sizeof(BukMyTraineeMenuTrReg);
//}
//
////------------------------------------------------------------------------
//// Structure    : BukMyTraineeMenuTrGdncRqst
//// Description  :
////------------------------------------------------------------------------
//
//void BukMyTraineeMenuTrGdncRqst::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId)
//{
//	msgHdr.opcode = BUK_MYTRAINEEMENU_TR_GDNCRQST;
//	msgHdr.dst = dst;
//	msgHdr.src = src;
//	msgHdr.msgLen = sizeof(BukMyTraineeMenuTrGdncRqst);
//	this->tId = tId;
//}
//
////------------------------------------------------------------------------
//// Structure    : BukMyTraineeMenuTrExit
//// Description  :
////------------------------------------------------------------------------
//
//void BukMyTraineeMenuTrExit::initMsg (const CdModuleId& dst, const CdModuleId& src)
//{
//	msgHdr.opcode = BUK_MYTRAINEEMENU_TR_EXIT;
//	msgHdr.dst = dst;
//	msgHdr.src = src;
//	msgHdr.msgLen = sizeof(BukMyTraineeMenuTrExit);
//}

//------------------------------------------------------------------------
// Structure    : BukMyTraineeMenuTrReq
// Description  :
//------------------------------------------------------------------------

void BukMyTraineeMenuTrReq::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 reqId)
{
	msgHdr.opcode = BUK_MYTRAINEEMENU_TR_REQ;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMyTraineeMenuTrReq);
	this->reqId = reqId;
}

//------------------------------------------------------------------------
// Structure    : BukTrMyTraineeMenuRes
// Description  :
//------------------------------------------------------------------------

void BukTrMyTraineeMenuRes::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 resId)
{
	msgHdr.opcode = BUK_TR_MYTRAINEEMENU_RES;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrMyTraineeMenuRes);
	this->resId = resId;
}

//============================================================================
// end of bukMyTraineeMenuTraineeMsgs.cpp
//============================================================================
