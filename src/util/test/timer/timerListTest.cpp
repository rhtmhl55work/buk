//============================================================================
// File         : timerListTest.cpp
// Description  : Implements different test cases to test add() and delete()
// 		  functions.
// Created By	: Indu Mishra
// Modified By	:
// Last Modified: 29/04/2003
// Modified By  : Indu Mishra
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
#include "cdsuqueue.h"
#include "cdsutimer.h"

#ifdef __ECOS

#define CHOICE_THRU_SOCK 1
#define TEST_SOCK_PORT 9500

int createTestSock ();
int getChoiceSock (int sockfd);

// Create the test socket connection.
int createTestSock ()
{
	int sock, msgsock;
	struct sockaddr_in serveraddr, cli_addr;

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		CDSU_TRACE (4, "socket fail\n");
		perror (" Socket creation error\n");
		return -1;
	} 
	
	memset (&serveraddr, 0, sizeof (serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons (TEST_SOCK_PORT);
	serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);

	if (bind (sock, (struct sockaddr*) &serveraddr, sizeof (serveraddr)) < 0)
	{
		CDSU_TRACE (4, "bind failure\n");
		perror (" Bind fail\n");
		return -1;
	} 

	if ((listen (sock, 5)) < 0)
	{
		CDSU_TRACE (4, "listen failure\n");
		perror (" Listen fail\n");
		return -1;
	} 

	socklen_t clilen = sizeof (cli_addr);
	//CDSU_TRACE (4, "Waiting for test input connection\n");
	if ((msgsock = accept (sock, (sockaddr*) &cli_addr, &clilen)) < 0)
	{
		CDSU_TRACE (4, "accept error\n");
		perror (" accept fail\n");
		return -1;
	} else 
	{
		CDSU_TRACE (4, "connection established\n");
	}

	close (sock);
	return msgsock;
}
#endif


#if defined (__ECOS)
externC void init_all_network_interfaces ();
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

// To choose different test cases.
enum CHOICE
{
	ADD_NO_NODE = 1,
	ADD_FIRST_NODE,
	ADD_LAST_NODE,
	ADD_MIDDLE_NODE,
	TO_FIRST_NODE,
	STOP_FIRST_NODE,
	STOP_LAST_NODE,
	STOP_MIDDLE_NODE,
	CYCLIC_TIMER
};


//----------------------------------------------------------------------------
// Function prototypes.
//----------------------------------------------------------------------------
void cdSuUserTimeoutHlr (CdSuTimeout);

// Functions called up to start Timers.
void startTestTimer1 (Uint16, Uint32, int);

// Functions called up to start Cyclic Timers.
void startTestTimer2 (Uint16, Uint32);

// Function to StopTimer
void stopTestTimer1(Uint32);

// CallBackFunction.
void CallBackFunction (CdSuTimeout &);

// Function getUserInput
void getUserInput(void );

// Function handleUserInput
void handleUserInput(int );

// Function to Start Tick Mechanism.
void startTickMechanism (void );

// Function to receive TimeOuts.
void timerTimeOut(int );

// Function recFromQueue
void* recFromQueue(void*);

// Temp Function to test Cyclic Stop Timer.
void* cyclicTest(void*);

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
bool isCyclic = false;

//---------------------------------------------------------------------
//Part for Message Queue;
//---------------------------------------------------------------------
CdSuTimerMsg* tmmsg;
CdSuQueue <CdSuTimerMsg*>* queue ;

// Create timer handler.
CdSuTimerHandler* timerHandler = new CdSuTimerHandler;

//---------------------------------------------------------------------
// Function     : main
// Description  :
//---------------------------------------------------------------------
int
main (int argc, char **argv)
{
#ifdef __ECOS
	init_all_network_interfaces ();
#endif
	CdSuTrace::setLevel (5);
	
#ifndef __ECOS
	// Function to get user input.
	getUserInput();
#endif
	int testSockFd;
	bool isConnEst;
#ifdef CHOICE_THRU_SOCK
	testSockFd = createTestSock ();
	if (testSockFd < 0)
	{
		isConnEst = false;
		//exit (-1);
	}
#endif

	// Data to get input from client file.
	bool gotValue = false;
	while(!gotValue)
	{
		bool isConnEst = true;
		// setup communication for test input
		if (isConnEst == true)
		{
#ifdef CHOICE_THRU_SOCK
			int choice = 0;
			// get choice from socket
			choice = getChoiceSock (testSockFd);
			handleUserInput(choice);
#endif
			gotValue = true;
		}
	}
	//close (testSockFd);
			
	// Thread Id of main().
	maintid = pthread_self();

	// Create Queues.
	queue = new CdSuQueue <CdSuTimerMsg*> (MAX+20);
	
	int retVal;
	// Create a new thread for calling queue function
	pthread_attr_t t1;
	pthread_attr_init (&t1);
	//pthread_attr_setscope (&thread1Attr, PTHREAD_SCOPE_SYSTEM);
	retVal = pthread_create (&tid1, &t1, recFromQueue, NULL);
	if (retVal != 0)
	 	CDSU_TRACE (4, "Thread1 create error\n");
	
	// Close Remote Socket.
	close (testSockFd);
	
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
		
		if( sig == SIGINT)
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

}// main

// Get choice from socket.
int getChoiceSock (int sockfd)

{
	int choice;
	if (read (sockfd, (char*)&choice, sizeof (int)) < 0)
	{
		CDSU_TRACE (4, "read error\n");
		return -1;
	}
	
	choice = ntohl (choice);
	return choice;
}


//-----------------------------------------------------------------------------
// Function getUserInput()
//-----------------------------------------------------------------------------
void getUserInput(void )
{
	CDSU_TRACE(4, "Function getUserInput\n");
	int input;
	CDSU_TRACE(4, "*************************************\n");
	CDSU_TRACE(4, "CHOICE:\n");
	
	CDSU_TRACE(4, "ADD_NO_NODE : 1\n");
	CDSU_TRACE(4, "ADD_FIRST_NODE : 2\n");
	CDSU_TRACE(4, "ADD_LAST_NODE : 3\n");
	CDSU_TRACE(4, "ADD_MIDDLE_NODE : 4\n");
	CDSU_TRACE(4, "TO_FIRST_NODE : 5\n");
	CDSU_TRACE(4, "STOP_FIRST_NODE : 6\n");
	CDSU_TRACE(4, "STOP_LAST_NODE : 7\n");
	CDSU_TRACE(4, "STOP_MIDDLE_NODE : 8\n");
	CDSU_TRACE(4, "CYCLIC_TIMER : 9\n");
	CDSU_TRACE(4, "*************************************\n");
	
	CDSU_TRACE(4, "\tEnter Your Choice for Menu: ");
	scanf ("%d", &input);
	CDSU_TRACE (4, "\n");
	handleUserInput (input);
}

//-----------------------------------------------------------------------------
// Function handleUserInput()
//-----------------------------------------------------------------------------
void handleUserInput(int input )
{
	CDSU_TRACE(4, "Function handleUserInput\n");
	switch(input)
	{
	case ADD_NO_NODE:
		{
			CDSU_TRACE(4, "ADD_NO_NODE\n");
			Uint16 timeCnt = 2;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 1;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			MAX = 1;
			break;
		}

	case ADD_FIRST_NODE:
		{	CDSU_TRACE(4, "ADD_FIRST_NODE\n");
			Uint16 timeCnt = 14;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 1;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			timeCnt = 4;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 1;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			MAX = 2;
			break;
		}

	case ADD_LAST_NODE:
		{
			CDSU_TRACE(4, "ADD_LAST_NODE\n");
			Uint16 timeCnt = 2;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			// Start Timers for different denominations.
			timeCnt = 12;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 1;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			MAX = 3;
			break;
		}

	case ADD_MIDDLE_NODE:
		{
			CDSU_TRACE(4, "ADD_MIDDLE_NODE\n");
			Uint16 timeCnt = 2;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			// Start Timers for different denominations.
			timeCnt = 22;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			// Start Timers for different denominations.
			timeCnt = 12;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 1;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
		
			MAX = 5;
			break;
		}

	case TO_FIRST_NODE:
		{
			CDSU_TRACE(4, "TO_FIRST_NODE\n");
			Uint16 timeCnt = 3;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 3;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			MAX = 3;
			break;
		}

	case STOP_FIRST_NODE:
		{
			CDSU_TRACE(4, "STOP_FIRST_NODE\n");
			Uint16 timeCnt = 3;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			// Call stopTimer.
			Uint32 timerId = 1;
			MAX = 2;
			stopTestTimer1(timerId);
			break;
		}

	case STOP_LAST_NODE:
		{
			CDSU_TRACE(4, "STOP_LAST_NODE\n");
			Uint16 timeCnt = 3;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			timeCnt = 13;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			MAX = 4;
			Uint32 timerId = 4;	
			stopTestTimer1(timerId);
			break;
		}

	case STOP_MIDDLE_NODE:
		{
			CDSU_TRACE(4, "STOP_MIDDLE_NODE\n");
			Uint16 timeCnt = 3;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			int noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			timeCnt = 23;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 2;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			timeCnt = 13;
			timeUni = CDSU_TMR_TU_HUNDREDMS;
			noOfTimer = 1;
			startTestTimer1(timeCnt, timeUni, noOfTimer);
			
			MAX = 5;
			Uint32 timerId = 5;
			stopTestTimer1(timerId);
			break;
		}

	case CYCLIC_TIMER:
		{
			CDSU_TRACE(4, "CYCLIC_TIMER\n");
			
			Uint16 timeCnt = 2;
			Uint32 timeUni = CDSU_TMR_TU_HUNDREDMS;
			
			int retVal;
			pthread_t tid;
			
			// Create a new thread for calling queue function
			pthread_attr_t thr;
			pthread_attr_init (&thr);
			//pthread_attr_setscope (&thread1Attr, PTHREAD_SCOPE_SYSTEM);
			retVal = pthread_create (&tid, &thr, cyclicTest, NULL);
			if (retVal != 0)
			CDSU_TRACE (4, "Thread1 create error\n");

			startTestTimer2(timeCnt, timeUni);
			
			MAX = 1;
			isCyclic = true;
			break;
		}
		
	default :
		{
			CDSU_TRACE(4, "Sorry! Try Again\n");
			break;
		}
	}
	
}//handleUserInput

//---------------------------------------------------------------------
// Function     :startTestTimer1() 
// Description  :
//---------------------------------------------------------------------
void startTestTimer1 (Uint16 timeCnt, Uint32 timeUni, int noOfTimer) 
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	CdSuTimerId timerId[noOfTimer];
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = timeCnt;
	timerInfo1.timeUnits = (CdSuTimeUnits)timeUni;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	for(int i = 0;i < noOfTimer; i++)
	{
		int back = timerHandler->startTimer (timerId[ i ], timerInfo1);
		
		if (back == CDSU_TMR_RV_INV_TMRID)
		{
		CDSU_TRACE1 (4, "Timer NOT started, id: %d\n", i);
		} else if (back == CDSU_TMR_RV_SUCCESS)
		{
		CDSU_TRACE1 (4, "Timer started, id: %d\n", timerId[i]);
		} else if(back == CDSU_TMR_RV_INV_CALLBACK)
		{
		CDSU_TRACE1 (4, "Invalid Timer CallBack Function for id: %d\n", i);
		}
	}//for
	
}//startTestTimer


//---------------------------------------------------------------------
// Function     :startTestTimer2() 
// Description  :
//---------------------------------------------------------------------
void startTestTimer2 (Uint16 timeCnt, Uint32 timeUni) 
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = timeCnt;
	timerInfo1.timeUnits = (CdSuTimeUnits)timeUni;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_CYCLIC;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

	int back = timerHandler->startTimer (timerId, timerInfo1);
	
	if (back == CDSU_TMR_RV_INV_TMRID)
	{
	CDSU_TRACE (4, "Timer NOT started\n");
	} else if (back == CDSU_TMR_RV_SUCCESS)
	{
	CDSU_TRACE1 (4, "Timer started, id: %d\n", timerId);
	} else if(back == CDSU_TMR_RV_INV_CALLBACK)
	{
	CDSU_TRACE (4, "Invalid Timer CallBack Function \n");
	}else if(back == CDSU_TMR_RV_INV_TMRHASHDEL)
	{
	CDSU_TRACE (4, "Sorry,Node Could Not Be inserted In Hash.\n");
	}
	
}//startTestTimer

//-------------------------------------------------------------------------
// Function recFromQueue
// ------------------------------------------------------------------------
void* recFromQueue(void*)
{
	CDSU_TRACE(4, "Function recFromQueue\n");
	
	if(isCyclic)
	{
		//CDSU_TRACE(4, "CYCLIC rec from QUEUE\n");
		timerTimeOut(40);
	}else 
	{
		timerTimeOut(MAX);
	}
	return(0);
}

//--------------------------------------------------------------------------
// Function to receive  Timer TimeOuts.
//--------------------------------------------------------------------------
// Function to receive  Timer TimeOuts.
void timerTimeOut(int noOfTimer)
{
	CDSU_TRACE(4, "Function timerTimeOut: \n");
	//CDSU_TRACE1 (4, "MAX : %d\n",noOfTimer);
	//diag_printf("Function cdsuTimeoutHlr: Success\n");
	
	int count = 0;
	// wait for messages from the TimerHandler
	//while(count != (noOfTimer))
	while(1)
	{
		if (queue->empty() == false)
		{
			CdSuTimerMsg* tmmsg;
			if(queue->pop (tmmsg))
				CDSU_TRACE (1, "Success in timeOut\n");
			count++;
			CDSU_TRACE1(4, "TO message removed from Queue:%d\n",count);
			//delete tmmsg;
		}
		if(count == noOfTimer)
			break;
	}
	
	return;
}//timerTimeOut


//---------------------------------------------------------------------
// Function     : cdSuUserTimeoutHlr
// Description  : user handler function 
//--------------------------------------------------------------------
void cdSuUserTimeoutHlr (CdSuTimeout tempTimeOut)
{
	CDSU_TRACE(4, "Function cdsuTimeoutHlr\n");
	CdModuleId tmpId ;
	tmpId = tempTimeOut.destModuleId;
	
	CdSuTimerMsg *ptr;
	ptr = new CdSuTimerMsg;
	ptr->initMsg (tmpId, tmpId, tempTimeOut.timerId,
		tempTimeOut.userInfo );

	//ptr->destId = tempTimeOut.destModuleId;
	//ptr->timerId = tempTimeOut.timerId;
	//ptr->userInfo = tempTimeOut.userInfo;
	//ptr->opcode = tempTimeOut.opcode;
	
	//add message in queue.
	if(tempTimeOut.destModuleId.srvcHlrId.srvcType == MODULE_P)
	{
		queue->add(ptr);
		
	}

	CDSU_TRACE1(4, "TimeOut: TimerId = %d\n", ptr->timerId);

}	// void userTimeoutHandler ()

//--------------------------------------------------------------------
//Temp function to verify stop of cyclic timer.
//--------------------------------------------------------------------
void* cyclicTest(void *) 
{
	CDSU_TRACE (4, "Stop Cyclic Timer");
	sleep(3);
	stopTestTimer1(1);
	pthread_kill(maintid, SIGINT );
	return(0);

};

//---------------------------------------------------------------------
// Function to Stop a Timer
//---------------------------------------------------------------------
void stopTestTimer1(Uint32 timerId)
{
	CDSU_TRACE(4, "Function StopTimer\n");
	Uint32 retVal = timerHandler->stopTimer(timerId);
	if(retVal != CDSU_TMR_RV_SUCCESS )
	{
		CDSU_TRACE1(4, "StopTimer Failure, timerId Invalid %d\n", timerId);
	}
	MAX = MAX--;
}


//---------------------------------------------------------------------
// Function     : CallBackFunction
// Description  : Used to make a message and send it back to client. 
// Input        : reference timerId and reference UserInfo.
//---------------------------------------------------------------------
void CallBackFunction (CdSuTimeout & tempTimeOut1)
{
	CDSU_TRACE1 (4, "TimeoutId : %d\n", tempTimeOut1.timerId);
}

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
	
}

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


//====================================================================
// <End of timertest.cpp>
//====================================================================
