//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testmenu.cpp
// Description  : Test menu implementation file
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================
#include <stdio.h>
#ifdef TEST_INPUT_THRU_SOCK
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#endif

#include "testmenu.h"
#include "cdsutrace.h"
#include "testfunctions.h"
#ifdef TEST_SERVER
CdTestCases cdTests [] = {
{"Print menu"			,		cdPrintTestMenu		},
{"Set trace level: 0"		,		cdSetTraceLevel0	},
{"Set trace level: 1"		,		cdSetTraceLevel1	},
{"Set trace level: 2"		,		cdSetTraceLevel2	},
{"Set trace level: 3"		,		cdSetTraceLevel3	},
{"Set trace level: 4"		,		cdSetTraceLevel4	},
{"Set trace level: 5"		,		cdSetTraceLevel5	},
{"Initialize Test setup"	,		initSetup		},
{"Set Invalid Instance ID"	,		setInvNhInstId		},
{"Set Instance ID More Than Max",		setInstIdMoreThanMax 	},
{"Set Invalid SM Actor ID"	,		setInvSmActorId		},
{"Set Invalid Appl Actor ID"	,		setInvApplActorId	},
{"Set Invalid NH Actor ID"	,		setInvNhActorId		},
{"Set Buffer Length 0"		,		setBufLenZero		},
{"Set Buffer Length More Than Max",		setBufLenMoreThanMax	},
{"Set Select Timer to 0"	,		setSelTimerZero		},
{"Set Select Timer to More Than Max",		setSelTimerMoreThanMax	},
{"Set No Hlr Timer to 0" 	, 		setNoHlrTimerZero	},
{"Set No Hlr Timer to More Than Max", 		setNoHlrTimerMoreThanMax},
{"Set No Of Udp Port More Than Max", 		setNoOfUdpPortMoreThanMax},
{"Set Local Port to 0"		,		setLocalPortZero	},
{"Dont Set Local Port"		,		dontSetLocalPort	},
{"Set Dest Port to 0"		,		setDestPortZero		},
{"Set Dest Port Correct One"	,		setDestPort		},
{"Set Dest Port Different One"	,		setDestPortDiff		},
{"Set Dest Port To Self Port"	,		setDestPortSelf		},
{"Set Invalid Port Id"		,		setInvPortId		},
{"Set Invalid Data Pointer"	,		setInvDataPtr		},
{"Set Data Len to 0"		,		setDataLenZero		},
{"Set Diff Local Address" 	,		setLocalAddrDif		},
{"Set Local Address = Dest Addr",		setLocalAddrToDest	},
{"Set Diff Dest Address"	,		setDestAddrDif		},
{"Set No Ack Req"		,		setNoAckReq		},
{"Display Parameter Setting"	,		displayParamSetting	},
{"Init UDP NH"			,		initUdpNh		},
{"Terminate UDP NH"		,		terminateUdpNh		},
{"Delete UDP NH"		,		deleteUdpNh		},
{"Ctrl Op with Invalid Opcode"	,		invCtrlUdpNh		},
{"Config Op with Invalid Opcode",		invalidConfig		},
{"Initial Configure"		, 		initialConfig		},
{"General Configure"		, 		generalConfig		},
{"Start UDP NH"			,		startUdpNh		},
{"Stop UDP NH"			,		stopUdpNh		},
{"Get Status of UDP NH"		,		statusReqUdpNh		},
{"Get Statistics of UDP NH"	,		statsReqUdpNh		},
{"Add Udp Port"			,		addUdpPort		},
{"Rem Udp Port"			,		remUdpPort		},
{"Send Data "			,		sendData		},
{"Send Data over always opened port",	sendDataAllOpened	},
{"Statitics Test"			,		statTest 		},
{"Statitics Test Remote SetUp"	,	statRemoteSetUp	 		},
//--------------------------< BEGIN >-----------------------------------------
// TODO: Add test cases here, format { "Name of test" , testFunction },
//{"" },

//---------------------------< END >------------------------------------------

{0				,		0			}
};
#else
CdTestCases cdTests [] = {
{"Print menu"			,		cdPrintTestMenu		},
{"Set trace level: 0"		,		cdSetTraceLevel0	},
{"Set trace level: 1"		,		cdSetTraceLevel1	},
{"Set trace level: 2"		,		cdSetTraceLevel2	},
{"Set trace level: 3"		,		cdSetTraceLevel3	},
{"Set trace level: 4"		,		cdSetTraceLevel4	},
{"Set trace level: 5"		,		cdSetTraceLevel5	},
{"Initialize Test setup"	,		initSetup		},
{"Set Invalid Instance ID"	,		setInvNhInstId		},
{"Set Instance ID More Than Max",		setInstIdMoreThanMax 	},
{"Set Invalid SM Actor ID"	,		setInvSmActorId		},
{"Set Invalid Appl Actor ID"	,		setInvApplActorId	},
{"Set Invalid NH Actor ID"	,		setInvNhActorId		},
{"Set Buffer Length 0"		,		setBufLenZero		},
{"Set Buffer Length More Than Max",		setBufLenMoreThanMax	},
{"Set Select Timer to 0"	,		setSelTimerZero		},
{"Set Select Timer to More Than Max",		setSelTimerMoreThanMax	},
{"Set No Hlr Timer to 0" 	, 		setNoHlrTimerZero	},
{"Set No Hlr Timer to More Than Max", 		setNoHlrTimerMoreThanMax},
{"Set No Of Udp Port More Than Max", 		setNoOfUdpPortMoreThanMax},
{"Set Local Port to 0"		,		setLocalPortZero	},
{"Dont Set Local Port"		,		dontSetLocalPort	},
{"Set Dest Port to 0"		,		setDestPortZero		},
{"Set Dest Port Correct One"	,		setDestPort		},
{"Set Dest Port Different One"	,		setDestPortDiff		},
{"Set Dest Port To Self Port"	,		setDestPortSelf		},
{"Set Invalid Port Id"		,		setInvPortId		},
{"Set Invalid Data Pointer"	,		setInvDataPtr		},
{"Set Data Len to 0"		,		setDataLenZero		},
{"Set Diff Local Address" 	,		setLocalAddrDif		},
{"Set Local Address = Dest Addr",		setLocalAddrToDest	},
{"Set Diff Dest Address"	,		setDestAddrDif		},
{"Display Parameter Setting"	,		displayParamSetting	},
{"Init UDP NH"			,		initUdpNh		},
{"Terminate UDP NH"		,		terminateUdpNh		},
{"Delete UDP NH"		,		deleteUdpNh		},
{"Ctrl Op with Invalid Opcode"	,		invCtrlUdpNh		},
{"Config Op with Invalid Opcode",		invalidConfig		},
{"Initial Configure"		, 		initialConfig		},
{"General Configure"		, 		generalConfig		},
{"Start UDP NH"			,		startUdpNh		},
{"Stop UDP NH"			,		stopUdpNh		},
{"Get Status of UDP NH"		,		statusReqUdpNh		},
{"Get Statistics of UDP NH"	,		statsReqUdpNh		},
{"Add Udp Port"			,		addUdpPort		},
{"Rem Udp Port"			,		remUdpPort		},
{"Set No Ack Req"		,		setNoAckReq		},
{"Send Data "			,		sendData		},
{"Send Data over always opened port",	sendDataAllOpened	},
{"Statitics Test"			,		statTest 		},
{"Statitics Test Remote SetUp"	,	statRemoteSetUp	 		},

//--------------------------< BEGIN >-----------------------------------------
// TODO: Add test cases here, format { "Name of test" , 0 },

};
#endif

//----------------------------------------------------------------------------
// Function	: cdGetNumTests
// Description	: get the number of test cases
// Inputs	: None.
// Return Value	: numtests
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

int cdGetNumTests ()
{
	// compute the number of tests
	int numTests = 0;
	while (cdTests[numTests].testName != 0)
	{
		++numTests;
	}
	return numTests;
}

//----------------------------------------------------------------------------
// Function	: cdGetNumTests
// Description	: get the number of test cases
// Inputs	: None.
// Return Value	: numtests
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdPrintTestMenu ()
{
	printf ("========================================================\n");
	for (int i = 0; cdTests[i].testName != 0 ; ++i)
	printf ("(%d) %s\n", i, cdTests[i].testName);
	printf ("--------------------------------------------------------\n");
}

//----------------------------------------------------------------------------
// Function	: cdCreateTestConn
// Description	: create socket and wait for connection from test client
// Inputs	: None.
// Return Value	: sockfd on success and -1 on failure
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

#ifdef TEST_INPUT_THRU_SOCK
int cdCreateTestConn ()
{
	int sock, testSock;
	struct sockaddr_in servAddr, cliAddr;

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		CDSU_TRACE (1, "socket failure\n");
		return -1;
	} 
	
	memset (&servAddr, 0, sizeof (servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons (CDTEST_SOCK_PORT);
	servAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	if (bind (sock, (struct sockaddr*) &servAddr, sizeof (servAddr)) < 0)
	{
		CDSU_TRACE (1, "bind failure\n");
		return -1;
	} 

	if ((listen (sock, 5)) < 0)
	{
		CDSU_TRACE (1, "listen failure\n");
		return -1;
	} 

	socklen_t cliLen = sizeof (cliAddr);
	CDSU_TRACE (3, "Waiting for connection\n");
	if ((testSock = accept (sock, (sockaddr*) &cliAddr, &cliLen)) < 0)
	{
		CDSU_TRACE (1, "accept error\n");
		return -1;
	} else 
	{
		CDSU_TRACE (3, "Connection established\n");
	}

	close (sock);
	return testSock;
} // cdCreateTestConn
#endif

//----------------------------------------------------------------------------
// Function	: cdExecuteTests
// Description	: get the choice and executes the test functions
// Inputs	: None.
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdExecuteTests (int testSock)
{
	int choice;
	// get number of tests
	int numTests = cdGetNumTests ();

	bool isConnEst = true;
	while (isConnEst)
	{
		// get test choice
#ifdef TEST_INPUT_THRU_SOCK
		choice = cdGetChoiceFromSock (testSock);
#else

		cdPrintTestMenu ();
		printf ("Enter your choice; 0 for menu; 255 to exit : ");
		scanf ("%d", &choice);
#endif
		if (choice == CDTEST_END_NUM) 
		{
			isConnEst = false;
		}
		if ( (choice >=0) && (choice < numTests) )
		{
			cdTests[choice].testFn ();
		} else
		{
			printf ("Wrong choice! \n");
		}
	}
} // cdExecuteTests

//----------------------------------------------------------------------------
// Function	: cdGetChoiceFromSock
// Description	: get the choice from socket
// Inputs	: sock fd
// Return Value	: choice
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

#ifdef TEST_INPUT_THRU_SOCK
int cdGetChoiceFromSock (int testSock)
{
	int choice;
	if (read (testSock, (char*)&choice, sizeof (int)) < 0)
	{
		CDSU_TRACE (1, "read error\n");
		return -1;
	}
	else
	{
		choice = ntohl (choice);
		return choice;
	}	
} // cdGetChoiceFromSock
#endif

//----------------------------------------------------------------------------
// Function	: cdCloseTestConn
// Description	: close the test connection
// Inputs	: sock fd
// Return Value	: -1 on error, 1 on success
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

#ifdef TEST_INPUT_THRU_SOCK
int cdCloseTestConn (int testSock)
{
	if (close (testSock) < 0)
	{
		CDSU_TRACE (1, "close failur\n");
		return -1;
	} else
	{
		return 1;
	}
} // cdCloseTestConn
#endif


//============================================================================
// <End fo testmenu.cpp>
//============================================================================


