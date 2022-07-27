//==============================================================================
// BUK assignment - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: bukmsgthrd2.cpp
// Description	: Tests for taskscheduler.
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


#define CDSU_UDP_TESTER1_LPN 50002

//two applications for sending and receiving message
CdModuleId applMid1;
CdModuleId applMid2;

// for creating applications (threads)
void createApplications ();

//using CdSuThread 
class TestThread:public CdSuThread
{
public:
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
	selfMid.hwUnitId.rackId = 2 ;
	selfMid.hwUnitId.unitId = 2;
	selfMid.hwUnitId.slotId = 2;
	selfMid.hwUnitId.procId = 2;
	
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

	hwUnitId.rackId = 1;
	hwUnitId.unitId = 1;
	hwUnitId.slotId = 1;
	hwUnitId.procId = 1;

	CdSuMdIpcpInfo ipcpInfo;
	ipcpInfo.ipcpMech = CDSU_IPCP_MECH_UDP;

	char remoteipAdd [4];
	remoteipAdd [0] = 192;
	remoteipAdd [1] = 168;
	remoteipAdd [2] = 16;
	remoteipAdd [3] = 129;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;

	ipcpInfo.u.remoteUdpInfo.portNo = 50001;
	std::cout<<"-------------TEST CASE[5]--------------\n";

	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Destination HwUnit registered successfully. Test[5] SUCCESS\n");
	else
		CDSU_TRACE (1, "Destination HwUnit registration fails\n");
		

	//-------------------------------------------------------------------
	// Application II Module Id.
	//-------------------------------------------------------------------
	memset (&applMid2, 0, sizeof (applMid2));

	applMid2.srvcHlrId.srvcType = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appType = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appId = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appModuleId = 2;


	applMid2.hwUnitId.rackId = 2 ;
	applMid2.hwUnitId.unitId = 2;
	applMid2.hwUnitId.slotId = 2;
	applMid2.hwUnitId.procId = 2;

	if (cdSuMdRegisterMid (applMid2) == true)
	{
		printf ("Application II has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf ("Error:: Application II cannot be registered with MsgDistributor .\n");
	}


	createApplications ();
	
	/*while (1)
	{
		CdSuTsQueue <int> queue (5);	
		int i = 0;
		queue.popBlocking (i);
	}*/
	
	//while (1)
	//{
	//	; // Intentional
	//}

	sleep(30);

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
	
	TestThread* testThread2 = new TestThread;
	testThread2->spawn (TestThread::applicationStart2);

} // createApplication ()

//---------------------------------------------------------------------------
// applicationStart2 () 
// Description  : Second application thread. 
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------
//thread id: testThread2 : applMid2
void* TestThread::applicationStart2 (void* dummy2)
{
	CdMessage* rxMsg;

	CDSU_TRACE (2, "Application Start 2\n");

	char buf [25];
	for (int i = 0; i < 25; ++i)
	{
		buf [i] = 'x';
	}

	std::cout<<"-------------TEST CASE[7]--------------\n";
	
	applMid1.srvcHlrId.srvcType = 6;
	applMid1.srvcHlrId.appSrvcHlrId.appType = 6;
	applMid1.srvcHlrId.appSrvcHlrId.appId = 6;
	applMid1.srvcHlrId.appSrvcHlrId.appModuleId = 6;

	applMid1.hwUnitId.rackId = 1 ;
	applMid1.hwUnitId.unitId = 1;
	applMid1.hwUnitId.slotId = 1;
	applMid1.hwUnitId.procId = 1;

	CdSuIntraCardMsg* msg = (CdSuIntraCardMsg*) cdSuGetMsgBuf (sizeof (CdSuIntraCardMsg));
	msg->initMsg (applMid1, applMid2, buf, strlen (buf));
	printf ("Intra Card Message Sent\n");
	if (cdSuMdSendMsg (msg) == false)
	{
		CDSU_TRACE (1, "ApplicationStart2: Error sending message\n");
		cdSuFreeMsgBuf (msg);
	}else
	{
		CDSU_TRACE (1, "Sent message. Test[7] - SUCCESS\n");
	}
	
	if (cdSuMdRecvMsg (&rxMsg, applMid2) == true)
	{
		printf ("Intra Card Messge Recvd\n");
		CDSU_TRACE (5, "Msg Rxvd in MsgDistrTestI appl2. Test[11] - SUCCESS\n");
		
		//memset(&msg, 0, strlen(msg));
		
		CdSuIntraCardMsg* msg1 = (CdSuIntraCardMsg*) rxMsg;
		CDSU_TRACE1 (5, "msg.dataLen = %d\n", msg1->length);
		CDSU_TRACE1 (5, "msg.data = %s\n", msg1->buffer);
		
		//std::cout<<"rcvd msg: "<<msg1->buffer<<"\n";
		
		cdSuFreeMsgBuf (rxMsg);
		
		// Exits Thread1
		//pthread_exit (&applThread1);
	}

//	std::cout<<"-------------TEST CASE[8 & 10]--------------\n";

	//sleep (10);
	
	// Exits thread2
	//pthread_exit (&applThread2);
		
} // applicationStart2

//============================================================================
// End of msgdistr-test.cpp
//============================================================================
