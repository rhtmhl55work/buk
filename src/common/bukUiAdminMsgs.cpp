//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukUiAdminMsgs.cpp
// Description	: Definitions for all messages between UI and ADMIN
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

//============================================================================
//Include files
//============================================================================

#include "bukUiAdminMsgs.h"
#include <stdio.h>

//------------------------------------------------------------------------
// Structure    : BukUiAdWrk
// Description  :
//------------------------------------------------------------------------

void BukUiAdWrk::initMsg (int mid)
{
	opcode = BUK_UI_AD_WRKTOMT;
	this->mId = mid;
}

//------------------------------------------------------------------------
// Structure    : BukUiAdStrTrng
// Description  :
//------------------------------------------------------------------------

void BukUiAdStrTrng::initMsg ()
{
	opcode = BUK_UI_AD_STRTRNG;
}

//------------------------------------------------------------------------
// Structure    : BukUiAdStpTrng
// Description  :
//------------------------------------------------------------------------

void BukUiAdStpTrng::initMsg ()
{
	opcode = BUK_UI_AD_STPTRNG;
}

//------------------------------------------------------------------------
// Structure    : BukUiAdExit
// Description  :
//------------------------------------------------------------------------

void BukUiAdExit::initMsg ()
{
	opcode = BUK_UI_AD_EXIT;
}

//============================================================================
// end of bukUiAdminMsgs.cpp
//============================================================================
