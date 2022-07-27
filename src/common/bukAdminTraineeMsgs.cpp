//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukAdminTraineeMsgs.cpp
// Description	: Definitions for all messages between ADMIN and TRAINEE
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bukAdminTraineeMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukAdTrRegAck
// Description  :
//------------------------------------------------------------------------

void BukAdTrRegAck::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_AD_TR_REGACK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdTrRegAck);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukAdTrRegNack
// Description  :
//------------------------------------------------------------------------

void BukAdTrRegNack::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_AD_TR_REGNACK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdTrRegNack);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukAdTrMid
// Description  :
//------------------------------------------------------------------------

void BukAdTrMid::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid, Uint16 tid)
{
	msgHdr.opcode = BUK_AD_TR_MTID;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdTrMid);
	this->mId = mid;
	this->tId = tid;

}

//------------------------------------------------------------------------
// Structure    : BukAdTrTrCmplt
// Description  :
//------------------------------------------------------------------------

void BukAdTrTrCmplt::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
//void BukAdTrTrCmplt::initMsg (const CdModuleId& dst, const CdModuleId& src)
{
	msgHdr.opcode = BUK_AD_TR_TRCMPLT;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdTrTrCmplt);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukTrAdReg
// Description  :
//------------------------------------------------------------------------

void BukTrAdReg::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_TR_AD_TRID;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrAdReg);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukTrAdTrCmpltAck
// Description  :
//------------------------------------------------------------------------

void BukTrAdTrCmpltAck::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_TR_AD_TRCMPLTACK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrAdTrCmpltAck);
	this->tId = tid;
}

//============================================================================
// end of bukAdminTraineeMsgs.cpp
//============================================================================
