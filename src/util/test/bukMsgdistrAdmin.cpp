//==============================================================================
// BUK assignment - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: bukmsgthrd.cpp
// Description	: 
// Ref Docs	:
// Author	: Trainee - 2013
// Created On   : 07.10.2013
// Last Modified:
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cdsumdapi.h"
#include "msgdistr-test.h"
#include "cdsutrace.h"
#include <iostream>
#include <cdsuthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cdsunhudpapi.h"
#include <semaphore.h>
#include "bukUiAdminMsgs.h"

//for message distributor
#define CDSU_UDP_TESTER1_LPN 50001	

//for udp nh
int localPortOfBothFixed = 50003;
int remotePortOfBothFixed = 0xffff;

//server ip
char localIpAddr [4] = { 192, 168, 16, 129 };

//client ip
char remoteIpAddr [4] = {192, 168, 16, 129 };

void smMgmtEventHlr (void*);
void applEventHlr (void*);
void smMgmtEventHlrForMsgIntf (void*);
void applEventHlrForMsgIntf (void*);

//extern void cdCreateManagerReceiverThread (CdModuleId* mId);
//extern void cdCreateApplReceiverThread (CdModuleId* mId);

// Module ID of Different Module
CdSuActorId smModId;		// Sys Mgmt Actor ID.
CdSuActorId nhModId;		// nh MOD ID.
CdSuActorId applnModId;		// Application Actor ID.

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

//two applications for sending and receiving message
CdModuleId applMid1;
CdModuleId applMid2;
CdModuleId applMid3;

// for creating applications (threads)
void createApplications ();
void createApplication2 ();

//using CdSuThread 
class TestThread:public CdSuThread
{
public:
	static void* applicationStart1 (void*);
	static void* applicationStart2 (void*);
private:

};

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
}

// Helper Functions
//---------------------------------------------------------------------------
// Function	: cdSuNhUdpConvIp2Int ()
// Description  : 
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

Uint32 cdSuNhUdpConvIp2Int (const char* ipaddr)
{
	Uint32 ipa;
	char* p = (char*)&ipa;
	p[0] = ipaddr [0];
	p[1] = ipaddr [1];
	p[2] = ipaddr [2];
	p[3] = ipaddr [3];
	
	return ipa;
} 

//---------------------------------------------------------------------------
// Function	: cdSuNhUdpConvInt2Ip ()
// Description  : 
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void cdSuNhUdpConvInt2Ip (char* ipaddr,  Uint32 a)
{
	memcpy (ipaddr, (char*) a, 4);
} 

//---------------------------------------------------------------------------
// Function	: cdSuNhUdpConvInt2Addr ()
// Description  : 
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

char* cdSuNhUdpConvInt2Addr (Uint32 a)
{
	struct sockaddr_in addr;
	memset (&addr,0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = a;
	addr.sin_port = 0;
	return inet_ntoa (addr.sin_addr);
} 

//----------------------------------------------------------------------------
// Function	: displayErrorType ()
// Description	: This is System Management Thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------


void displayErrorType (CdSuNhUdpRspFailCode failCode)
{
	if (CDSU_NHUDP_RSPFCODE_INVALID > failCode && failCode > -1)
	printf ("Error Type %s\n", failCodeString [failCode]);
}

//----------------------------------------------------------------------------
// Function	: smMgmtEventHlr ()
// Description	: This is Listener Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void smMgmtEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	printf ("Response to System Manager\n");
	
	CdSuNhUdpMiEvent* eventParam = (CdSuNhUdpMiEvent*) arg;
	switch (eventParam->opcode)
	{
	case CDSU_NHUDP_MI_INIT_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiInitRspEvent*)eventParam)->rspCode;
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Init Nh Success\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
		} else 
		{
			printf ("Init Nh fail\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
			displayErrorType (((CdSuNhUdpMiInitRspEvent*)eventParam)->failCode);
			
		}
		}
		break;
		
	case CDSU_NHUDP_MI_DELNH_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiDelNhRspEvent*)eventParam)->rspCode;
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Del Nh Success\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
		} else 
		{
			printf ("Del Nh fail\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
			displayErrorType (((CdSuNhUdpMiDelNhRspEvent*)eventParam)->failCode);
			
		}
		}
		break;

	case CDSU_NHUDP_MI_CTRL_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiCtrlRspEvent*)eventParam)->rspCode;
		CdSuNhUdpSubOpcode subOpcode = ((CdSuNhUdpMiCtrlRspEvent*)eventParam)->subOpcode;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			if (subOpcode == CDSU_NHUDP_SO_START_RSP)
			{
			printf ("Start UDP NH Failed\n");
			} else  if (subOpcode == CDSU_NHUDP_SO_STOP_RSP)
			{
			printf (" Stop UDP NH Failed\n");
			} else
			{
				printf ("Control Operation Failed\n");
			}
			displayErrorType (((CdSuNhUdpMiCtrlRspEvent*)eventParam)->failCode);

		} else 
		{
			if (subOpcode == CDSU_NHUDP_SO_START_RSP)
			{
				printf ("Start UDP NH Success\n");
			} else  
			{
				printf (" Stop UDP NH Success\n");
			}
		
		}
		}
		break;
	
	case CDSU_NHUDP_MI_STATUS_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiStatusRspEvent*)eventParam)->rspCode;
		CdSuNhUdpState nhState = ((CdSuNhUdpMiStatusRspEvent*)eventParam)->nhState;
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Status Nh Success\n");
			if (nhState == CDSU_NHUDP_ST_CREATED)
			{
			printf ("State Of NH = CREATED\n");
			} else if (nhState == CDSU_NHUDP_ST_RUNNING)
			{
			printf ("State Of NH = RUNNING\n");
			}
		} else 
		{
			printf ("Status Nh fail\n");
			displayErrorType (((CdSuNhUdpMiStatusRspEvent*)eventParam)->failCode);

		}
		}
		break;

	case CDSU_NHUDP_MI_STATISTICS_RSP:
		{
		CdSuNhUdpMiStsRspEvent* stsRspEvnetParam = 
					((CdSuNhUdpMiStsRspEvent*)eventParam);
		CdSuNhUdpRspCode rspCode = stsRspEvnetParam->rspCode;
		
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Statistics Nh Success. Statistics Report==>\n");
			for (int i = 0 ; i < CDSU_NHUDP_MAX_CTR; ++i)
			{
			//printf ("COUNTER [%s]== %d\n",  counterString [i], stsRspEvnetParam->counters [i]);
			}

		} else 
		{
			printf ("Statistics Nh fail\n");
			displayErrorType (((CdSuNhUdpMiStsRspEvent*)eventParam)->failCode);

		}
		}
		break;

	} // switch
	
	//Delete event.
	delete eventParam;
	eventParam = NULL;
	//sem_post (&semaphore);
} // smMgmtEvnetHlr ()

//----------------------------------------------------------------------------
// Function	: applEventHlr()
// Description	: This is Application Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void applEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	printf ("Response to Application\n");
	
	CdSuNhUdpUiEvent* eventParam = (CdSuNhUdpUiEvent*) arg;
	switch (eventParam->opcode)
	{
	case CDSU_NHUDP_UI_ADD_UDPPORT_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		CdSuIpv4UdpPortNo localPortNo=  ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->localPortNo;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Add Udp Port Failed\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
			displayErrorType (failCode);
		} else 
		{
			printf ("Add Udp Port Succes\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
		}
		}
		break;
	
	case CDSU_NHUDP_UI_REM_UDPPORT_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiRemUdpPortRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiRemUdpPortRspEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiRemUdpPortRspEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Rem Udp Port Failed\n");
			printf ("Rem Port ID = %d\n", portId);
			displayErrorType (failCode);

		} else 
		{
			printf ("Rem Udp Port Succes\n");
			printf ("Rem Port ID = %d\n", portId);
		}
		}
		break;
	
	case CDSU_NHUDP_UI_DATA_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiDataRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiDataRspEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiDataRspEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		Uint32 opaqueData = ((CdSuNhUdpUiDataRspEvent*)eventParam)->opaqueData;
		
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Send Data Failed\n");
			printf ("Port ID = %d\n", portId);
			printf ("Reason %d\n", failCode);
			printf ("Opaque Data %d\n", opaqueData);
			displayErrorType (failCode);

		} else 
		{
			printf ("Send Data Succes\n");
			printf ("Port ID = %d\n", portId);
			printf ("Opaque Data %d\n", opaqueData);
		}
		}
		break;

	case CDSU_NHUDP_UI_DATA_IND:
		{

			printf ("Response to Application\n");
			CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiDataIndEvent*)eventParam)->rspCode;
			CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiDataIndEvent*)eventParam)->failCode;
			CdSuNhUdpPortId portId = ((CdSuNhUdpUiDataIndEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
			if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
			{
				printf ("Data Ind Failed\n");
				printf ("Port ID = %d\n", portId);
				printf ("Reason %d\n", failCode);
				printf ("Data Len = %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen);
				printf ("Data = %s\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr);
				printf ("Remote PortNo %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo);
				printf ("Dest Addr = %s\n", cdSuNhUdpConvInt2Addr (((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr));
				displayErrorType (((CdSuNhUdpUiDataIndEvent*)eventParam)->failCode);


			} else 
			{
			printf ("Data Indication\n");
			printf ("Port ID = %d\n", portId);
			printf ("Data Len = %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen);
			//printf ("Data = %s\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr);
			printf ("Remote PortNo %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo);
			printf ("Dest Addr = %s\n", cdSuNhUdpConvInt2Addr (((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr));
			BukUiAdMsg* msg = (BukUiAdMsg *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
			std::cout<<"opcode: "<<msg->opcode<<"\n";
			//printf ("Data 2 = %d\n",(int)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr);

			if (((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen > 0)
			{
				// delete [] ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
			}

			//cdMessage* rxMsg;

			//createApplication2 ();
		}
		if (((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr != NULL)
		{
			delete [] ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
		}
		}
		break;
	
	default:
		break;
	} // 	switch (eventParam->opcode)
	delete  eventParam;
	 //sem_post (&semaphore);

} // applEventHlr ()

void initAllGlobals ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId.hwUnitId.rackId = 1;
	nhModId.hwUnitId.unitId = 1;
	nhModId.hwUnitId.slotId = 1;
	nhModId.hwUnitId.procId = 1;

	nhModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId.srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appId = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.
	
	// Allocate ActorIds (System Management Module ID).
	smModId.hwUnitId.rackId = 1;
	smModId.hwUnitId.unitId = 1;
	smModId.hwUnitId.slotId = 1;
	smModId.hwUnitId.procId = 1;

	smModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	smModId.srvcHlrId.appSrvcHlrId.appType = 2;
	smModId.srvcHlrId.appSrvcHlrId.appId = 2;
	smModId.srvcHlrId.appSrvcHlrId.appModuleId = 2;	// To be changed later.


	// Note: Appl Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Appl Module ID).
	applnModId.hwUnitId.rackId = 1;
	applnModId.hwUnitId.unitId = 1;
	applnModId.hwUnitId.slotId = 1;
	applnModId.hwUnitId.procId = 1;

	applnModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	applnModId.srvcHlrId.appSrvcHlrId.appType = 3;
	applnModId.srvcHlrId.appSrvcHlrId.appId = 3;
	applnModId.srvcHlrId.appSrvcHlrId.appModuleId = 3;	// To be changed later.
	
	// Note: Appl Hardware Unit Id should be same as given in testserver.
		
	/*CdSuMdInitParam msgDstbInitParam;
	memset ((void *)&msgDstbInitParam.selfMid, 0, sizeof (CdModuleId));
	msgDstbInitParam.ipaddr = cdSuNhUdpConvIp2Int (localIpAddr);
	
	if (cdSuMdInit (msgDstbInitParam) == true)
	{
		CDSU_TRACE (1, "Message distributor is created successfully.\n");
	} else 
	{
		printf ("Message distributor creation fail.\n");		
		return ;	
	}
	*/

	//if (cdSuMdRegisterMid ()
	cdSuMdRegisterMid (nhModId);
	cdSuMdRegisterMid (smModId);	
	cdSuMdRegisterMid (applnModId);	

} // initAllGlobals ()


int main ()
{
	CdSuTrace::setLevel (5);
	
	// Create the Message Distributor.
	char ipAddress [4];
	ipAddress [0] = 192;
	ipAddress [1] = 168;
	ipAddress [2] = 16;
	ipAddress [3] = 129;
	
	Uint32 ipaddr2;
	ipaddr2 = changeIpAddr (ipAddress);

	// ModuleId of the message distributor.
	CdModuleId selfMid;
	memset (&selfMid, 0, sizeof (selfMid));
	selfMid.hwUnitId.rackId = 1 ;
	selfMid.hwUnitId.unitId = 1;
	selfMid.hwUnitId.slotId = 1;
	selfMid.hwUnitId.procId = 1;
	
	CdSuMdInitParam param;
	param.selfMid = selfMid ;
	param.ipaddr = ipaddr2 ;
	param.udpLpn = CDSU_UDP_TESTER1_LPN;
	
	CDSU_TRACE (5, "Creating Message Distributor\n");
	// Create the Message Distributor.
	if (!cdSuMdInit (param))
	{
		
		CDSU_TRACE (5, "Creating Message Distributor failed\n");
		exit (0);	
	}
	
	// Register Hardware UnitId of another card.
	CdHardwareUnitId hwUnitId;

	hwUnitId.rackId = 2;
	hwUnitId.unitId = 2;
	hwUnitId.slotId = 2;
	hwUnitId.procId = 2;

	CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	char remoteipAdd [4];
	remoteipAdd [0] = 192;
	remoteipAdd [1] = 168;
	remoteipAdd [2] = 16;
	remoteipAdd [3] = 129;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;

	ipcpInfo.u.remoteUdpInfo.portNo = 50002;
//	std::cout<<"-------------TEST CASE[5]--------------\n";

	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Destination HwUnit registered successfully. Test[5] SUCCESS\n");
	else
		CDSU_TRACE (1, "Destination HwUnit registration fails\n");
		

	
	//-------------------------------------------------------------------
	// Application I Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&applMid1, 0, sizeof (applMid1));
	
//	std::cout<<"-------------TEST CASE[2]--------------\n";

	applMid1.srvcHlrId.srvcType = 4;
	applMid1.srvcHlrId.appSrvcHlrId.appType = 4;
	applMid1.srvcHlrId.appSrvcHlrId.appId = 4;
	applMid1.srvcHlrId.appSrvcHlrId.appModuleId = 4;

	applMid1.hwUnitId.rackId = 1 ;
	applMid1.hwUnitId.unitId = 1;
	applMid1.hwUnitId.slotId = 1;
	applMid1.hwUnitId.procId = 1;
//	std::cout<<"-------------TEST CASE[1]--------------\n";

	if (cdSuMdRegisterMid (applMid1) == true)
	{
		printf (" Application I has been registered to MsgDistributor Successfully. Test[1] - SUCCESS \n");
	} else
	{
		printf (" Error:: Application I cannot be registered with MsgDistributor . \n");
	}
	
	//-------------------------------------------------------------------
	// Application II Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&applMid2, 0, sizeof (applMid2));

	applMid2.srvcHlrId.srvcType = 5;
	applMid2.srvcHlrId.appSrvcHlrId.appType = 5;
	applMid2.srvcHlrId.appSrvcHlrId.appId = 5;
	applMid2.srvcHlrId.appSrvcHlrId.appModuleId = 5;

	applMid2.hwUnitId.rackId = 1 ;
	applMid2.hwUnitId.unitId = 1;
	applMid2.hwUnitId.slotId = 1;
	applMid2.hwUnitId.procId = 1;

	if (cdSuMdRegisterMid (applMid1) == true)
	{
		printf (" Application II has been registered to MsgDistributor Successfully. Test[1] - SUCCESS \n");
	} else
	{
		printf (" Error:: Application II cannot be registered with MsgDistributor . \n");
	}

	createApplications ();
	
	while (1)
	{
		CdSuTsQueue <int> queue (5);	
		int i = 0;
		queue.popBlocking (i);
	}
	while (1)
	{
		; // Intentional
	}

} // main ()

//---------------------------------------------------------------------------
// createApplications () 
// Description  : Creation of application Thread.  
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------

void createApplications ()
{
	// Create Application Thread1.
	
	TestThread* testThread1 = new TestThread;
	testThread1->spawn (TestThread::applicationStart1);

} // createApplication ()

void createApplication2 ()
{
	// Create Application Thread1.
	
	TestThread* testThread2 = new TestThread;
	testThread2->spawn (TestThread::applicationStart2);

} // createApplication ()

//---------------------------------------------------------------------------
// applicationStart1 () 
// Description  : First application thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------
//thread id = testThread1 : applMid1
void* TestThread::applicationStart1 (void* dummy1)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	initAllGlobals();

	CdSuNhUdpMiInitParam initParam;

	//--------------------------------------------------------------------
	// 1.First Instance : Management Intf is Callback.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhModId;
	initParam.smActorId = smModId;
	initParam.smEventHlr = smMgmtEventHlr;
	
	// Create Instance.
	cdSuNhUdpMiInit (initParam, instID);

	// Start The UDP Nh
	CdSuNhUdpMiCtrlReqParam ctrlReqParam;
	ctrlReqParam.nhUdpInstId = instID;
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	if (cdSuNhUdpMiCtrlReq (ctrlReqParam) == CDSU_NHUDP_API_RT_SUCCESS )
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
	}

	CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;
	
	//--------------------------------------------------------------------
	// 1. Udp Port of type Both Fixed. : Callback Function Intf.
	// Instance : 1.
	//--------------------------------------------------------------------
	addUdpPortReqParam.nhUdpInstId	= instID;
	addUdpPortReqParam.applActorId	= applnModId;
	addUdpPortReqParam.applEventHlr = applEventHlr;
	addUdpPortReqParam.localPortNo = localPortOfBothFixed;
	addUdpPortReqParam.destPortNo = remotePortOfBothFixed;
	addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);

	// Add Udp Port
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, 	portKey);

	CdMessage* rxMsg;

	// Any Message that the Application receives.
	std::cout<<"-------------TEST CASE[11]--------------\n";
	//while (1)
	//{
		printf ("Waiting For Intra Card Message\n");
		if (cdSuMdRecvMsg (&rxMsg, applMid1) == true)
		{
			printf ("Intra Card Messge Recvd\n");
			CDSU_TRACE (5, "Msg Rxvd in MsgDistrTestI appl1. Test[11] - SUCCESS\n");
			
			CdSuIntraCardMsg* msg1 = (CdSuIntraCardMsg*) rxMsg;
			
			CDSU_TRACE1 (5, "msg.dataLen = %d\n", msg1->length);
			CDSU_TRACE1 (5, "msg.data = %s\n", msg1->buffer);
				
			cdSuFreeMsgBuf (rxMsg);
		
			applMid3.srvcHlrId.srvcType = 2;
			applMid3.srvcHlrId.appSrvcHlrId.appType = 2;
			applMid3.srvcHlrId.appSrvcHlrId.appId = 2;
			applMid3.srvcHlrId.appSrvcHlrId.appModuleId = 2;

			applMid3.hwUnitId.rackId = 2;
			applMid3.hwUnitId.unitId = 2;
			applMid3.hwUnitId.slotId = 2;
			applMid3.hwUnitId.procId = 2;
			
			char buf[4];
			//memset (&buf, 0, sizeof (buf));
			//buf[0]='a';
			//buf[1]='\0';
			
			strcpy(buf,"ack");
			CdSuIntraCardMsg* msg = (CdSuIntraCardMsg*) cdSuGetMsgBuf (sizeof (CdSuIntraCardMsg));
			msg->initMsg (applMid2, applMid1, buf, strlen (buf));
			printf ("Intra Card Message Sent\n");
			if (cdSuMdSendMsg (msg) == false)
			{
				CDSU_TRACE (1, "ApplicationStart1: Error sending message\n");
				cdSuFreeMsgBuf (msg);
			}else
			{
				CDSU_TRACE (1, "Sent message. Test[7] - SUCCESS\n");
			}

			// Exits Thread1
			//pthread_exit (&applThread1);
		}
		// exit (0);


	//} // while loop

}// applicationStart1

void* TestThread::applicationStart2 (void* dummy1)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdMessage* rxMsg;

	// Any Message that the Application receives.
	std::cout<<"-------------TEST CASE[11]--------------\n";
	//while (1)
	//{
		printf ("Waiting For Intra Card Message\n");
		if (cdSuMdRecvMsg (&rxMsg, applMid1) == true)
		{
			printf ("Intra Card Messge Recvd\n");
			CDSU_TRACE (5, "Msg Rxvd in MsgDistrTestI appl1. Test[11] - SUCCESS\n");
			
			CdSuIntraCardMsg* msg1 = (CdSuIntraCardMsg*) rxMsg;
			
			CDSU_TRACE1 (5, "msg.dataLen = %d\n", msg1->length);
			CDSU_TRACE1 (5, "msg.data = %s\n", msg1->buffer);
				
			cdSuFreeMsgBuf (rxMsg);
		
			applMid3.srvcHlrId.srvcType = 2;
			applMid3.srvcHlrId.appSrvcHlrId.appType = 2;
			applMid3.srvcHlrId.appSrvcHlrId.appId = 2;
			applMid3.srvcHlrId.appSrvcHlrId.appModuleId = 2;

			applMid3.hwUnitId.rackId = 2;
			applMid3.hwUnitId.unitId = 2;
			applMid3.hwUnitId.slotId = 2;
			applMid3.hwUnitId.procId = 2;
			
			char buf[4];
			//memset (&buf, 0, sizeof (buf));
			//buf[0]='a';
			//buf[1]='\0';
			
			strcpy(buf,"ack");
			CdSuIntraCardMsg* msg = (CdSuIntraCardMsg*) cdSuGetMsgBuf (sizeof (CdSuIntraCardMsg));
			msg->initMsg (applMid2, applMid1, buf, strlen (buf));
			printf ("Intra Card Message Sent\n");
			if (cdSuMdSendMsg (msg) == false)
			{
				CDSU_TRACE (1, "ApplicationStart1: Error sending message\n");
				cdSuFreeMsgBuf (msg);
			}else
			{
				CDSU_TRACE (1, "Sent message. Test[7] - SUCCESS\n");
			}

			// Exits Thread1
			//pthread_exit (&applThread1);
		}
		// exit (0);


	//} // while loop

}// applicationStart2


//============================================================================
// End of bukmsgthrd.cpp
//============================================================================
