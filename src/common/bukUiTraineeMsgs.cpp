//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukUiTraineeMsgs.cpp
// Description	: Definitions for all messages between UI and TRAINEE
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bukUiTraineeMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukUiTrReg
// Description  :
//------------------------------------------------------------------------

void BukUiTrReg::initMsg ()
{
	opcode = BUK_UI_TR_REG;
}

//------------------------------------------------------------------------
// Structure    : BukUiTrGdncRqst
// Description  :
//------------------------------------------------------------------------

void BukUiTrGdncRqst::initMsg (int tid)
{
	tId = tid;
	opcode = BUK_UI_TR_GDNCRQST;
}

//------------------------------------------------------------------------
// Structure    : BukUiTrExit
// Description  :
//------------------------------------------------------------------------

void BukUiTrExit::initMsg ()
{
	opcode = BUK_UI_TR_EXIT;
}


//============================================================================
// end of bukUiTraineeMsgs.cpp
//============================================================================
