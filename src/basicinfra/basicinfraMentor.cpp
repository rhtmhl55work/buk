//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: basicinfraMentor.cpp
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
CdModuleId mentorM;

//for registering network handler
CdSuActorId nhMentorM;

//for udp nh
int localPortOfBothFixedMentor;
int remotePortOfBothFixedMentor;

// Sys Mgmt Actor ID.
CdSuActorId smMentorMid;

void basicinfraMentor()
{
	//assign NH ports
	localPortOfBothFixedMentor = CDSU_UDP_MENTOR_NH;
	remotePortOfBothFixedMentor = REMOTE_PORT_BOTH_FIXED;

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
	selfMid.hwUnitId = getHwUnitIdMentor();

	CdSuMdInitParam param;
	param.selfMid = selfMid ;
	param.ipaddr = ipaddr2 ;
	param.udpLpn = CDSU_UDP_MENTOR_MD;

	CDSU_TRACE (5, "Creating Message Distributor\n");
	// Create the Message Distributor.
	if (!cdSuMdInit (param))
	{
		CDSU_TRACE (5, "Creating Message Distributor failed\n");
		exit (0);
	}

	// Register Hardware UnitId of admin card.
	CdHardwareUnitId hwUnitId;
	hwUnitId = getHwUnitIdAdmin();
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

	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Admin HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Admin HwUnit registration fails\n");

	// Register Hardware UnitId of trainee card
	CdHardwareUnitId trHwUnitId;
	trHwUnitId = getHwUnitIdTrainee();
	//CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	//char remoteipAdd [4];
	//remoteipAdd [0] = IP0;
	//remoteipAdd [1] = IP1;
	//remoteipAdd [2] = IP2;
	//remoteipAdd [3] = IP3;

	Uint32 remoteipaddT = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipaddT;
	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_TRAINEE_MD;

	if (cdSuMdRegisterHwUnit (trHwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Trainee HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Trainee HwUnit registration fails\n");

	//-------------------------------------------------------------------
	// Application I Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&mentorM, 0, sizeof (mentorM));
	mentorM = getApplMidMentor();

	if (cdSuMdRegisterMid (mentorM) == true)
	{
		printf (" Application Module of mentor has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: Application Module of mentor cannot be registered with MsgDistributor . \n");
	}
	// Allocate ActorIds (Network handler Module ID).
	nhMentorM = getNhMidMentor();

	if (cdSuMdRegisterMid (nhMentorM) == true)
	{
		printf (" NH UDP has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: NH UDP cannot be registered with MsgDistributor . \n");
	}
} //basicinfraMentor()

// to get hardware unit id of mentor
CdHardwareUnitId getHwUnitIdMentor()
{
	CdHardwareUnitId mthwUnit;

	mthwUnit.rackId = 2;
	mthwUnit.unitId = 2;
	mthwUnit.slotId = 2;
	mthwUnit.procId = 2;

	return mthwUnit;
} //getHwUnitIdMentor()

// to get network handler module id of mentor
CdSuActorId getNhMidMentor()
{
	nhMentorM.hwUnitId.rackId = 2;
	nhMentorM.hwUnitId.unitId = 2;
	nhMentorM.hwUnitId.slotId = 2;
	nhMentorM.hwUnitId.procId = 2;

	nhMentorM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhMentorM.srvcHlrId.appSrvcHlrId.appType = 2;
	nhMentorM.srvcHlrId.appSrvcHlrId.appId = 2;
	nhMentorM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	return nhMentorM;
} //getNhMidMentor()

// to get message distributor application module id module id of mentor
CdModuleId getApplMidMentor()
{
	mentorM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	mentorM.srvcHlrId.appSrvcHlrId.appType = 4;
	mentorM.srvcHlrId.appSrvcHlrId.appId = 4;
	mentorM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	mentorM.hwUnitId.rackId = 2;
	mentorM.hwUnitId.unitId = 2;
	mentorM.hwUnitId.slotId = 2;
	mentorM.hwUnitId.procId = 2;

	return mentorM;
} //getApplMidMentor()

//============================================================================
// End of basicinfraMentor.cpp
//============================================================================
