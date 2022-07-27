//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: basicinfraMyTraineemenu.cpp
// Description	:
// Ref Docs	:
// Author	: rohitm 2022
// Contributors	:
// Created On	: 22.07.2022
// Last Modified:
//============================================================================

#include "basicinfra.h"
#include "basicinfraparam.h"

//for registering message distributor applications
CdModuleId myTraineeMenuM;

//for registering network handler
CdSuActorId nhMyTraineeMenuM;

//for udp nh
int localPortOfBothFixedMyTraineeMenu;
int remotePortOfBothFixedMyTraineeMenu;

// Sys Mgmt Actor ID
CdSuActorId smMyTraineeMenuMid;

void basicinfraMyTraineeMenu()
{
	//assign ports for NH
	localPortOfBothFixedMyTraineeMenu = CDSU_UDP_MYTRAINEEMENU_NH;
	remotePortOfBothFixedMyTraineeMenu = REMOTE_PORT_BOTH_FIXED;

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
	selfMid.hwUnitId = getHwUnitIdMyTraineeMenu();

	CdSuMdInitParam param;
	param.selfMid = selfMid ;
	param.ipaddr = ipaddr2 ;
	param.udpLpn = CDSU_UDP_MYTRAINEEMENU_MD;

	CDSU_TRACE (5, "Creating Message Distributor\n");
	// Create the Message Distributor.
	if (!cdSuMdInit (param))
	{
		CDSU_TRACE (5, "Creating Message Distributor failed\n");
		exit (0);
	}

	// Register Hardware UnitId of trainee card.
	CdHardwareUnitId tHwUnitId;
	tHwUnitId = getHwUnitIdTrainee();
	CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	char remoteipAdd [4];
	remoteipAdd [0] = IP0;
	remoteipAdd [1] = IP1;
	remoteipAdd [2] = IP2;
	remoteipAdd [3] = IP3;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;
	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_TRAINEE_MD;

	if (cdSuMdRegisterHwUnit (tHwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Trainee HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Trainee HwUnit registration failed.\n");

//	// Register Hardware UnitId of admin card.
//	CdHardwareUnitId hwUnitId;
//	hwUnitId = getHwUnitIdAdmin();
//	CdSuMdIpcpInfo ipcpInfo;
//	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;
//
//	char remoteipAdd [4];
//	remoteipAdd [0] = IP0;
//	remoteipAdd [1] = IP1;
//	remoteipAdd [2] = IP2;
//	remoteipAdd [3] = IP3;
//	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
//	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;
//	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_ADMIN_MD;
//
//	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
//		CDSU_TRACE (5, "Admin HwUnit registered successfully.\n");
//	else
//		CDSU_TRACE (1, "Admin HwUnit registration failed.\n");

//	// Register Hardware UnitId of mentor card.
//	CdHardwareUnitId mtHwUnitId;
//	mtHwUnitId = getHwUnitIdMentor();
//
//	//CdSuMdIpcpInfo ipcpInfo;
//	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;
//
//	//char remoteipAdd [4];
//	//remoteipAdd [0] = IP0;
//	//remoteipAdd [1] = IP1;
//	//remoteipAdd [2] = IP2;
//	//remoteipAdd [3] = IP3;
//
//	Uint32 remoteipaddM = changeIpAddr (remoteipAdd);
//	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipaddM;
//	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_MENTOR_MD;
//
//	if (cdSuMdRegisterHwUnit (mtHwUnitId, ipcpInfo) == true)
//		CDSU_TRACE (5, "Mentor HwUnit registered successfully.\n");
//	else
//		CDSU_TRACE (1, "Mentor HwUnit registration failed.\n");

	//-------------------------------------------------------------------
	// Application Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&myTraineeMenuM, 0, sizeof (myTraineeMenuM));
	myTraineeMenuM = getApplMidMyTraineeMenu();

	if (cdSuMdRegisterMid (myTraineeMenuM) == true)
	{
		printf (" Application Module of myTraineeMenu has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: Application Module of myTraineeMenu cannot be registered with MsgDistributor . \n");
	}
	// Allocate ActorIds (Network handler Module ID).
	nhMyTraineeMenuM = getNhMidMyTraineeMenu();

	if (cdSuMdRegisterMid (nhMyTraineeMenuM) == true)
	{
		printf (" NH UDP has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: NH UDP cannot be registered with MsgDistributor.\n");
	}
} //basicinfraMyTraineeMenu()

// to get hardware unit id of MyTraineeMenu
CdHardwareUnitId getHwUnitIdMyTraineeMenu()
{
	CdHardwareUnitId myTraineeMenuHwUnit;

	myTraineeMenuHwUnit.rackId = 13;
	myTraineeMenuHwUnit.unitId = 13;
	myTraineeMenuHwUnit.slotId = 13;
	myTraineeMenuHwUnit.procId = 13;

	return myTraineeMenuHwUnit;
} //getHwUnitIdMyTraineeMenu()

// to get Network handler module id of Trainee
CdSuActorId getNhMidMyTraineeMenu()
{
	nhMyTraineeMenuM.hwUnitId.rackId = 13;
	nhMyTraineeMenuM.hwUnitId.unitId = 13;
	nhMyTraineeMenuM.hwUnitId.slotId = 13;
	nhMyTraineeMenuM.hwUnitId.procId = 13;

	nhMyTraineeMenuM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhMyTraineeMenuM.srvcHlrId.appSrvcHlrId.appType = 2;
	nhMyTraineeMenuM.srvcHlrId.appSrvcHlrId.appId = 2;
	nhMyTraineeMenuM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	return nhMyTraineeMenuM;
} //getNhMidMyTraineeMenu()

// to get message distributor application module id of MyTraineeMenu
CdModuleId getApplMidMyTraineeMenu()
{
	myTraineeMenuM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	myTraineeMenuM.srvcHlrId.appSrvcHlrId.appType = 4;
	myTraineeMenuM.srvcHlrId.appSrvcHlrId.appId = 4;
	myTraineeMenuM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	myTraineeMenuM.hwUnitId.rackId = 13;
	myTraineeMenuM.hwUnitId.unitId = 13;
	myTraineeMenuM.hwUnitId.slotId = 13;
	myTraineeMenuM.hwUnitId.procId = 13;

	return myTraineeMenuM;
} //getApplMidMyTraineeMenu()

//============================================================================
// End of basicinfraMyTraineeMenu.cpp
//============================================================================

