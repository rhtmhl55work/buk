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

#ifdef __ECOS
char applThreadSpace [1024 * 16];
externC void init_all_network_interfaces ();
#endif

enum
{
	TESTER_THREAD_ONE_MOD_ID = 1,
	TESTER_THREAD_TWO_MOD_ID 
};

#define CDSU_UDP_TESTER2_LPN 8005

// CdModuleId applMid1;
CdModuleId applMid2;
CdModuleId applMid3;

void createApplications ();
// pthread_t applThread1;
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
	printf ("%s\n",__PRETTY_FUNCTION__);

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
	CDSU_TRACE (5, "Entered msgdistr2-test main ()\n");

#ifdef __ECOS
	init_all_network_interfaces ();
	sleep (3);
#endif

	// Create the Message Distributor.
	char ipAddress [4];
	ipAddress [0] = 192;
	ipAddress [1] = 168;
#ifdef __ECOS
	ipAddress [2] = 51;
	ipAddress [3] = 246;
#elif sun
	ipAddress [2] = 51;
	ipAddress [3] = 224;
#else
	ipAddress [2] = 16;
	ipAddress [3] = 72;
#endif

	Uint32 ipaddr;
	ipaddr = changeIpAddr (ipAddress);

	// Set the moduleId of the message distributor.
	CdModuleId selfMid;
	memset (&selfMid, 0, sizeof (selfMid));
	selfMid.hwUnitId.rackId = 2 ;
	selfMid.hwUnitId.unitId = 2;
	selfMid.hwUnitId.slotId = 2;
	selfMid.hwUnitId.procId = 2;
	
	CdSuMdInitParam param;
	param.selfMid = selfMid;
	param.ipaddr = ipaddr;
	param.udpLpn = CDSU_UDP_TESTER2_LPN ;

	// Create the Message Distributor.
	if (!cdSuMdInit (param))
	{
		printf ("Message Distributor Creation Failed\n");
		exit (-1);
	} else
	{
		printf ("Message Distributor Creation Success \n");	
	}
	
	//-------------------------------------------------------------------
	// Application I Module Id and Ipcp Info.
	//-------------------------------------------------------------------
/*
	memset (&applMid1, 0, sizeof (applMid1));
	
	applMid1.hwUnitId.rackId = 2 ;
	applMid1.hwUnitId.unitId = 2;
	applMid1.hwUnitId.slotId = 2;
	applMid1.hwUnitId.procId = 2;
	applMid1.srvcHlrId.srvcType = 1;
	applMid1.srvcHlrId.appSrvcHlrId.appType = 1;
	applMid1.srvcHlrId.appSrvcHlrId.appId = 1;
	applMid1.srvcHlrId.appSrvcHlrId.appModuleId = 1;

	
	if (cdSuMdRegisterMid (applMid1) == true)
	{
		printf (" Application I has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf (" Error:: Application I cannot be registered with MsgDistributor .\n");
	}

*/
	//-------------------------------------------------------------------
	// Application II Module Id.
	//-------------------------------------------------------------------
	memset (&applMid2, 0, sizeof (applMid2));

	applMid2.hwUnitId.rackId = 2 ;
	applMid2.hwUnitId.unitId = 2;
	applMid2.hwUnitId.slotId = 2;
	applMid2.hwUnitId.procId = 2;
	applMid2.srvcHlrId.srvcType = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appType = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appId = 2;
	applMid2.srvcHlrId.appSrvcHlrId.appModuleId = 2;



	if (cdSuMdRegisterMid (applMid2) == true)
	{
		printf ("Application II has been registered to MsgDistributor Successfully.\n");
	} else
	{
		printf ("Error:: Application II cannot be registered with MsgDistributor .\n");
	}

	createApplications ();

#ifndef __ECOS
	//	void** thread_return;
	//	pthread_join (applThread2, thread_return);
		sleep (10);
		pthread_cancel (applThread2);
		exit (0);
#else

		CdSuTsQueue <int> queue (5);	
		int i = 0;
		queue.popBlocking (i);
#endif

}

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
/*	
#ifdef __ECOS
	pthread_attr_t applAttr1;
	pthread_attr_init (&applAttr1);
	pthread_attr_setstackaddr (&applAttr1, (void*)&applThreadSpace [sizeof (applThreadSpace)]);
	pthread_attr_setstacksize (&applAttr1, sizeof (applThreadSpace));
	pthread_create (&applThread1, &applAttr1, applicationStart1, 0);

#elif sun
	pthread_attr_t applAttr;
	pthread_attr_init (&applAttr);
	pthread_attr_setscope (&applAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&applThread1, &applAttr, applicationStart1, 0);

#else
	pthread_create (&applThread1, 0, applicationStart1, 0);
#endif
*/

#ifdef __ECOS
	pthread_attr_t applAttr2;
	pthread_attr_init (&applAttr2);
	pthread_attr_setstackaddr (&applAttr2, (void*)&applThreadSpace [sizeof (applThreadSpace)]);
	pthread_attr_setstacksize (&applAttr2, sizeof (applThreadSpace));
	pthread_create (&applThread2, &applAttr2, applicationStart2, 0);

#elif __sun
	pthread_attr_t applAttr2;
	pthread_attr_init (&applAttr2);
	pthread_attr_setscope (&applAttr2, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&applThread2, &applAttr2, applicationStart2, 0);

#else
	pthread_create (&applThread2, 0, applicationStart2, 0);
#endif


} // createApplication ()

/*
//---------------------------------------------------------------------------
// applicationStart1 () 
// Description  : First application thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------
void* applicationStart1 (void*)
{
	printf ("%s\n", __PRETTY_FUNCTION__);
	CDSU_TRACE (5, "Entered Thread1\n");
	
	CdMessage* rxMsg;

	// Any Message that the Application receives.
	while (1)
	{
		if (cdSuMdRecvMsg (&rxMsg, applMid1) == true)
		{
			printf ("Msg Rxvd.\n");
			cdSuFreeMsgBuf (rxMsg);
			void* retVal;
			pthread_exit (retVal);
		}
	
	} // while loop

}// applicationStart1
*/

//---------------------------------------------------------------------------
// applicationStart2 () 
// Description  : Second application thread. 
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------
void* applicationStart2 (void*)
{
	printf ("%s\n", __PRETTY_FUNCTION__);

	CdMessage* rxMsg;
	
	// Any Message that the Application receives.
	while(1)
	{
		if (cdSuMdRecvMsg (&rxMsg, applMid2) == true)
		{
			// Receives Msg sent from msgdistr-test.
			CDSU_TRACE (5, "Intercard Msg Received.\n");
			MyTestMsg* myTestMsg = (MyTestMsg*) rxMsg;
			myTestMsg->ntoh ();
			CDSU_TRACE1 (5, "msg.a = %x\n", myTestMsg->a);
			CDSU_TRACE1 (5, "msg.b = %x\n", myTestMsg->b);
			CDSU_TRACE1 (5, "msg.c = %x\n", myTestMsg->c);
			CDSU_TRACE1 (5, "msg.dataLen = %d\n", myTestMsg->buflen);
			CDSU_TRACE1 (5, "msg.data = %d\n", myTestMsg->data);

			cdSuFreeMsgBuf (myTestMsg);
			//pthread_exit (&applThread2);

		}
			
	} // while loop
		
} // applicationStart2
