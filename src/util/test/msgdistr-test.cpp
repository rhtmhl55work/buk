//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: taskscheduler-test.cpp
// Description	: Tests for taskscheduler.
// Ref Docs	:
// Author	: Seema Chandak.
// Created On   : Thu Jun 19 10:26:09 IST 2003
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


#ifdef __ECOS
char applThreadSpace1 [1024 * 16];
char applThreadSpace2 [1024 * 16];
externC void init_all_network_interfaces ();
#endif

enum
{
	TESTER_THREAD_ONE_MOD_ID = 1,
	TESTER_THREAD_TWO_MOD_ID 
};

#define CDSU_UDP_TESTER1_LPN 8001

CdModuleId applMid1;
CdModuleId applMid2;
CdModuleId applMid3;
CdModuleId applMid4;

void createApplications ();
pthread_t applThread1;
pthread_t applThread2;
void* applicationStart1 (void*);
void* applicationStart2 (void*);

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
	
#ifdef __ECOS
	init_all_network_interfaces ();
	sleep (3);
	char c;
	printf ("Enter to Continue[Once Network Interface Up]\n");
	scanf ("%c", &c);
#endif
	// Create the Message Distributor.
	char ipAddress [4];
	ipAddress [0] = 192;
	ipAddress [1] = 168;
#ifdef __ECOS
	ipAddress [2] = 50;
	ipAddress [3] = 176;
#elif sun
	ipAddress [2] = 51;
	ipAddress [3] = 224;
#else
	ipAddress [2] = 16;
	ipAddress [3] = 72;
#endif
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

#ifdef __LINUX
	char remoteipAdd [4];
	remoteipAdd [0] = 192;
	remoteipAdd [1] = 168;
	remoteipAdd [2] = 16;
	remoteipAdd [3] = 161;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;

#elif sun
	char remoteipAdd [4];
	remoteipAdd [0] = 192;
	remoteipAdd [1] = 168;
	remoteipAdd [2] = 51;
	remoteipAdd [3] = 224;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;


#else
	char remoteipAdd [4];
	remoteipAdd [0] = 192;
	remoteipAdd [1] = 168;
	remoteipAdd [2] = 16;
	remoteipAdd [3] = 32;
	Uint32 remoteipadd = changeIpAddr (remoteipAdd);
	ipcpInfo.u.remoteUdpInfo.ipaddr = remoteipadd;

#endif

	ipcpInfo.u.remoteUdpInfo.portNo = 8005;
	std::cout<<"-------------TEST CASE[5]--------------\n";

	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Destination HwUnit registered successfully. Test[5] SUCCESS\n");
	else
		CDSU_TRACE (1, "Destination HwUnit registration fails\n");
		
	std::cout<<"-------------TEST CASE[6]--------------\n";
	

	if (cdSuMdRegisterHwUnit (hwUnitId, ipcpInfo) == true)
		CDSU_TRACE (5, "Destination HwUnit registered successfully.\n");
	else
		CDSU_TRACE (1, "Destination HwUnit registration fails. Test[6] - SUCCESS \n");

	


	//-------------------------------------------------------------------
	// Application I Module Id and Ipcp Info.
	//-------------------------------------------------------------------
	memset (&applMid1, 0, sizeof (applMid1));
	
	std::cout<<"-------------TEST CASE[2]--------------\n";

	if (cdSuMdRegisterMid (applMid1) == true)
	{
		printf (" Application I has been registered to MsgDistributor Successfully. \n");
	} else
	{
		printf (" Error:: Application I cannot be registered with MsgDistributor . Test[2] - SUCCESS \n");
	}


	applMid1.srvcHlrId.srvcType = 1;
	applMid1.srvcHlrId.appSrvcHlrId.appType = 1;
	applMid1.srvcHlrId.appSrvcHlrId.appId = 1;
	applMid1.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	applMid1.hwUnitId.rackId = 1 ;
	applMid1.hwUnitId.unitId = 1;
	applMid1.hwUnitId.slotId = 1;
	applMid1.hwUnitId.procId = 1;
	std::cout<<"-------------TEST CASE[1]--------------\n";

	if (cdSuMdRegisterMid (applMid1) == true)
	{
		printf (" Application I has been registered to MsgDistributor Successfully. Test[1] - SUCCESS \n");
	} else
	{
		printf (" Error:: Application I cannot be registered with MsgDistributor . \n");
	}
	std::cout<<"-------------TEST CASE[3]--------------\n";

	if (cdSuMdUnregisterMid (applMid1) == true)
	{
		printf (" Application I has been Unregistered to MsgDistributor Successfully. Test[3] SUCCESS\n");
	} else
	{
		printf (" Error:: Application I cannot be Unregistered with MsgDistributor .\n");
	}
	
	std::cout<<"-------------TEST CASE[4]--------------\n";

	if (cdSuMdUnregisterMid (applMid4) == true)
	{
		printf (" Application IV has been Unregistered to MsgDistributor Successfully. \n");
	} else
	{
		printf (" Error:: Application IV cannot be Unregistered with MsgDistributor . Test[4] - SUCCESS\n");
	}




	if (cdSuMdRegisterMid (applMid1) == true)
	{
		printf (" Application I has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: Application I cannot be registered with MsgDistributor .\n");
	}
	//-------------------------------------------------------------------
	// Application II Module Id.
	//-------------------------------------------------------------------
	memset (&applMid2, 0, sizeof (applMid2));

	applMid2.srvcHlrId.srvcType = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appType = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appId = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appModuleId = 2;


	applMid2.hwUnitId.rackId = 1 ;
	applMid2.hwUnitId.unitId = 1;
	applMid2.hwUnitId.slotId = 1;
	applMid2.hwUnitId.procId = 1;

	if (cdSuMdRegisterMid (applMid2) == true)
	{
		printf ("Application II has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf ("Error:: Application II cannot be registered with MsgDistributor .\n");
	}

	createApplications ();
	
	#if 0
	int ch;	
	printf ("Enter 0 to EXIT \n");
	scanf ("%d", &ch);
	if (ch == 0)
	{
		exit (0);
	
	}	
       #endif
	
	while (1)
	{
#ifndef __ECOS 
		void* thread_return;
		pthread_join (applThread1, &thread_return);
		pthread_join (applThread2, &thread_return);
		cdSuMdTerminate ();
		exit (0);
#else

		CdSuTsQueue <int> queue (5);	
		int i = 0;
		queue.popBlocking (i);
#endif
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
	
#ifdef __ECOS
	pthread_attr_t applAttr1;
	pthread_attr_init (&applAttr1);
	pthread_attr_setstackaddr (&applAttr1, (void*)&applThreadSpace1 [sizeof (applThreadSpace1)]);
	pthread_attr_setstacksize (&applAttr1, sizeof (applThreadSpace1));
	printf ("Creating Thread 1\n");
	if (pthread_create (&applThread1, &applAttr1, applicationStart1, 0) == 0)
	{
		printf ("Thread 1 Created Successfully\n");	
	}

#elif sun
	pthread_attr_t applAttr;
	pthread_attr_init (&applAttr);
	pthread_attr_setscope (&applAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&applThread1, &applAttr, applicationStart1, 0);

#else
	pthread_create (&applThread1, 0, applicationStart1, 0);
#endif

#ifdef __ECOS
	pthread_attr_t applAttr2;
	pthread_attr_init (&applAttr2);
	pthread_attr_setstackaddr (&applAttr2, (void*)&applThreadSpace2 [sizeof (applThreadSpace2)]);
	pthread_attr_setstacksize (&applAttr2, sizeof (applThreadSpace2));
	printf ("Creating Thread 2\n");
	if (pthread_create (&applThread2, &applAttr2,
		applicationStart2, 0) == 0)
	{
		printf ("Thread2 Created Successfully\n");
	}
#elif sun
	pthread_attr_t applAttr2;
	pthread_attr_init (&applAttr2);
	pthread_attr_setscope (&applAttr2, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&applThread2, &applAttr2, applicationStart2, 0);

#else
	pthread_create (&applThread2, 0, applicationStart2, 0);
#endif


} // createApplication ()

//---------------------------------------------------------------------------
// applicationStart1 () 
// Description  : First application thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------
void* applicationStart1 (void* dummy1)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdMessage* rxMsg;
	CdMessage* rxMsg1;
	std::cout<<"-------------TEST CASE[12]--------------\n";
	if (cdSuMdRecvMsg (&rxMsg1, applMid4) == true)
		{
			printf ("Intra Card Messge Recvd\n");
			CDSU_TRACE (5, "Msg Rxvd in MsgDistrTestI appl4.\n");
			cdSuFreeMsgBuf (rxMsg1);
		}
	else 
		{
			CDSU_TRACE (5, "Unable to receive from unregistered module. Test[12] - SUCCESS\n");
		}

	// Any Message that the Application receives.
	std::cout<<"-------------TEST CASE[11]--------------\n";
	while (1)
	{
		printf ("Waiting For Intra Card Message\n");
		if (cdSuMdRecvMsg (&rxMsg, applMid1) == true)
		{
			printf ("Intra Card Messge Recvd\n");
			CDSU_TRACE (5, "Msg Rxvd in MsgDistrTestI appl1. Test[11] - SUCCESS\n");
			cdSuFreeMsgBuf (rxMsg);
			
			// Exits Thread1
			pthread_exit (&applThread1);
		}
		// exit (0);
	
	} // while loop

}// applicationStart1

//---------------------------------------------------------------------------
// applicationStart2 () 
// Description  : Second application thread. 
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------

void* applicationStart2 (void* dummy2)
{
	CDSU_TRACE (2, "Application Start 2\n");

	char buf [25];
	for (int i = 0; i < 25; ++i)
	{
		buf [i] = 'x';
	}

	std::cout<<"-------------TEST CASE[7]--------------\n";
	
	CdSuIntraCardMsg* msg = (CdSuIntraCardMsg*) cdSuGetMsgBuf (
					sizeof (CdSuIntraCardMsg));
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
	
	std::cout<<"-------------TEST CASE[8 & 10]--------------\n";

	CdSuIntraCardMsg* msg2 = (CdSuIntraCardMsg*) cdSuGetMsgBuf (
					sizeof (CdSuIntraCardMsg));
	msg2->initMsg (applMid4, applMid2, buf, strlen (buf));
	printf ("Intra Card Message Sent\n");
	if (cdSuMdSendMsg (msg2) == false)
	{
		CDSU_TRACE (1, "ApplicationStart2: Error sending message. Test[8,10] - SUCCESS\n");
		cdSuFreeMsgBuf (msg2);
	}else
	{
		CDSU_TRACE (1, "Sent message.\n");
	}



	memset (&applMid3, 0, sizeof (applMid3));

	applMid3.hwUnitId.rackId = 2 ;
	applMid3.hwUnitId.unitId = 2;
	applMid3.hwUnitId.slotId = 2;
	applMid3.hwUnitId.procId = 2;
	applMid3.srvcHlrId.srvcType = 2;
	applMid3.srvcHlrId.appSrvcHlrId.appType = 2;
	applMid3.srvcHlrId.appSrvcHlrId.appId = 2;
	applMid3.srvcHlrId.appSrvcHlrId.appModuleId = 2;
	
	MyTestMsg* myTestMsg = (MyTestMsg*) cdSuGetMsgBuf (sizeof (MyTestMsg));
	myTestMsg->initMsg (applMid3, applMid2, 2345, "Seema Chandak", 
			 strlen ("Seema Chandak"));

	CDSU_TRACE1 (5, "Length of msg being sent is %d\n", myTestMsg->msgHdr.msgLen);

	myTestMsg->hton ();
	std::cout<<"-------------TEST CASE[9]--------------\n";

	if (cdSuMdSendMsg (myTestMsg) == false)
	{
		CDSU_TRACE (1, "applicationStart2: Error sending message\n");
		cdSuFreeMsgBuf (myTestMsg);
	}else
	{
		CDSU_TRACE (1, "Sent Msg. Test[9] - SUCCESS\n");

	}
	//sleep (10);
	
	// Exits thread2
	pthread_exit (&applThread2);
		
} // applicationStart2

//============================================================================
// End of msgdistr-test.cpp
//============================================================================
