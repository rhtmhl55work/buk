//============================================================================
// File         : timerFuncTest.cpp
// Description  : Contains different test cases to test Valid/Invalid Inputs
// 		and StartTimer, StopTimer functions.
// Created By	: Indu Mishra
// Last Modified: 29th April, 2003
// Modified By	: Indu Mishra
//============================================================================

//----------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------

#ifdef __ECOS
#include <network.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/infra/diag.h>
#endif

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "cdsutrace.h"
#include "cdsutimer.h"

#ifdef __ECOS
#define CHOICE_THRU_SOCK 1
#define TEST_SOCK_PORT 9500
#endif


#if defined (__ECOS)
externC void init_all_network_interfaces ();
#endif

int createTestSock ();
int getChoiceSock (int sockfd);
void printMenu ();
int getNumTests ();

#ifdef __ECOS
// create the test socket connection
int createTestSock ()
{
	int sock, msgsock;
	struct sockaddr_in serveraddr, cli_addr;

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		CDSU_TRACE (1, "socket fail\n");
		return -1;
	} 
	
	memset (&serveraddr, 0, sizeof (serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons (TEST_SOCK_PORT);
	serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);

	if (bind (sock, (struct sockaddr*) &serveraddr, sizeof (serveraddr)) < 0)
	{
		CDSU_TRACE (1, "bind failure\n");
		return -1;
	} 

	if ((listen (sock, 5)) < 0)
	{
		CDSU_TRACE (1, "listen failure\n");
		return -1;
	} 

	socklen_t clilen = sizeof (cli_addr);
	CDSU_TRACE (1, "Waiting for test input connection\n");
	if ((msgsock = accept (sock, (sockaddr*) &cli_addr, &clilen)) < 0)
	{
		CDSU_TRACE (1, "accept error\n");
		return -1;
	} else 
	{
		CDSU_TRACE (1, "connection established\n");
	}

	close (sock);
	return msgsock;
}

// get choice from socket
int getChoiceSock (int sockfd)
{
	int choice;
	if (read (sockfd, (char*)&choice, sizeof (int)) < 0)
	{
		CDSU_TRACE (1, "read error\n");
		return -1;
	}
	
	choice = ntohl (choice);
	return choice;
}

#endif


//----------------------------------------------------------------------------
// Enumerations.
//----------------------------------------------------------------------------
// To test ModuleId.
enum Modules
{
	MODULE_P = 1,
	MODULE_Q,
	MODULE_R
};

//----------------------------------------------------------------------------
// Function prototypes.
//----------------------------------------------------------------------------

// test functions
typedef void (*PtrFunc) ();
struct TestCases 
{
	char* testName;
	PtrFunc testFn;
};

// Functions to test Inputs.
// timeCounts
void TT_TIME_COUNT_V();
void TT_TIME_COUNT_INV();
// timeUnits
void TT_TIME_UNIT_V_001();
void TT_TIME_UNIT_V_002 ();
void TT_TIME_UNIT_V_003 ();
void TT_TIME_UNIT_V_004 ();
void TT_TIME_UNIT_V_005 ();
void TT_TIME_UNIT_INV ();
// destModule Id
void TT_DEST_MODULE_ID_V();
void TT_DEST_MODULE_ID_INV();
// opcode
void TT_OPCODE_V();
void TT_OPCODE_INV();
// timer type
void TT_TIMER_TYPE_V_001();
void TT_TIMER_TYPE_V_002();
void TT_TIMER_TYPE_INV();
// time report type
void TT_TIME_OUT_REP_TYPE_V_001();
void TT_TIME_OUT_REP_TYPE_V_002();
void TT_TIME_OUT_REP_TYPE_INV();
// call back Function 
void TT_CALL_BACK_V();
void TT_CALL_BACK_INV();
// stop timer
void TT_STOP_TIMER_V();
void TT_STOP_TIMER_INV();
// start timer
void TT_START_TIMER_V();

// Array of TestCases and their Descriptions.
TestCases tests [] = {
	{"Valid Time Count",TT_TIME_COUNT_V},
	{"Invalid Time Count",TT_TIME_COUNT_INV},
	{"Valid Time Unit 001",TT_TIME_UNIT_V_001},
	{"Valid Time Unit 002",TT_TIME_UNIT_V_002},
	{"Valid Time Unit 003",TT_TIME_UNIT_V_003},
	{"Valid Time Unit 004",TT_TIME_UNIT_V_004},
	{"Valid Time Unit 005",TT_TIME_UNIT_V_005},
	{"Invalid Time Unit",TT_TIME_UNIT_INV},
	{"Valid Destination ModuleId",TT_DEST_MODULE_ID_V},
	{"Invalid Destination ModuleId",TT_DEST_MODULE_ID_INV},
	{"Valid Opcode",TT_OPCODE_V},
	{"Invalid Opcode",TT_OPCODE_INV},
	{"Valid Timer Type 001",TT_TIMER_TYPE_V_001},
	{"Valid Timer Type 002",TT_TIMER_TYPE_V_002},
	{"Invalid Timer Type",TT_TIMER_TYPE_INV},
	{"Valid TimeOut Report Type 001",TT_TIME_OUT_REP_TYPE_V_001},
	{"Valid TimeOut Report Type 002",TT_TIME_OUT_REP_TYPE_V_002},
	{"Invalid TimeOut Report Type",TT_TIME_OUT_REP_TYPE_INV},
	{"Valid CallBack Function",TT_CALL_BACK_V},
	{"Invalid CallBack Function",TT_CALL_BACK_INV},
	{"Valid Stop Timer",TT_STOP_TIMER_V},
	{"Invalid Stop Timer",TT_STOP_TIMER_INV},
	{"Valid Start Timer",TT_START_TIMER_V}
	
};

// User TimeOut Handler Function.
void cdSuUserTimeoutHlr (CdSuTimeout);

// Function userInput
void userInput(void );

// CallBackFunction.
void CallBackFunction (CdSuTimeout &);

// Function to Start Tick Mechanism.
void startTickMechanism (void );

#if defined (__ECOS) || defined (sun)
void sigalrmHandler (int signo, siginfo_t * info, void *context);
#else
void sigalrmHandler (int xx);
#endif

//----------------------------------------------------------------------------
// Global variables.
//----------------------------------------------------------------------------
// Total number ot timers started.
int MAX = 0;

// Tid of mail thread
int maintid;
pthread_t tid1;
// Total number of test cases.

// Create timer handler.
CdSuTimerHandler* timerHandler = new CdSuTimerHandler;

//---------------------------------------------------------------------
// Function     : main
// Description  :
//---------------------------------------------------------------------

int main (int argc, char **argv)
{
	sigset_t set;
	sigfillset (&set);

	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
	{
		CDSU_TRACE (1, "signal mask error\n");
	}



#ifdef __ECOS
	init_all_network_interfaces ();
#endif
	CdSuTrace::setLevel (4);

	maintid = pthread_self();
	
	// Function UserInput
	userInput();

	// Start Tick Mechanism.
	startTickMechanism ();
	
	while (1)
	{
		// Clear all signals except SIGALRM.
		sigset_t sigset;
		sigemptyset (&sigset);
		sigaddset (&sigset, SIGALRM);
		sigaddset (&sigset, SIGINT);
		
		pthread_sigmask (SIG_UNBLOCK, &sigset, NULL);
		// Wait for SIGALRM to arrive.
		int sig;

		sigwait (&sigset, &sig);
		
		if (sig == SIGINT)
		{
			CDSU_TRACE(4, "Main : Exit While Loop\n");
			break;
		}

		// Call the timer "tick ()" function.
		if ( sig == SIGALRM)
		{
			timerHandler->timerTickHandler ();
		}

	}
}//main

//----------------------------------------------------------------------------
// print menu 
//----------------------------------------------------------------------------
void printMenu ()
{
	printf ("========================================================\n");
	int i;
	for (i = 0; tests[i].testFn != 0 ; ++i)
	printf ("(%d) %s\n", i, tests[i].testName);
	printf ("(%d) EXIT!\n",i);
	printf ("---------------------------------------------------------\n");
}

int getNumTests ()
{
	// compute the number of tests
	int numTests = 0;
	for (int i = 0; tests[i].testFn != 0 ; ++i)
		++numTests;
	return numTests;
}



//----------------------------------------------------------------------------
// Function : UserInput()
// Description : TO get User Input.
//----------------------------------------------------------------------------
void userInput(void )
{
	int choice;
	int numTests = getNumTests ();
	bool gotValue = false;
	int testSockFd;
	while(!gotValue)
	{

#ifndef CHOICE_THRU_SOCK
		printMenu ();
		printf ("Enter your choice:  ");
#endif

		bool isConnEst = true;
#ifdef CHOICE_THRU_SOCK
		// setup communication for test input
		testSockFd = createTestSock ();
		if (testSockFd < 0)
		{
			isConnEst = false;
		}
#endif

		if (isConnEst == true)
		{
#ifdef CHOICE_THRU_SOCK
			// get choice from socket
			choice = getChoiceSock (testSockFd);
#else
			// get choice from teminal
			printf ("Enter your choice; 0 for menu : ");
			scanf ("%d", &choice);
#endif
			if(choice == numTests)
			{
				printf("EXIT!\n");
				exit(0);

				
			}
			if ( (choice >=0) && (choice < numTests) )
			{
				gotValue = true;
				tests[choice].testFn ();
			} else
			{
				CDSU_TRACE (4, "Wrong choice! \n");
			}
		}
	}
	close (testSockFd);
		
}//userInput

//---------------------------------------------------------------------
// Function     :TT_START_TIMER_V() 
// Description  : To start a valid timer.
//---------------------------------------------------------------------
void TT_START_TIMER_V () 
{
	CDSU_TRACE (4, "Start Timer : \n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_START_TIMER_V


//---------------------------------------------------------------------
// Function     : cdSuUserTimeoutHlr
// Description  : user handler function 
//--------------------------------------------------------------------
void cdSuUserTimeoutHlr (CdSuTimeout tempTimeOut)
{
	//CDSU_TRACE(4, "Function cdsuTimeoutHlr: Success\n");
	//printf ("CdSuUserTimeOut/handler\n");
#ifdef __ECOS
	//diag_printf("Function cdsuTimeoutHlr: Success\n");
#endif
	CdSuTimerMsg *ptr;
	
	CdModuleId mId;
	mId.srvcHlrId.srvcType = tempTimeOut.destModuleId.srvcHlrId.srvcType;
	
	ptr = new CdSuTimerMsg;
	ptr->initMsg (mId, mId, tempTimeOut.timerId, tempTimeOut.userInfo );

	//ptr->destId = tempTimeOut.destModuleId;
	//ptr->timerId = tempTimeOut.timerId;
	//ptr->userInfo = tempTimeOut.userInfo;
	//ptr->opcode = tempTimeOut.opcode;
	
	//add message in queue.
	if(tempTimeOut.destModuleId.srvcHlrId.srvcType == MODULE_P)
	{
		CDSU_TRACE1 (4, "cdSuTimeoutHlr : srvcType is%d\n",
				tempTimeOut.destModuleId.srvcHlrId.srvcType);
		
	}else
	{
		CDSU_TRACE(4, "INVALID srvcType \n");
	}
}//cdSuUserTimeOutHlr

//----------------------------------------------------------------------------
// Function    : TT_TIME_COUNT_V()
// Description : Function to test Valid TimeCount
//----------------------------------------------------------------------------
void TT_TIME_COUNT_V()
{
	
	CDSU_TRACE (4, "Time Count:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 2;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}

}//TT_TIME_COUNT_V

//----------------------------------------------------------------------------
// Function	: TT_TIME_COUNT_INV()
// Description  : Function to test InValid TimeCount
//----------------------------------------------------------------------------
void TT_TIME_COUNT_INV()
{
	
	CDSU_TRACE (4, "Time Count:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 0;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}

}//TT_TIME_COUNT_INV

//----------------------------------------------------------------------------
// Function	: TT_TIME_UNIT_V_001()
// Description	: Function to test TimeUnits for TENS.
//----------------------------------------------------------------------------
void TT_TIME_UNIT_V_001()
{
	CDSU_TRACE (4, "Time Unit:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_TENMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_TIME_UNIT_V_001

//----------------------------------------------------------------------------
// Function	: TT_TIME_UNIT_V_002()
// Description	: Function to test TimeUnits for TENS.
//----------------------------------------------------------------------------
void TT_TIME_UNIT_V_002()
{
	CDSU_TRACE (4, "Time Unit:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
}//TT_TIME_UNIT_V_002

//----------------------------------------------------------------------------
// Function	: TT_TIME_UNIT_V_003()
// Description	: Function to test TimeUnits for SECOND.
//----------------------------------------------------------------------------
void TT_TIME_UNIT_V_003()
{
	CDSU_TRACE (4, "Time Unit:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
}//TT_TIME_UNIT_V_003

//----------------------------------------------------------------------------
// Function	: TT_TIME_UNIT_V_004()
// Description	: Function to test TimeUnits for MINUTE.
//----------------------------------------------------------------------------
void TT_TIME_UNIT_V_004()
{
	CDSU_TRACE (4, "Time Unit:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_MINUTE;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_TIME_UNIT_V_004

//----------------------------------------------------------------------------
// Function	: TT_TIME_UNIT_V_005()
// Description	: Function to test TimeUnits for HOUR.
//----------------------------------------------------------------------------
void TT_TIME_UNIT_V_005()
{
	CDSU_TRACE (4, "Time Unit:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HOUR;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
}//TT_TIME_UNIT_V_005

//----------------------------------------------------------------------------
// Function	: TT_TIME_UNIT_INV()
// Description	: Function to test TimeUnits for INVALID.
//----------------------------------------------------------------------------
void TT_TIME_UNIT_INV()
{
	CDSU_TRACE (4, "Time Unit:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_INVALID;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
}//TT_TIME_UNIT_INV


//--------------------------------------------------------------------------
// Function	: TT_DEST_MODULE_ID_V()
// Description	: Function to test Valid DestModuleId
//--------------------------------------------------------------------------
void TT_DEST_MODULE_ID_V()
{
	CDSU_TRACE (4, "Destination ModuleId:%d\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_DEST_MODULE_ID_V

//--------------------------------------------------------------------------
// Function	: TT_DEST_MODULE_ID_INV()
// Description	: Function to test InValid DestModuleId
//--------------------------------------------------------------------------
void TT_DEST_MODULE_ID_INV()
{
	CDSU_TRACE (4, "Destination ModuleId:\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_Q;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_DEST_MODULE_ID_INV

//---------------------------------------------------------------------------
// Function	: TT_OPCODE_V()
// function to test Valid Opcode
//---------------------------------------------------------------------------
void TT_OPCODE_V()
{
        CDSU_TRACE (4, "Opcode :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}	
	
}//TT_OPCODE_V

//---------------------------------------------------------------------------
// Function	: TT_OPCODE_INV()
// Description	: function to test InValid Opcode
//---------------------------------------------------------------------------
void TT_OPCODE_INV()
{
        CDSU_TRACE (4, "Opcode :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = (Uint16)NULL;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
		
}//TT_OPCODE_V


//----------------------------------------------------------------------------
// Function	: TT_TIMER_TYPE_V_001()
// Description	: Function to test TimerType for Trigger Once Timer.
//----------------------------------------------------------------------------
void TT_TIMER_TYPE_V_001()
{
	CDSU_TRACE (4, "TimerType :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_TIMER_TYPE_V_001

//----------------------------------------------------------------------------
// Function	: TT_TIMER_TYPE_V_002()
// Description	: Function to test TimerType for Cyclic Timer.
//----------------------------------------------------------------------------
void TT_TIMER_TYPE_V_002()
{
	CDSU_TRACE (4, "TimerType :\n");
	printf ("Cyclic\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_CYCLIC;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_TIMER_TYPE_V_002


//----------------------------------------------------------------------------
// Function	: TT_TIMER_TYPE_INV()
// Description	: Function to test TimerType for Invalid Timer Type.
//----------------------------------------------------------------------------
void TT_TIMER_TYPE_INV()
{
	CDSU_TRACE (4, "TimerType :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_INVALID;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
}//TT_TIMER_TYPE_V_001



//---------------------------------------------------------------------------
// Function	: TT_TIME_OUT_REP_TYPE_V_001()
// Description	: Function to testTimeout Report Type for CallBack.
//---------------------------------------------------------------------------
void TT_TIME_OUT_REP_TYPE_V_001()
{
	CDSU_TRACE (4, "TimeOut Report Type :%d\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	timerInfo1.timerCallBackFn = CallBackFunction;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
		
}//TT_TIME_OUT_REP_TYPE_V_001 

//---------------------------------------------------------------------------
// Function	: TT_TIME_OUT_REP_TYPE_V_002
// Description	: Function to testTimeout Report Type for Message.
//---------------------------------------------------------------------------
void TT_TIME_OUT_REP_TYPE_V_002()
{
	CDSU_TRACE (4, "TimeOut Report Type :%d\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
		
}//TT_TIME_OUT_REP_TYPE_V_002 

//---------------------------------------------------------------------------
// Function	: TT_TIME_OUT_REP_TYPE_INV()
// Description	:Function to testTimeout Report Type for Invalid TimeOut Report Type.
//---------------------------------------------------------------------------
void TT_TIME_OUT_REP_TYPE_INV()
{
	CDSU_TRACE (4, "TimeOut Report Type :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
		
}//TT_TIME_OUT_REP_TYPE_INV 


//---------------------------------------------------------------------------
// Function	: TT_CALL_BACK_V()
// Description	: unction to test Valid CallBack.
//---------------------------------------------------------------------------
void TT_CALL_BACK_V()
{
	CDSU_TRACE (4, "CallBack Function :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	timerInfo1.timerCallBackFn = CallBackFunction;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
	
}//TT_CALL_BACK_V

//---------------------------------------------------------------------------
// Function	: TT_CALL_BACK_INV()
// Description	: Function to test InValid CallBack.
//---------------------------------------------------------------------------
void TT_CALL_BACK_INV()
{
	CDSU_TRACE (4, "CallBack Function :\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_HUNDREDMS;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 5;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
}//TT_CALL_BACK_INV

//---------------------------------------------------------------------------
// Function	: TT_STOP_TIMER_V()
// Description	: Function to test Valid Stop Timer
//---------------------------------------------------------------------------
void TT_STOP_TIMER_V()
{
	CDSU_TRACE (4, "Stop Timer : \n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}
		
}//TT_STOP_TIMER_V


//---------------------------------------------------------------------------
// Function	: TT_STOP_TIMER_INV()
// Description	: Function to test Invalid Stop Timer
//---------------------------------------------------------------------------
void TT_STOP_TIMER_INV()
{
	CDSU_TRACE (4, "Stop Timer : %d\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 5;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_SUCCESS)
	{
		CDSU_TRACE1 (4, "Timer started, Id: %d\n", timerId);
	} else 
	{
		CDSU_TRACE1 (4, "Invalid Timer Id: %d\n", timerId);
	}	
	
	Uint32 retVal = timerHandler->stopTimer(++timerId);
	if(retVal != CDSU_TMR_RV_SUCCESS )
	{
		CDSU_TRACE1(4, "StopTimer Failure, timerId Invalid %d\n", timerId);
	}else
	{
		CDSU_TRACE (4, "StopTimer Success!\n");
	}
		
}//TT_STOP_TIMER_INV


//---------------------------------------------------------------------
// Function     : CallBackFunction
// Description  : Used to make a message and send it back to client. 
// Input        : reference timerId and reference UserInfo.
//---------------------------------------------------------------------
void CallBackFunction (CdSuTimeout & tempTimeOut1)
{
	CDSU_TRACE1 (4, "CallBackFunction : %d\n", tempTimeOut1.timerId);
}//CallBackFunction

//---------------------------------------------------------------------
// Function     : sigalrmHandler
// Description  : Solaris implementation of timer inbuilt functions.
//---------------------------------------------------------------------

#if defined (__ECOS) || defined (sun)
void
sigalrmHandler (int signo, siginfo_t * info, void *context)
#else
void
sigalrmHandler (int xx)
#endif
{
	timerHandler->timerTickHandler ();
	
}//sigalrmHandler

//------------------------------------------------------------------------
// Function StartTickMechanism
//------------------------------------------------------------------------
void startTickMechanism (void )
{

	CDSU_TRACE(4, "Function startTickMechanism\n");
	
#if defined (__ECOS) || defined (sun)
	// ecos timer initialization.
	struct itimerspec timerValue;	// Timeout value on eCos
	sigset_t mask;	// Signal Mask
	timer_t timer1;	// Timer

	// enable posix timer, initialise the signal set 
	sigfillset (&mask);

	// install signal handler
	struct sigaction sa;
	sa.sa_sigaction = sigalrmHandler;	// on signal this fn gets invoked.
	sa.sa_mask = mask;
	sa.sa_flags = SA_SIGINFO;
#ifdef sun
	sa.sa_flags |= SA_RESTART;
#endif
	// specify the action associated with a signal
	sigaction (SIGALRM, &sa, NULL);

	// create timers
	struct sigevent sev;

	// notification type
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGALRM;
	sev.sigev_value.sival_int = 0xABCDEF01;

	// timer values
	timerValue.it_value.tv_sec = 0;
	timerValue.it_value.tv_nsec = 10000000;
	timerValue.it_interval.tv_sec = 0;
	timerValue.it_interval.tv_nsec = 10000000;   //10 ms basic tick
	timer_create (CLOCK_REALTIME, &sev, &timer1);
	timer_settime (timer1, 0, &timerValue, NULL);

#else
	itimerval itval, otval;
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_usec = 10000;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_usec = 10000;

	// set signal handler
	setitimer (ITIMER_REAL, &itval, &otval);

#endif

} // void startTickMechanism ()


//====================










