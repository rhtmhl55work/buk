#ifndef __BUK_MENTOR_TRAINEE_MSGS_H__
#define __BUK_MENTOR_TRAINEE_MSGS_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukMentorTraineeMsgs.h
// Description	: Declarations for all messages between MENTOR and TRAINEE
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

enum BukMentorTraineeMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Mentor to Trainee
	//--------------------------------------------------------------------

	// Request and response Messages
	BUK_MT_TR_INTRO = 0,		// introduction msg
	BUK_MT_TR_RPRTRQST,		    // report request msg
	BUK_MT_TR_GDNCRPLY,		    // guidance reply msg
	BUK_MT_TR_GDNCRJCT,		    // guidance reject msg
};

enum BukTraineeMentorMsgOpcode
{
	//--------------------------------------------------------------------
	// Messages from Trainee to Mentor
	//--------------------------------------------------------------------

	// Request and response Messages
	BUK_TR_MT_INTRORPLY = 0,    // introduction msg reply
	BUK_TR_MT_PRDCRPRT,		    // periodic report
	BUK_TR_MT_RPRT,			    // report
	BUK_TR_MT_GDNCRQST		    // guidance request
};

//============================================================================
//Message Structures
//============================================================================

typedef struct BukMtTrMsgs : public CdMessage
{
	Uint16 tId;
}BukMtTrMsgs;

typedef struct BukTrMtMsgs : public CdMessage
{
	Uint16 mId;
}BukTrMtMsgs;

//------------------------------------------------------------------------
// Structure    : BukMtTrIntro
// Description  :
//------------------------------------------------------------------------

typedef struct BukMtTrIntro : public BukMtTrMsgs
{
	Uint16 mId;

	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId, Uint16 tId);
}BukMtTrIntro;

//------------------------------------------------------------------------
// Structure    : BukMtTrRprtRqst
// Description  :
//------------------------------------------------------------------------

typedef struct BukMtTrRprtRqst : public BukMtTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukMtTrRprtRqst;

//------------------------------------------------------------------------
// Structure    : BukMtTrGdncRply
// Description  :
//------------------------------------------------------------------------

typedef struct BukMtTrGdncRply : public BukMtTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukMtTrGdncRply;

//------------------------------------------------------------------------
// Structure    : BukMtTrGdncRjct
// Description  :
//------------------------------------------------------------------------

typedef struct BukMtTrGdncRjct : public BukMtTrMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 tId);
}BukMtTrGdncRjct;

//------------------------------------------------------------------------
// Structure    : BukTrMtIntroRply
// Description  :
//------------------------------------------------------------------------

typedef struct BukTrMtIntroRply : public BukTrMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukTrMtIntroRply;

//------------------------------------------------------------------------
// Structure    : BukTrMtPrdcRprt
// Description  :
//------------------------------------------------------------------------

typedef struct BukTrMtPrdcRprt : public BukTrMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukTrMtPrdcRprt;

//------------------------------------------------------------------------
// Structure    : BukTrMtRprt
// Description  :
//------------------------------------------------------------------------

typedef struct BukTrMtRprt : public BukTrMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukTrMtRprt;

//------------------------------------------------------------------------
// Structure    : BukTrMtGdncRqst
// Description  :
//------------------------------------------------------------------------

typedef struct BukTrMtGdncRqst : public BukTrMtMsgs
{
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint16 mId);
}BukTrMtGdncRqst;

#endif

//============================================================================
// end of bukMentorTraineeMsgs.h
//============================================================================
