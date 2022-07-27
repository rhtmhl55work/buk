//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename 	: bukAdminMentorMsgs.cpp
// Description	: Definitions for all messages between ADMIN and MENTOR
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================
#include "bukAdminMentorMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukAdMtRegAck
// Description  :
//------------------------------------------------------------------------

void BukAdMtRegAck::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_AD_MT_REGACK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdMtRegAck);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukAdMtRegNack
// Description  :
//------------------------------------------------------------------------

void BukAdMtRegNack::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_AD_MT_REGNACK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdMtRegNack);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukAdMtWrk
// Description  :
//------------------------------------------------------------------------

void BukAdMtWrk::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_AD_MT_WRK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdMtWrk);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukAdMtTid
// Description  :
//------------------------------------------------------------------------

void BukAdMtTid::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid, Uint16 tid)
{
	msgHdr.opcode = BUK_AD_MT_TRID;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdMtTid);
	this->mId = mid;
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukAdMtTrCmplt
// Description  :
//------------------------------------------------------------------------

void BukAdMtTrCmplt::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_AD_MT_TRCMPLT;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukAdMtTrCmplt);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukMtAdReg
// Description  :
//------------------------------------------------------------------------

void BukMtAdReg::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_MT_AD_MTID;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtAdReg);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukMtAdTrLost
// Description  :
//------------------------------------------------------------------------

void BukMtAdTrLost::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_MT_AD_TRLOST;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtAdTrLost);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukMtAdTrCmpltAck
// Description  :
//------------------------------------------------------------------------

void BukMtAdTrCmpltAck::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_MT_AD_TRCMPLTACK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtAdTrCmpltAck);
	this->mId = mid;
}

//============================================================================
// end of bukAdminMentorMsgs.cpp
//============================================================================
