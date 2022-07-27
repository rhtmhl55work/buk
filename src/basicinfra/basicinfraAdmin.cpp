//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: basicinfraAdmin.cpp
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
CdModuleId adminM;

//for registering network handler
CdSuActorId nhAdminM;

//for udp nh
int localPortOfBothFixedAdmin;
int remotePortOfBothFixedAdmin;

//server ip
char localIpAddr [4] = {(char)IP0, (char)IP1, (char)IP2, (char)IP3};

//client ip
char remoteIpAddr [4] = {(char)IP0, (char)IP1, (char)IP2, (char)IP3};

// Sys Mgmt Actor ID.
CdSuActorId smAdminMid;

CdSuNhUdpInstId instID;
CdSuNhUdpPortKey portKey;

char failCodeString  [] [100] =
{
	"",
	"CDSU_NHUDP_RSPFCODE_INVPARAM",
	"CDSU_NHUDP_RSPFCODE_INV_OP",
	"CDSU_NHUDP_RSPFCODE_UDPPORT_OVERFLOW",
	"CDSU_NHUDP_RSPFCODE_BINDERROR",
	"CDSU_NHUDP_RSPFCODE_CONNECTERROR",
	"CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST",
	"CDSU_NHUDP_RSPFCODE_UNKNOWNERROR",
	"CDSU_NHUDP_RSPFCODE_SENDERROR",
	"CDSU_NHUDP_RSPFCODE_OPENERROR",
	"CDSU_NHUDP_RSPFCODE_READERROR",
	"CDSU_NHUDP_RSPFCODE_INVALID"
} ;


void basicinfraAdmin()
{
	//ports assignment for NH
	localPortOfBothFixedAdmin = CDSU_UDP_ADMIN_NH;
	remotePortOfBothFixedAdmin = REMOTE_PORT_BOTH_FIXED;

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
	selfMid.hwUnitId = getHwUnitIdAdmin();

	CdSuMdInitParam param;
	param.selfMid = selfMid ;
	param.ipaddr = ipaddr2 ;
	param.udpLpn = CDSU_UDP_ADMIN_MD;

	CDSU_TRACE (5, "Creating Message Distributor\n");
	// Create the Message Distributor.
	if (!cdSuMdInit (param))
	{

		CDSU_TRACE (5, "Creating Message Distributor failed\n");
		exit (0);
	}

	// Register Hardware UnitId of mentor card
	CdHardwareUnitId hwUnitId;
	hwUnitId = getHwUnitIdMentor();

	CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	char remoteipAdd [4];
	remoteipAdd [0] = IP0;
	remoteipAdd [1] = IP1;
	remoteipAdd [2] = IP2;
	remoteipAdd [3] = IP3;

	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;

	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_MENTOR_MD;

	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Mentor HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Mentor HwUnit registration fails\n");

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

	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;
	ipcpInfo.u.remoteUdpInfo.portNo = CDSU_UDP_TRAINEE_MD;

	if (cdSuMdRegisterHwUnit (trHwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Trainee HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Trainee HwUnit registration failed.\n");

	//-------------------------------------------------------------------
	// Application Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&adminM, 0, sizeof (adminM));
	adminM = getApplMidAdmin();

	if (cdSuMdRegisterMid (adminM) == true)
	{
		printf (" Application has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: Application cannot be registered with MsgDistributor . \n");
	}

	// Allocate ActorIds (Network handler Module ID).
	nhAdminM = getNhMidAdmin();

	if (cdSuMdRegisterMid (nhAdminM) == true)
	{
		printf (" NH UDP has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: NH UDP cannot be registered with MsgDistributor . \n");
	}
} //basicinfraAdmin()

//---------------------------------------------------------------------------
// changeIpAddr ()
// Description  : Function which changes char[4] to Uint32. Here used to
// 		  change the IpV4Address which is in char[4] format to
// 		  Uint32 format.
// Inputs       : char ipaddress [4].
// Return Value : Uint32 ipaddr.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

Uint32 changeIpAddr (char ipaddr[])
{
	CDSU_TRACE1 (4, "%s\n",__PRETTY_FUNCTION__);

	Uint32 ipa;
	char* p = (char*)&ipa;
	p[0] = ipaddr[0];
	p[1] = ipaddr[1];
	p[2] = ipaddr[2];
	p[3] = ipaddr[3];

	return (ipa);
} //changeIpAddr()

// function to get hardware unit id of admin
CdHardwareUnitId getHwUnitIdAdmin()
{
	CdHardwareUnitId adhwUnit;

	adhwUnit.rackId = 1;
	adhwUnit.unitId = 1;
	adhwUnit.slotId = 1;
	adhwUnit.procId = 1;

	return adhwUnit;
} //getHwUnitIdAdmin()

// function to get network handler id of admin
CdSuActorId getNhMidAdmin()
{
	nhAdminM.hwUnitId.rackId = 1;
	nhAdminM.hwUnitId.unitId = 1;
	nhAdminM.hwUnitId.slotId = 1;
	nhAdminM.hwUnitId.procId = 1;

	nhAdminM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhAdminM.srvcHlrId.appSrvcHlrId.appType = 2;
	nhAdminM.srvcHlrId.appSrvcHlrId.appId = 2;
	nhAdminM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	return nhAdminM;
} //getNhMidAdmin()

// function to get message distributor application module id of admin
CdModuleId getApplMidAdmin()
{
	adminM.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	adminM.srvcHlrId.appSrvcHlrId.appType = 4;
	adminM.srvcHlrId.appSrvcHlrId.appId = 4;
	adminM.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	adminM.hwUnitId.rackId = 1;
	adminM.hwUnitId.unitId = 1;
	adminM.hwUnitId.slotId = 1;
	adminM.hwUnitId.procId = 1;

	return adminM;
} //getApplMidAdmin()

//============================================================================
// End of basicinfraAdmin.cpp
//============================================================================
