#ifndef __BSD_ADMIN_MENTOR_MSGS_H__
#define __BSD_ADMIN_MENTOR_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	:bsdAdminMentorMsgs.h
// Description	:Declarations for all messages between ADMIN and MENTOR
// Ref Docs	:
// Author	:
// Contributors	:
// Created On	:Wed Dec  5 12:03:18 IST 2012
// Last Modified:
//============================================================================

#include "cdsuutiltypes.h"

//============================================================================
//Message Opcodes
//============================================================================

enum BsdAdminMentorMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Admin to Mentor
	//--------------------------------------------------------------------

	// Request Messages

	BSD_AD_MT_WRK = 0
};

typedef struct BsdAdMtWrk : public CdMessage
{
	Uint16 mId;

	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BsdAdMtWrk;

