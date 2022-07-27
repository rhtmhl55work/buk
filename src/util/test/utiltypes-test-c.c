//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: cdsuutiltypes-test-c.cpp
// Description	: Tests for cdsuutiltypes-c.
// Ref Docs	:
// Author	: Vijaya Kumar 
// Created On   : Sat Apr 22 19:28:54 IST 2006
// Last Modified: 
//==============================================================================

#include <stdio.h>
#include "cdsuutiltypes-c.h"

int main ()
{

	CdModuleId selfModuleId;
	CdModuleId destModuleId;
	CdModuleId peerDestModId;
	
	printf("Entered Main\n");
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
//	destModuleId.srvcHlrId.plfSrvcHlrId.pad8 = 3;
//	destModuleId.srvcHlrId.plfSrvcHlrId.plfModuleId = 300 ;

	peerDestModId.hwUnitId.rackId = 2;
	peerDestModId.hwUnitId.unitId = 2;
	peerDestModId.hwUnitId.slotId = 2;
	peerDestModId.hwUnitId.procId = 2;
	peerDestModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_PLATFORM_SERVICE;
//	peerDestModId.srvcHlrId.plfSrvcHlrId.pad8 = 3;
//	peerDestModId.srvcHlrId.plfSrvcHlrId.plfModuleId = 300 ;
	
	
/*	if (destModuleId == peerDestModId)
	{
		printf("== Operator tested successfully\n");
	} else
	{
		printf("== Operator not tested successfully\n");
	}
	
	peerDestModId.srvcHlrId.plfSrvcHlrId.plfModuleId++;

	if (destModuleId == peerDestModId)
	{
		printf("Error in == Operator \n");
	} else
	{
		printf("== Operator tested successfully\n");
	}

	if (selfModuleId == destModuleId) 
	{
		printf("Error in == operator\n");
	} else
	{
		printf("== operator tested successfully\n");
	}
	
	if (selfModuleId != destModuleId)
	{
		printf("!= operator tested successfully\n");
	} else
	{
		printf("Error in != operator\n");
	}
	
	destModuleId = selfModuleId;

	if (selfModuleId == destModuleId)
	{
		printf("= and == operator tested successfully\n");
	} else
	{
		printf("Error in = operator\n");
	}
	if (selfModuleId != destModuleId)
	{
		printf("Error in = or != operator");
	} else
	{
		printf("= and != operator tested successfully\n");
	}
*/	
}
