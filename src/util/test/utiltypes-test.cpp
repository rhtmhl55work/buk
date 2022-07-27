//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: cdsuutiltypes-test.cpp
// Description	: Tests for cdsuutiltypes.
// Ref Docs	:
// Author	: Seema Jain
// Created On   : Wed Dec  3 17:50:24 IST 2003
// Last Modified: 
//==============================================================================

#include "cdsuutiltypes.h"
#include "cdsutrace.h"

int main ()
{
	CdSuTrace::setLevel (5);
	CDSU_TRACE (1, "Entered Main\n");

	CdModuleId selfModuleId;
	CdModuleId destModuleId;
	CdModuleId peerDestModId;
	
	//--------------------------------------------------------------------
	// Testing =, != and == operators of CdModuleId.
	//--------------------------------------------------------------------
	selfModuleId.hwUnitId.rackId = 1;
	selfModuleId.hwUnitId.unitId = 1;
	selfModuleId.hwUnitId.slotId = 1;
	selfModuleId.hwUnitId.procId = 1;
	selfModuleId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	selfModuleId.srvcHlrId.appSrvcHlrId.appType = 3 ;
	selfModuleId.srvcHlrId.appSrvcHlrId.appId = 4 ;
	selfModuleId.srvcHlrId.appSrvcHlrId.appModuleId = 7;

	destModuleId.hwUnitId.rackId = 2;
	destModuleId.hwUnitId.unitId = 2;
	destModuleId.hwUnitId.slotId = 2;
	destModuleId.hwUnitId.procId = 2;
	destModuleId.srvcHlrId.srvcType = SM_SERVICE_TYPE_PLATFORM_SERVICE;
	destModuleId.srvcHlrId.plfSrvcHlrId.pad8 = 3;
	destModuleId.srvcHlrId.plfSrvcHlrId.plfModuleId = 300 ;

	peerDestModId.hwUnitId.rackId = 2;
	peerDestModId.hwUnitId.unitId = 2;
	peerDestModId.hwUnitId.slotId = 2;
	peerDestModId.hwUnitId.procId = 2;
	peerDestModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_PLATFORM_SERVICE;
	peerDestModId.srvcHlrId.plfSrvcHlrId.pad8 = 3;
	peerDestModId.srvcHlrId.plfSrvcHlrId.plfModuleId = 300 ;
	
	
	if (destModuleId == peerDestModId)
	{
		CDSU_TRACE (1, "== Operator tested successfully\n");
	} else
	{
		CDSU_TRACE (1, "== Operator not tested successfully\n");
	}
	
	peerDestModId.srvcHlrId.plfSrvcHlrId.plfModuleId++;

	if (destModuleId == peerDestModId)
	{
		CDSU_TRACE (1, "Error in == Operator \n");
	} else
	{
		CDSU_TRACE (1, "== Operator tested successfully\n");
	}

	if (selfModuleId == destModuleId) 
	{
		CDSU_TRACE (1, "Error in == operator\n");
	} else
	{
		CDSU_TRACE (1, "== operator tested successfully\n");
	}
	
	if (selfModuleId != destModuleId)
	{
		CDSU_TRACE (1, "!= operator tested successfully\n");
	} else
	{
		CDSU_TRACE (1, "Error in != operator\n");
	}
	
	destModuleId = selfModuleId;

	if (selfModuleId == destModuleId)
	{
		CDSU_TRACE (1, "= and == operator tested successfully\n");
	} else
	{
		CDSU_TRACE (1, "Error in = operator\n");
	}
	if (selfModuleId != destModuleId)
	{
		CDSU_TRACE (1, "Error in = or != operator");
	} else
	{
		CDSU_TRACE (1, "= and != operator tested successfully\n");
	}
	
}
