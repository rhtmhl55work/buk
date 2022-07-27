//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsukeyallocator.cpp
// Description  : Key Id Determination functions.
// Author       : 
// Contributor  : 
// Created On   : 08 January ,2003 
// Last Modified: 
// Created By   : Indu Mishra
// Notes        : NIL
//============================================================================
#include "cdsukeyallocator.h"

//----------------------------------------------------------------------------
// Function     : CdSuKeyAllocator ()
// Description  : Constructor.
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Name		: CdSuKeyAllocator::CdSuKeyAllocator ()
// Description  : The Constructor of the class. Initializes the private
// 		  variables of the class.
// Inputs       : No input values.
// Return Value : No return parameters.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

CdSuKeyAllocator::CdSuKeyAllocator()
{
	keyId = 0;
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

}// CdSuKeyAllocator()

//---------------------------------------------------------------------------
// Name		: CdSuKeyAllocator::~CdSuKeyAllocator ()
// Description  : The Constructor of the class. Initializes the private
// 		  variables of the class.
// Inputs       : No input values.
// Return Value : No return parameters.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

CdSuKeyAllocator::~CdSuKeyAllocator()
{
	//CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
}// ~CdSuKeyAllocator()

//---------------------------------------------------------------------------
// Name		: CdSuKeyAllocator::KeyGenerator ()
// Description  : This function is used to generate a unique keyId and allocated
//		  is incremented by 1.so that next time new id is generated.when
//                allocated becomes equal to MAX_TIMER then allocated is set to 0		
// Inputs       : No input values.
// Return Value : No return parameters.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

Uint32 CdSuKeyAllocator::KeyGenerator()
{
	keyId++;
	if (keyId == CDSU_TMR_MAX_KEY)
		keyId = 0;

	return (keyId);

}// KeyGenerator()

//============================================================================
// End of <cdsukeyallocator.cpp>
//============================================================================

