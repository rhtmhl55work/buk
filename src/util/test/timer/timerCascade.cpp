//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename		: timerCascade.cpp
// Description		: Used to test H/W timer.
// 			  This program starts a given number of timers, and
// 			  logs their timeOut times. Can be run on Linux, and
// 			  the target.Use cascaded timers.
// Ref Docs		: MPC8260 Document. 
// Author		: Indu Mishra
// Created On		: 29/05/03
// Last Modified	:
// Last Modified On  	: 02/07/03
//============================================================================

//----------------------------------------------------------------------------
// Include Header Files.
//----------------------------------------------------------------------------
#include <pthread.h>
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

#ifdef __ECOS
#include "cdsu_counter.h"
#include<cyg/hal/mpc82xx_hl/hl_gptimer.h>
#include <cyg/hal/mpc82xx_hl/hl_quiccii.h>
#include <cyg/hal/quiccII/ppc82xx.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#endif

#include "cdsutrace.h"
#include "cdsutimer.h"
#include "cdsuutiltypes.h"
#include "cdsuqueue.h"


// The readings are as follows:
// reading 1: absolute time before startTimer
// reading 2: absolute time after startTimer
// reading 3: absolute time on timeout

#define TOTAL_THREADS 1 
#define MAX_THREAD1_TIMERS 10 
#define MAX_TIMERS (MAX_THREAD1_TIMERS )
#ifdef __ECOS
#define printf diag_printf
#endif

//----------------------------------------------------------------------------
// External Functions
//----------------------------------------------------------------------------
#ifdef __ECOS
externC void init_all_network_interfaces (void);
#endif
extern void* timerThreadEntry(void*);

//----------------------------------------------------------------------------
// Enumerations.
//----------------------------------------------------------------------------
enum Modules
{
	MODULE_P = 1,
	MODULE_Q,
	MODULE_R
};

//----------------------------------------------------------------------------
// Function prototypes.
//----------------------------------------------------------------------------

// User TimeOut Function.
void cdSuUserTimeoutHlr (CdSuTimeout);

// Timer Initailization Function.
void timerInit (void);

// Function to Start Timer.
void* startTestTimer (void*);
void* CyclicTimer (void*);

// Function to craete a timer handler.
void createTimerHandler(void);

// Call Back Function.
void CallBackFunction (CdSuTimeout &);

#if defined (__ECOS) || defined (sun)
void sigalrmHandler (int signo, siginfo_t * info, void *context);
#else
void sigalrmHandler (int xx);
#endif

// Function to do calculation for linux timers.
void calc_linux (void);

// Function to do calculation for ecos timers.
void calc_ecos (void);

//----------------------------------------------------------------------------
// Global variables.
//----------------------------------------------------------------------------

CdSuTimerHandler *timerHandler;
#ifdef __ECOS
CdSuCounter *hl_timer;
#endif

// thread ids
pthread_t tid1, tid2;

int maintid;			//tid of mail thread
int counter = TOTAL_THREADS;	//number of threads started.

bool isCyclic = false;
bool isTimer = false;


#ifdef __ECOS
// Input structure.
struct Input
{
	int beforeStart;
	int afterTO;
};

// Structure to store time difference for each Timeout.
struct Result
{
	int TO_diff;
};

// Array of structures of type OutPut
struct Input input[MAX_TIMERS];

// Array of structures of type Result
struct Result result[MAX_TIMERS];

#else

//structure timezone
struct timezone* Timezone;

//structure for storing absolute timeValues for time before StartTimer,
//time after StartTimer and time after Timeout .
struct OutPut
{
	long timeUnits;
	timeval StartTime;
	timeval TimeOutTime;
	
};

//structure to store time difference for each StartTimer and Timeout
struct Result
{
	long timeUnits;
	long start_to_diff;
};

//array of structures of type OutPut
struct OutPut time_output[MAX_TIMERS];

//array of structures of type Result
struct Result time_result[MAX_TIMERS];

#endif

//---------------------------------------------------------------------
//Part for Message Queue;
//---------------------------------------------------------------------
CdSuTimerMsg* tmmsg;

CdSuQueue <CdSuTimerMsg*>* queue1; 

//----------------------------------------------------------------------------
// Function Name: main()
// Description	: Main function.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
// ---------------------------------------------------------------------------
int main (int argc, char **argv)
{
	sigset_t set;
	sigfillset (&set);

	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
	{
		CDSU_TRACE (1, "signal mask error\n");
	}

	CDSU_TRACE(4, "Main\n");
	printf ("Main\n");
	// Set Trace Level
	CdSuTrace::setLevel (5);
	
	queue1 = new CdSuQueue <CdSuTimerMsg*> (MAX_THREAD1_TIMERS+5);
	int retVal;
	createTimerHandler();
	maintid = pthread_self();

#ifdef __ECOS
	// Initialize the network stack
	init_all_network_interfaces();

	// Call constructor of hl_timer.
	hl_timer = CdSuCounter::Instance(); 
	Uint32 count = hl_timer->readCounter();
	printf("Main: readcount is%d\n",count);

#endif
	retVal = pthread_create (&tid1, 0, CyclicTimer, NULL);
	if (retVal != 0)
		CDSU_TRACE (4, "Thread1 create error\n");

	//--------------------------------------------------------------------
	// Initialise the timer related ds.
	//--------------------------------------------------------------------
#ifndef __ECOS	
	timerInit();

	Timezone = (struct timezone*) malloc (sizeof (timezone));
#endif
	
#ifdef __ECOS
	// block all signals.
	sigset_t set;
	sigfillset (&set);
	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
	{
		CDSU_TRACE (1, "signal mask error\n");
	}
	
	//Create a thread to start Timer Clock Initialisations.
	pthread_attr_t tThreadAttr;
	pthread_attr_init (&tThreadAttr);
	if (pthread_create (&tid2, &tThreadAttr, timerThreadEntry, NULL) != 0)
	{
		CDSU_TRACE (1, "Timer thread create error\n");
		return 1;
	}

#endif
#ifndef __ECOS
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

		if ( sig == SIGINT)
		{
			printf("Received SIGINT, count = %d\n", counter);
			counter--;	//on completion a thread sends
					// an SIGINT
			if (counter == 0) break;
					// when the counter becomes 0,
					// exit and print the results
			
		}
		// Call the timer "tick ()" function.
		if ( sig == SIGALRM)
			timerHandler->timerTickHandler ();
	}

	//TODO: Calculation of Timeout for Linux.
	calc_linux();

#endif // ifdef __ECOS

#ifdef __ECOS 
	sleep(10);
	
	//TODO: Calculation for Timeout for Ecos
	//calc_ecos();
	
	//Loop to prevent thread exiting main
	while(1)
	{
	}
#endif
		
} // main

//----------------------------------------------------------------------------
// Function Name: createTimerHandler()
// Description	: Used to create Handlers for timers.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------
void createTimerHandler(void)
{
	// Initialize the global variables
	timerHandler = new CdSuTimerHandler;	

};

//----------------------------------------------------------------------------
// Function Name: startTestTimer()
// Description	: Used to start timers.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------

void* startTestTimer (void*) 
{
	isTimer = true;
	CDSU_TRACE(4, "StartTestTimer\n");
	printf ("StartTestTimer\n");
	
	int i;
	CdSuTimerId timerId[ MAX_THREAD1_TIMERS ];
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 1;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;

#ifndef __ECOS
	long units;
	switch(timerInfo1.timeUnits)
	{
		case CDSU_TMR_TU_TENMS: units = 10*1000; //10 ms
					break;
		case CDSU_TMR_TU_HUNDREDMS:units = 100*1000; //100ms
					   break;
		case CDSU_TMR_TU_SECOND:units = 1000000; //1sec
					break;
		case CDSU_TMR_TU_MINUTE:units = 60*1000000; //60 sec
					break;
		case CDSU_TMR_TU_HOUR:units = 60*60*1000000; //60 min
					break;
		default:break;
	}
	units = units * timerInfo1.timeCounts;
#endif
	for (i = 0; i < MAX_THREAD1_TIMERS; i++)
	{
		
#ifdef __ECOS
		//Read HL_Timer Counter.
		input[i].beforeStart = hl_timer->readCounter();
		//printf("Start readcount %d\t = %u\n",j, hl_timer->readCounter());
		 	
#else
		struct timeval tz;
		gettimeofday(&tz, Timezone);
#endif
		int back = timerHandler->startTimer (timerId[ i ], timerInfo1);
#ifndef __ECOS
		time_output[timerId[i]].StartTime = tz;	
		time_output[timerId[i]].timeUnits = units;
#endif
		
		if (back == CDSU_TMR_RV_INV_TMRID)
		{
			CDSU_TRACE1 (4, "Timer NOT started, id: %d\n", i);
		} else if (back == CDSU_TMR_RV_SUCCESS)
		{
			CDSU_TRACE1 (4, "Timer started, id: %d\n", timerId[i]);
			printf ("Timer started, id: %d\n", timerId[i]);
		} else if(back == CDSU_TMR_RV_INV_CALLBACK)
		{
			CDSU_TRACE1 (4, "Invalid Timer CallBack Function for id: %d\n", i);
		}
	} //for
	
	int count = 0;
	
#ifndef __ECOS
	struct timeval tz1;
#endif
	// wait for messages from the TimerHandler
	while(count != (MAX_THREAD1_TIMERS))
	{
		if (queue1->empty() == false)
		{
			CdSuTimerMsg* tmmsg;
			bool value = queue1->pop (tmmsg);
			if(value)
			{
#ifdef __ECOS
			input[count].afterTO = hl_timer->readCounter();
			//printf("TO readcount %d\t = %u\n",j, hl_timer->readCounter());
			CDSU_TRACE1 (4, "TimeOut Id%d\n",tmmsg->timerId);			
#else
			gettimeofday(&tz1, Timezone);
			time_output[tmmsg->timerId].TimeOutTime = tz1;
			CDSU_TRACE1 (4, "TimeOut %d: \n",tmmsg->timerId);
#endif
			count ++;
			}
		}
		if(count == (MAX_THREAD1_TIMERS))
			break;
	}

	//signal that thread is over
#ifndef _ECOS
	pthread_kill (maintid, SIGINT);
#endif
	return 0;
} //startTestTimer()

//----------------------------------------------------------------------------
// Function Name: startTestTimer1()
// Description	: Used to start timers.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------

void* CyclicTimer (void*) 
{
	isCyclic = true;
	printf("CyclicTimer\n");
	CdSuTimerId timerId;
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 1;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_CYCLIC;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;
#ifndef __ECOS
	long units;
	switch(timerInfo1.timeUnits)
	{
		case CDSU_TMR_TU_TENMS: units = 10*1000; //10 ms
					break;
		case CDSU_TMR_TU_HUNDREDMS:units = 100*1000; //100ms
					   break;
		case CDSU_TMR_TU_SECOND:units = 1000000; //1sec
					break;
		case CDSU_TMR_TU_MINUTE:units = 60*1000000; //60 sec
					break;
		case CDSU_TMR_TU_HOUR:units = 60*60*1000000; //60 min
					break;
		default:break;
	}
	units = units * timerInfo1.timeCounts;
#endif
	
#ifdef __ECOS	

		input[0].beforeStart = hl_timer->readCounter();
#else 
		struct timeval tz;
		gettimeofday(&tz, Timezone);
#endif
		int back = timerHandler->startTimer (timerId, timerInfo1);
#ifndef __ECOS
		time_output[timerId].StartTime = tz;	
		time_output[timerId].timeUnits = units;
#endif
		if (back == CDSU_TMR_RV_INV_TMRID)
		{
			CDSU_TRACE1 (4, "Timer NOT started, id: %d\n", timerId);
		} else if (back == CDSU_TMR_RV_SUCCESS)
		{
			CDSU_TRACE1 (4, "Timer started, id: %d\n", timerId);
		} else if(back == CDSU_TMR_RV_INV_CALLBACK)
		{
			CDSU_TRACE1 (4, "Invalid Timer CallBack Function for id: %d\n",
					timerId);
		}
	
	int count = 0;
	// wait for messages from the TimerHandler
	while(1)
	{
		if (queue1->empty() == false)
		{
			CdSuTimerMsg* tmmsg;
			bool value = queue1->pop (tmmsg);
			if(value)
			{
#ifdef __ECOS			
			input[count].afterTO = hl_timer->readCounter();			
			//Read HL_Timer Counter.
			CDSU_TRACE1 (4, "TimeOut Id:%d\n",tmmsg->timerId);
#else
			printf ("TimeOut Id:%d\n",tmmsg->timerId );
			struct timeval tz1;
			gettimeofday(&tz1, Timezone);
			time_output[++count].TimeOutTime = tz1;
#endif
			count ++;
			}
		}
		if(count == (MAX_THREAD1_TIMERS+5))
			break;
	}

	//signal that thread is over
#ifndef __ECOS 
        pthread_kill (maintid, SIGINT);
#endif
	return 0;
} //startTestTimer()


//----------------------------------------------------------------------------
// Function Name: cdSuUserTimeoutHandler()
// Description	: User Time Out handler.
// Inputs	: CdSuTimeOut.
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------
void cdSuUserTimeoutHlr (CdSuTimeout tempTimeOut)
{
	//CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	CdModuleId tmpId;
	tmpId = tempTimeOut.destModuleId;
	CdSuTimerMsg* ptr = new CdSuTimerMsg;
	ptr->initMsg (tmpId, tmpId, tempTimeOut.timerId, tempTimeOut.userInfo );
	
	//add message in queue.
	if(tempTimeOut.destModuleId.srvcHlrId.srvcType == MODULE_P)
	{
		queue1->add(ptr);
		
	}
	
} // void userTimeoutHandler ()

//----------------------------------------------------------------------------
// Function Name: CallBackFunction()
// Description	: Used to make a message and send it back to client.
// Inputs	: CdSuTimeout.
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//----------------------------------------------------------------------------

void CallBackFunction (CdSuTimeout & tempTimeOut1)
{
	//CDSU_TRACE1 (4, "TimeoutId : %d\n", tempTimeOut1.timerId);
} //CallBackFunction (CdSuTimeout & tempTimeOut1)


//----------------------------------------------------------------------------
// Function Name: sigalrmHandler()
// Description	: Solaris implementation of timer inbuilt functions.
// Inputs	: signo, siginfo_t * info, void *context
// Return Value	: 
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------

#if defined (__ECOS) || defined (sun)
void
sigalrmHandler (int signo, siginfo_t * info, void *context)
#else
void
sigalrmHandler (int xx)
#endif
{
	timerHandler->timerTickHandler ();
} //sigalrmHandler


//----------------------------------------------------------------------------
// function Name: timerInit()
// Description	: Does the necessary initialization for the timer.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------
void timerInit(void)
{
	CDSU_TRACE(4, "timerInit()\n");
	printf ("timerInit()\n");
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
} // timerInit()


//----------------------------------------------------------------------------
// Function Name: calc_linux()
// Description	: Does the necessary calculation for the timer.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------
void calc_linux(void)
{
#ifndef __ECOS
	long one;
	long two;
	long three;
	long four;
		
	printf("\tTimeUnits\t Ab.TimeOut Time\t\n");

if(isTimer)
{
	for(int i = 0; i < MAX_TIMERS; i++)
	{
		one = (time_output[i].StartTime.tv_sec * 1000000)+
			(time_output[i].StartTime.tv_usec);

		three = (time_output[i].TimeOutTime.tv_sec * 1000000)+
			(time_output[i].TimeOutTime.tv_usec);
		
		//assigning the calculated values in Result array.
		time_result[i].timeUnits = time_output[i].timeUnits;
		time_result[i].start_to_diff = three - one;
		
		printf("\t%ld \t %ld \t\n",time_result[i].timeUnits, time_result[i].start_to_diff);
	}	
}else 
if(isCyclic)
{
	for(int i = 0; i < 10; i++)
	{
		if(i == 0)
		{
			time_result[0].timeUnits = 0;
			time_result[0].start_to_diff = 0;
			printf("\t%ld\t\t%ld \n",time_result[i].timeUnits, time_result[i].start_to_diff);
	 	}
		if(i == 1)
		{
			one = (time_output[i].StartTime.tv_sec * 1000000)+
				(time_output[i].StartTime.tv_usec);

			three = (time_output[i].TimeOutTime.tv_sec * 1000000)+
				(time_output[i].TimeOutTime.tv_usec);
				
			time_result[1].timeUnits = time_output[1].timeUnits;
			time_result[1].start_to_diff = three - one;
			printf("\t%ld \t\t %ld \n",time_result[i].timeUnits, time_result[i].start_to_diff);
	
		}
		if((i > 1) && (i <= 10))
		{
			int j = 0;
			j = i;
				
			one = ((time_output[i].TimeOutTime.tv_sec * 1000000)+
				(time_output[i].TimeOutTime.tv_usec));

			two = ((time_output[--j].TimeOutTime.tv_sec * 1000000)+
				(time_output[--j].TimeOutTime.tv_usec));
				
			four = one - two;
				
			time_result[i].timeUnits = 0;
			time_result[i].start_to_diff = four;
			printf("\t%ld \t\t %ld \n",time_result[i].timeUnits, time_result[i].start_to_diff);
	
		}
		
	}

}//isCyclic
#endif

}// calc_linux


//----------------------------------------------------------------------------
// Function Name: calc_ecos()
// Description	: Does the necessary calculation for the timer on ecos.
// Inputs	: none
// Return Value	: none
// Side Effects	: 
// Algorithm	:
//---------------------------------------------------------------------------
void calc_ecos(void)
{
#ifdef __ECOS
if(isTimer)
{
	printf("Logs\n");
	//TODO: Calculation of timeout time.
	for(int i = 1;i <= MAX_THREAD1_TIMERS;i++)
	{
		printf("ID : %d, Start : %u, TO : %u\n",i, input[i].beforeStart,
			input[i].afterTO );
	}
	/*int diff = 0;
	printf("TimeOut Calculation\n");
	for(int i = 0;i < MAX_THREAD1_TIMERS;i++)
	{
		if((input[i].beforeStart > input[i].afterTO)& (i > 0))
		{
			int temp = input[--i].afterTO;
			diff = (65535-temp)+input[i].afterTO;
		}
		result[i].TO_diff = diff - input[i].beforeStart ;
	}
	printf ("Before_Timer_Start\t","After_TO\t","Total_TO");
	for(int i = 0; i < MAX_THREAD1_TIMERS; i++)
	{
		printf("\t %d, \t%d",input[i].beforeStart,
				input[i].afterTO );
		printf("%d\n",result[i].TO_diff );
	}
	*/
}
//--------------------------------------------------------------------------
//For Cyclic Timer
//--------------------------------------------------------------------------
if(isCyclic)
{
	printf("Logs\n");
	//TODO: Calculation of timeout time.
	for(int i = 1;i <= MAX_THREAD1_TIMERS;i++)
	{
		printf("ID : %d, Start : %u, TO : %u\n",i, input[i].beforeStart,
			input[i].afterTO );
	}
	
	/*printf("Cyclic Calculations\n");
	for(int i = 0;i < MAX_THREAD1_TIMERS;i++)
	{
		//For First Timer Value.
		if(i == 0)
		{
			result[i].TO_diff = input[i].afterTO -
					input[i].beforeStart;
		}
		
		if((input[i].afterTO > input[--i].afterTO)& (i > 0))
		{
			result[i].TO_diff = input[i].afterTO -
					input[--i].afterTO;
		}else if((input[i].afterTO < input[--i].afterTO) & (i > 0))
		{
			result[i].TO_diff = 65535 - input[--i].afterTO;
		}
	
	}
	printf("Before_Timer_Start: %d\t",input[0].beforeStart);
	printf("After TO\t Diff\t\n");
	for(int i = 0; i < MAX_THREAD1_TIMERS; i++)
	{
		printf("\t %d \t%d \n",input[i].afterTO ,
				result[i].TO_diff);
		
	}*/
}
#endif
}// calc_ecos()


//====================================================================
// <End of timertest.cpp>
//====================================================================
