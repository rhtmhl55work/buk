#ifndef __BUK_ADMIN_TRAINEE_MSGS_H__
#define __BUK_ADMIN_TRAINEE_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukAdminTraineeMsgs.h
// Description	: Declarations for all messages between ADMIN and TRAINEE
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

enum BukAdminTraineeMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Admin to Trainee
	//--------------------------------------------------------------------

	// Request and response Messages
	BUK_AD_TR_REGACK = 0,	// trainee registration successful
	BUK_AD_TR_REGNACK,		// trainee registration failed
	BUK_AD_TR_MTID,			// start training
	BUK_AD_TR_TRCMPLT		// stop training
};

enum BukTraineeAdminMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Trainee to Admin
	//--------------------------------------------------------------------

	// Request and response Messages
	BUK_TR_AD_TRID = 0,		// registration request
	BUK_TR_AD_TRCMPLTACK	// training complete msg ack
};

//============================================================================
//Message Structures
//============================================================================

typedef struct BukAdTrMsgs : public CdMessage
{
	Uint16 tId;
}BukAdTrMsgs;

//------------------------------------------------------------------------
// Structure    : BukAdTrRegAck
// Description  :
//------------------------------------------------------------------------

typedef struct BukAdTrRegAck : public BukAdTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukAdTrRegAck;

//------------------------------------------------------------------------
// Structure    : BukAdTrRegNack
// Description  :
//------------------------------------------------------------------------

typedef struct BukAdTrRegNack : public BukAdTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukAdTrRegNack;

//------------------------------------------------------------------------
// Structure    : BukAdTrMid
// Description  :
//------------------------------------------------------------------------

typedef struct BukAdTrMid : public BukAdTrMsgs
{
	Uint16 mId;

	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mid, Uint16 tId);
}BukAdTrMid;

//------------------------------------------------------------------------
// Structure    : BukAdTrTrCmplt
// Description  :
//------------------------------------------------------------------------

typedef struct BukAdTrTrCmplt : public BukAdTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukAdTrTrCmplt;

//------------------------------------------------------------------------
// Structure    : BukTrAdReg
// Description  :
//------------------------------------------------------------------------

typedef struct BukTrAdReg : public BukAdTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukTrAdReg;

//------------------------------------------------------------------------
// Structure    : BukTrAdTrCmpltAck
// Description  :
//------------------------------------------------------------------------

typedef struct BukTrAdTrCmpltAck : public BukAdTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukTrAdTrcmpltAck;

#endif

//============================================================================
// end of bukAdminTraineeMsgs.h
//============================================================================
