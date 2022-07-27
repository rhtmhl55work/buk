//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukMentorTraineeMsgs.cpp
// Description	: Definitions for all messages between MENTOR and TRAINEE
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bukMentorTraineeMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukMtTrIntro
// Description  :
//------------------------------------------------------------------------

void BukMtTrIntro::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid, Uint16 tid)
{
	msgHdr.opcode = BUK_MT_TR_INTRO;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtTrIntro);
	this->mId = mid;
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukMtTrRprtRqst
// Description  :
//------------------------------------------------------------------------

void BukMtTrRprtRqst::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_MT_TR_RPRTRQST;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtTrRprtRqst);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukMtTrGdncRply
// Description  :
//------------------------------------------------------------------------

void BukMtTrGdncRply::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_MT_TR_GDNCRPLY;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtTrGdncRply);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukMtTrGdncRjct
// Description  :
//------------------------------------------------------------------------

void BukMtTrGdncRjct::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tid)
{
	msgHdr.opcode = BUK_MT_TR_GDNCRJCT;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukMtTrGdncRjct);
	this->tId = tid;
}

//------------------------------------------------------------------------
// Structure    : BukTrMtIntroRply
// Description  :
//------------------------------------------------------------------------

void BukTrMtIntroRply::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_TR_MT_INTRORPLY;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrMtIntroRply);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukTrMtPrdcRprt
// Description  :
//------------------------------------------------------------------------

void BukTrMtPrdcRprt::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_TR_MT_PRDCRPRT;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrMtPrdcRprt);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukTrMtRprt
// Description  :
//------------------------------------------------------------------------

void BukTrMtRprt::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_TR_MT_RPRT;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrMtRprt);
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukTrMtGdncRqst
// Description  :
//------------------------------------------------------------------------

void BukTrMtGdncRqst::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BUK_TR_MT_GDNCRQST;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BukTrMtGdncRqst);
	this->mId = mid;
}

//============================================================================
// end of bukMentorTraineeMsgs.cpp
//============================================================================
