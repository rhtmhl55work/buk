//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testserver.cpp
// Description  : Test server
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================
#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/thread.inl>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "testmenu.h"
#include "timerdef.h"
#include "cdsutrace.h"
//#include "mgctypes.h"
#include "cdsumdapi.h"
//#include "ss7macdb.h"
//#include "ss7cardmacmgr.h"
//#include "tdmIf.h"

#define TARGET_IP_ADDR "192.168.16.72"
#define TARGET_IP_PORT 9555

//----------------------------------------------------------------------------
// Globals 
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Externs 
//----------------------------------------------------------------------------
extern CdSuTimerHandler* cdSuTimerHandler;
//----------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------
#ifdef __ECOS
externC void init_all_network_interfaces ();
void startResourceMonitoring ();
#endif
Uint32 changeIpaddr (char ipaddr []);

// Takes start trigger from operator.
Uint32 getStartTrigger ();

//----------------------------------------------------------------------------
// Function	: main 
// Description	: Main function
// Inputs	: argc, argv
// Return Value	: int
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------
int main (int argc, char* argv [])
{
#ifdef __ECOS
	// Initialize eth2 on GII card.
	// TODO: make sure this shifts to eCos.

	//Uint32* cr2 = (Uint32*)0x40000008;
	//*cr2 = 0x005f2030;
	init_all_network_interfaces ();
#endif
	// sleep (3);

		
	sigset_t set;
	sigfillset (&set);
	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
	{
		CDSU_TRACE (1, "signal mask error\n");
		return -1;
	}
	
	CdSuTrace::setLevel (1);
	
	//----------------------------------------------------------------------
	// Initialize the MD.
	//----------------------------------------------------------------------
	CdSuMdInitParam param;
	//CdModuleId mid;
	memset (&param.selfMid, 0, sizeof (CdModuleId));
	CdHardwareUnitId hid = {0,0,1,0};
	char ipaddr [] = {192,168,16,72};
	param.selfMid.hwUnitId = hid;
	param.ipaddr = changeIpaddr (ipaddr);
	param.udpLpn = 9701; 
	cdSuMdInit (param);
/*
	//---------------------------------------------------------------------------
	// Register the other h/w unit (MGC) also. 
	//---------------------------------------------------------------------------
	char ipaddr1 [] = {192,168,51,209};
	gMgcHwUnitId1.rackId = 0;
	gMgcHwUnitId1.unitId = 0;
	gMgcHwUnitId1.slotId = 0;
	gMgcHwUnitId1.procId = 0;

	CdSuMdIpcpInfo info;
	info.ipcpMech = CDSU_IPCP_MECH_UDP;
	info.u.remoteUdpInfo.ipaddr = changeIpaddr (ipaddr1);
	info.u.remoteUdpInfo.portNo = 9700; 
	cdSuMdRegisterHwUnit (gMgcHwUnitId1, info);
*/

	// block all signals.
	//--------------------------------------------------------------------
	// NOTE : If you are running This Code Over Target Then 
	// Block the signals, So Uncomment The Following
	//--------------------------------------------------------------------
/*		
	sigset_t set;
	sigfillset (&set);
	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
	{
		CDSU_TRACE (1, "signal mask error\n");
		return -1;
	}
*/	
	// create timer handler
	if (cdCreateTimerHandler () < 0)
	{
		CDSU_TRACE (1, "create timer handler error\n");
		return -1;
	}

	// create timer thread	
	if (cdCreateTimerThread () < 0)
	{
		CDSU_TRACE (1, "create timer thread error\n");
		return -1;
	} 
	
#ifdef __ECOS
	//--------------------------------------------------------------------
	// Starting the resource monitor.
	//--------------------------------------------------------------------
	startResourceMonitoring ();
#endif


	// wait for connection from test client
	int testSock = 0;
#ifdef TEST_INPUT_THRU_SOCK
	testSock = cdCreateTestConn ();
	if (testSock < 0)
	{
		CDSU_TRACE (1, "create test connection error\n");
		return -1;
	}
#endif

	if (getStartTrigger () != 1)
	{
		return 0;
	}


//--------------------------< BEGIN >-----------------------------------------
// TODO: Add initialization code here
//---------------------------< END >------------------------------------------
/*
	//--------------------------------------------------------------------
	// TDM initialization.
	//--------------------------------------------------------------------
	TdmInterface *tif = new TdmInterface;
	tif->resetTimeSwitch();
	tif->setE1InNormalMode(0);
	tif->enableCcs(0);
	tif->programTimeSwitch(TdmInterface::TIF_SIGNALING);

	// Create the SS7 MAC DB.
	Ss7MacDb* ss7MacDb = new Ss7MacDb;

	// Wait for operator to say start.
	if (getStartTrigger () != 1)
	{
		return 0;
	}

	// Create the Card Mac Manager.
	Ss7CardMacMgr* cardMgr = new Ss7CardMacMgr (ss7MacDb);
	cardMgr->run ();
	*/
	
	
	// Run Receiver Thread.
	// cdCreateReceiverThread ();
	// execute the tests
	cdExecuteTests (testSock);

	//cardMgr->join (NULL);		 
	
//--------------------------< BEGIN >-----------------------------------------
// TODO: Add clean-up code here
//---------------------------< END >------------------------------------------
#ifdef TEST_INPUT_THRU_SOCK
	// close the connection
	if (cdCloseTestConn (testSock) < 0)
	{
		CDSU_TRACE (1, "close test connection error\n");
	}
#endif

	// delete timer thread
	if (cdDeleteTimerThread () < 0)
	{
		CDSU_TRACE (1, "delete timer thread error\n");
	}

	// delete timer handler
	if (cdDeleteTimerHandler () < 0)
	{
		CDSU_TRACE (1, "delete timer handler error\n");
	}

	return 0;
} // main

//----------------------------------------------------------------------------
// Function    : changeIpaddr
// Description : Change ip address from char [] to Uint32 
//----------------------------------------------------------------------------
#if 1
Uint32 changeIpaddr (char ipaddr [])
{
	Uint32 ipa;
	char* p = (char*)&ipa;
	p[0] = ipaddr [0];
	p[1] = ipaddr [1];
	p[2] = ipaddr [2];
	p[3] = ipaddr [3];
	return ipa;

} // changeIpaddr ()
#endif
#if 1
#ifdef __ECOS

//---------------------------------------------------------------------------
// Name		: startResourceMonitoring ()
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    : 
// then divide it 
//---------------------------------------------------------------------------
// extern void rsrcMonitorFunc (const CdSuTimeout &);

void startResourceMonitoring ()
{
	CdSuTimerInfo timerInfo;
	timerInfo.timerType = CDSU_TMR_TT_CYCLIC;
	timerInfo.opcode = 2;//MGC_SS7_BASIC_TIMEOUT_MSG;TODO
	timerInfo.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	//timerInfo.timerCallBackFn = rsrcMonitorFunc;

	timerInfo.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo.timeCounts = 1;

	CdSuTimerId timerId;
	CdSuTmrRetVal ret = cdSuTimerHandler->startTimer (timerId, timerInfo);
	if (ret == CDSU_TMR_RV_SUCCESS)
	{
		//CDPRINTF ("Resource Monitoring started!\n");
	}
} // startResourceMonitoring ()
#endif
#endif

#if 1
//---------------------------------------------------------------------------
// Name		: getStartTrigger ()
// Description  : Gets start trigger from operator.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    : 
// then divide it 
//---------------------------------------------------------------------------

Uint32 getStartTrigger ()
{
	// create socket.
	int sockfd, newsock;
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 0;
	} 

	// Bind socket.
	struct sockaddr_in serveraddr, cli_addr;
	memset (&serveraddr, 0, sizeof (serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons (TARGET_IP_PORT);
	serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*) &serveraddr, sizeof (serveraddr)) < 0)
	{
		CDSU_TRACE (1, "bind failure\n");
		return 0;
	} 

	// Lesten
	if ((listen (sockfd, 5)) < 0)
	{
		CDSU_TRACE (1, "listen failure\n");
		return 0;
	} 

	// Accept connection.
	#ifdef sun
		int clilen = sizeof (cli_addr);
	#else
		socklen_t clilen = sizeof (cli_addr);
	#endif
	
	CDSU_TRACE (1, "Waiting for test input connection\n");
	if ((newsock = accept (sockfd, (sockaddr*) &cli_addr, &clilen)) < 0)
	{
		CDSU_TRACE (1, "accept error\n");
		return 0;
	} else 
	{
		CDSU_TRACE (1, "connection established\n");
	}

	// Read from socket.
	Uint32 choice;
	if (read (newsock, (char*)&choice, sizeof (int)) < 0)
	{
		CDSU_TRACE (1, "read error\n");
		return 0;
	}
	else
	{
		choice = ntohl (choice);
		return choice;
	}
}
#endif
//============================================================================
// <End of testserver.cpp>
//============================================================================
