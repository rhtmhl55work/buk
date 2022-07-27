#ifndef __BUK_ADMIN_MENTOR_MSGS_H__
#define __BUK_ADMIN_MENTOR_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename 	: bukAdminMentorMsgs.h
// Description	: Declarations for all messages between ADMIN and MENTOR
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

enum BukAdminMentorMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Admin to Mentor
	//--------------------------------------------------------------------

	// Request and response Messages
	BUK_AD_MT_REGACK = 0,	// mentor registration successful
	BUK_AD_MT_REGNACK,		// mentor registration failed
	BUK_AD_MT_WRK,			// work request to mentor
	BUK_AD_MT_TRID,			// start training
	BUK_AD_MT_TRCMPLT		// stop training
};

enum BukMentorAdminMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Mentor to Admin
	//--------------------------------------------------------------------

	// Request and response Messages
	BUK_MT_AD_MTID = 0,		// registration request
	BUK_MT_AD_TRLOST,		// trainee contact lost report
	BUK_MT_AD_TRCMPLTACK	// training complete msg ack
};

//============================================================================
//Message Structures
//============================================================================

typedef struct BukAdMtMsgs : public CdMessage
{
	Uint16 mId;
}BukAdMtMsgs;

// Registration Successful acknowledgement
typedef struct BukAdMtRegAck : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukAdMtRegAck;

//Registration failed msg
typedef struct BukAdMtRegNack : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukAdMtRegNack;

// Allocate Work to mentor
typedef struct BukAdMtWrk : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukAdMtWrk;

// Start training : trainee id to mentor
typedef struct BukAdMtTid : public BukAdMtMsgs
{
	Uint16 tId;

	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId, Uint16 tId);
}BukAdMtTid;

// Stop training
typedef struct BukAdMtTrCmplt : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukAdMtTrCmplt;

// Registration request : mentor id
typedef struct BukMtAdReg : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukMtAdReg;

// Trainee Contact Lost report
typedef struct BukMtAdTrLost : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukMtAdTrLost;

// Training complete msg ack
typedef struct BukMtAdTrCmpltAck : public BukAdMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukMtAdTrCmpltAck;

#endif

//============================================================================
// end of bukAdminMentorMsgs.h
//============================================================================
