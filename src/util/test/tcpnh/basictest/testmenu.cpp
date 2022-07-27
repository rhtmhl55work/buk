//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testmenu.cpp
// Description  : Test menu implementation file
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified: By Sumit Bijwe for TCP NH Testing.
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
{"Create Appl Thrd "		,		createULThr	        },
{"Create SysMgmt thrd "		,		createSMThr             },
{"Create TCP NH"		,		createTcpNh 		},
{"Configure  TCP NH"		,		configTcpNh		},
{"Start TCP NH"			,		startTcpNh		},
{"Stop TCP NH"			,		stopTcpNh		},
{"Delete TCP NH"		,		deleteTcpNh		},
{"Terminate TCP NH"		,		terminateTcpNh		},
{"Status TCP NH"		,		statusReqTcpNh		},
{"Statistic TCP NH"		,		statsReqTcpNh		},
{"Add Listener"			,		addListener		},
{"Open Connection"		,		connectReq		},
{"Open Connection Load Testing" ,		connectReqLoadTesting	},
{"Send Data"			,		sendData		},
{"Send Data Load Testing"	,		sendDataLoadTesting	},
{"Remove Listener"		,		remListener		},
{"Close Connection"		,		closeConReq		},
{"Close Connection Load Testing",		closeConReqLoadTesting	},
{"Set Invalid NHTCP Instance Id",		setInvNhInstId		},
{"Print Stats			",		printStats		},
{"Exit"				,		cdSuExit 		},
//--------------------------< BEGIN >-----------------------------------------
// TODO: Add test cases here, format { "Name of test" , testFunction },
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
{"Create Appl Thrd "		,		createULThr	        },
{"Create SysMgmt thrd "		,		createSMThr             },
{"Create TCP NH"		,		createTcpNh 		},
{"Configure  TCP NH"		,		configTcpNh		},
{"Start TCP NH"			,		startTcpNh		},
{"Stop TCP NH"			,		stopTcpNh		},
{"Delete TCP NH"		,		deleteTcpNh		},
{"Terminate TCP NH"		,		terminateTcpNh		},
{"Status TCP NH"		,		statusReqTcpNh		},
{"Statistic TCP NH"		,		statsReqTcpNh		},
{"Add Listener"			,		addListener		},
{"Open Connection"		,		connectReq		},
{"Open Connection Load Testing" ,		connectReqLoadTesting	},
{"Send Data"			,		sendData		},
{"Send Data Load Testing"	,		sendDataLoadTesting	},
{"Remove Listener"		,		remListener		},
{"Close Connection"		,		closeConReq		},
{"Close Connection Load Testing",		closeConReqLoadTesting	},
{"Set Invalid NHTCP Instance Id",		setInvNhInstId		},
{"Print Stats			",		printStats		},
{"Exit"				,		cdSuExit 		},
#if 0
CdTestCases cdTests [] = {
{"Print menu"			,		0	},
{"Set trace level: 0"		,		0	},
{"Set trace level: 1"		,		0	},
{"Set trace level: 2"		,		0	},
{"Set trace level: 3"		,		0	},
{"Set trace level: 4"		,		0	},
{"Set trace level: 5"		,		0	},
#endif
//--------------------------< BEGIN >-----------------------------------------
// TODO: Add test cases here, format { "Name of test" , 0 },
//---------------------------< END >------------------------------------------
{0				,		0	}
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

#ifdef __sun
	int cliLen = sizeof (cliAddr);
#else
	socklen_t cliLen = sizeof (cliAddr);
#endif

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
