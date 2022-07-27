//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukUiMentorMsgs.cpp
// Description	: Definitions for all messages between UI and MENTOR
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bukUiMentorMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukUiMtReg
// Description  :
//------------------------------------------------------------------------

void BukUiMtReg::initMsg ()
{
	opcode = BUK_UI_MT_REG;
}

//------------------------------------------------------------------------
// Structure    : BukUiMtRprtRqst
// Description  :
//------------------------------------------------------------------------

void BukUiMtRprtRqst::initMsg (int mid)
{
	mId = mid;
	opcode = BUK_UI_MT_RPRTRQST;
}

//------------------------------------------------------------------------
// Structure    : BukUiMtExit
// Description  :
//------------------------------------------------------------------------

void BukUiMtExit::initMsg ()
{
	opcode = BUK_UI_MT_EXIT;
}

//============================================================================
// end of bukUiMentorMsgs.cpp
//============================================================================
