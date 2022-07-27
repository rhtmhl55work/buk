//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: basicinfraTrainee.cpp
// Description	:
// Ref Docs	:
// Author	: Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

#include "basicinfra.h"
#include "basicinfraparam.h"

//for registering message distributor applications
CdModuleId traineeM;

//for registering network handler
CdSuActorId nhTraineeM;

//for udp nh
int localPortOfBothFixedTrainee;
int remotePortOfBothFixedTrainee;

// Sys Mgmt Actor ID
CdSuActorId smTraineeMid;

void basicinfraTrainee()
{
	//assign ports for NH
	localPortOfBothFixedTrainee = CDSU_UDP_TRAINEE_NH;
	remotePortOfBothFixedTrainee = REMOTE_PORT_BOTH_FIXED;

	// Create the Message Distributor.
	char ipAddress [4];
	ipAddress [0] = IP0;
	ipAddress [1] = IP1;
	ipAddress [2] = IP2;
	ipAddress [3] = IP3;

	Uint32 ipaddr2;
	ipaddr2 = changeIpAddr (ipAddress);

	// ModuleId of the message distributor.
	CdModuleId selfMid;
	memset (&selfMid, 0, sizeof (selfMid));
	selfMid.hwUnitId = getHwUnitIdTrainee();

	CdSuMdInitParam param;
	param.selfMid = selfMid ;
	param.ipaddr = ipaddr2 ;
	param.udpLpn = CDSU_UDP_TRAINEE_MD;

	CDSU_TRACE (5, "Creating Message Distributor\n");
	// Create the Message Distributor.
	if (!cdSuMdInit (param))
	{
		CDSU_TRACE (5, "Creating Message Distributor failed\n");
		exit (0);
	}

	// Register Hardware UnitId of admin card.
	CdHardwareUnitId adHwUnitId;
	adHwUnitId = getHwUnitIdAdmin();
	CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	char remoteipAdd [4];
	remoteipAdd [0] = IP0;
	remoteipAdd [1] = IP1;
	remoteipAdd [2] = IP2;
	remoteipAdd [3] = IP3;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;
	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_ADMIN_MD;

	if (cdSuMdRegisterHwUnit (adHwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Admin HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Admin HwUnit registration failed.\n");

	// Register Hardware UnitId of myTraineeMenu card.
	CdHardwareUnitId mtmHwUnitId;
	mtmHwUnitId = getHwUnitIdMyTraineeMenu();
	//CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

//	char remoteipAdd [4];
//	remoteipAdd [0] = IP0;
//	remoteipAdd [1] = IP1;
//	remoteipAdd [2] = IP2;
//	remoteipAdd [3] = IP3;
	Uint32 remoteipaddMTM = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipaddMTM;
	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_MYTRAINEEMENU_MD;

	if (cdSuMdRegisterHwUnit (mtmHwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "MyTraineeMenu HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "MyTraineeMenu HwUnit registration failed.\n");

	// Register Hardware UnitId of mentor card.
	CdHardwareUnitId mHwUnitId;
	mHwUnitId = getHwUnitIdMentor();

	//CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	//char remoteipAdd [4];
	//remoteipAdd [0] = IP0;
	//remoteipAdd [1] = IP1;
	//remoteipAdd [2] = IP2;
	//remoteipAdd [3] = IP3;

	Uint32 remoteipaddM = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipaddM;
	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_MENTOR_MD;

	if (cdSuMdRegisterHwUnit (mHwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Mentor HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Mentor HwUnit registration failed.\n");

	//-------------------------------------------------------------------
	// Application Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&traineeM, 0, sizeof (traineeM));
	traineeM = getApplMidTrainee();

	if (cdSuMdRegisterMid (traineeM) == true)
	{
		printf (" Application Module of trainee has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: Application Module of trainee cannot be registered with MsgDistributor . \n");
	}
	// Allocate ActorIds (Network handler Module ID).
	nhTraineeM = getNhMidTrainee();

	if (cdSuMdRegisterMid (nhTraineeM) == true)
	{
		printf (" NH UDP has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: NH UDP cannot be registered with MsgDistributor.\n");
	}
} //basicinfraTraninee()

// to get hardware unit id of Trainee
CdHardwareUnitId getHwUnitIdTrainee()
{
	CdHardwareUnitId trHwUnit;

	trHwUnit.rackId = 3;
	trHwUnit.unitId = 3;
	trHwUnit.slotId = 3;
	trHwUnit.procId = 3;

	return trHwUnit;
} //getHwUnitIdTrainee()

// to get Network handler module id of Trainee
CdSuActorId getNhMidTrainee()
{
	nhTraineeM.hwUnitId.rackId = 3;
	nhTraineeM.hwUnitId.unitId = 3;
	nhTraineeM.hwUnitId.slotId = 3;
	nhTraineeM.hwUnitId.procId = 3;

	nhTraineeM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhTraineeM.srvcHlrId.appSrvcHlrId.appType = 2;
	nhTraineeM.srvcHlrId.appSrvcHlrId.appId = 2;
	nhTraineeM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	return nhTraineeM;
} //getNhMidTrainee()

// to get message distributor application module id of Trainee
CdModuleId getApplMidTrainee()
{
	traineeM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	traineeM.srvcHlrId.appSrvcHlrId.appType = 4;
	traineeM.srvcHlrId.appSrvcHlrId.appId = 4;
	traineeM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	traineeM.hwUnitId.rackId = 3;
	traineeM.hwUnitId.unitId = 3;
	traineeM.hwUnitId.slotId = 3;
	traineeM.hwUnitId.procId = 3;

	return traineeM;
} //getApplMidTrainee()

//============================================================================
// End of basicinfraTrainee.cpp
//============================================================================
