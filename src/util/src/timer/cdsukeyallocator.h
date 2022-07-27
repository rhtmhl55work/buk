#ifndef __KeyAllocator_H_
#define __KeyAllocator_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsukeyallocator.h
// Description  : keyId determination and allocation functions .
// Ref Docs     : 
// Author       : Indu Mishra
// Contributor  : 
// Created On   : January 08,2003
// Modified On  : 
// Modified By  : 
// Notes        : NIL
//============================================================================

#include <stdio.h>
#include <string.h>
#include "cdsutrace.h"

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
typedef unsigned int Uint32;

//---------------------------------------------------------------------------
// Name		: enum TIMER_FIELDS
// Description	: used to initialise variables required for key determination
// Notes	:
//---------------------------------------------------------------------------

enum CdSuTimerFields 
{
	CDSU_TMR_MAX_KEY = 0xffffffff
};


//---------------------------------------------------------------------------
// Name		: CdSuKeyAllocator.
// Description	: Class definitions for implementing key Determination function.
// Notes	:
//---------------------------------------------------------------------------

class CdSuKeyAllocator
{
public:
	CdSuKeyAllocator();		//Constructor
	~CdSuKeyAllocator();		//Destructor
	Uint32 KeyGenerator ();		//function to allocate key
	
private:
	Uint32 keyId;

};// class CdSuKeyAllocator



#endif
//-----------------------------------------------------------------------------
// End of <cdsukeyallocator.h>
//-----------------------------------------------------------------------------
