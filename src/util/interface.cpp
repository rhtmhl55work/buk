//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	:bsdAdminMentorMsgs.cpp
// Description	:Definitions for all messages between ADMIN and MENTOR
// Ref Docs	:
// Author	:
// Contributors	:
// Created On	:Wed Dec  5 12:46:35 IST 2012
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bsdAdminMentorMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BsdAdMtWrk.
// Description  : 
//------------------------------------------------------------------------

void BsdAdMtWrk::initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid)
{
	msgHdr.opcode = BSD_AD_MT_WRK;
	msgHdr.dst = dst;
	msgHdr.src = src;
	msgHdr.msgLen = sizeof(BsdAdMtWrk);
	this->mId = mid;
}

