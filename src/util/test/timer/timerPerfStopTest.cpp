//============================================================================
// File         : timertest.cpp
// Description  :
//============================================================================

//----------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------
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
#include "cdsutimer.h"
#include <pthread.h>
#include "cdsutrace.h"
#include "Queue.h"

// The readings are as follows:
// reading 1: absolute time before startTimer
// reading 2: absolute time after startTimer
// reading 3: absolute time on timeout

#define NUM_READINGS 3
#define NUM_OUTPUT 3
#define TOTAL_THREADS 1
#define MAX_THREAD1_TIMERS 7000
#define MAX_THREAD2_TIMERS 1000 
#define MAX_THREAD3_TIMERS 1000
#define MAX_TIMERS (MAX_THREAD1_TIMERS + MAX_THREAD2_TIMERS + MAX_THREAD3_TIMERS)

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
void cdSuUserTimeoutHlr (CdSuTimeout);

//functions called up by each thread
void* startTestTimer1 (void*);
void* startTestTimer2 (void*);
void* startTestTimer3 (void*);
void CallBackFunction (CdSuTimeout &);

#if defined (__ECOS) || defined (sun)
void sigalrmHandler (int signo, siginfo_t * info, void *context);
#else
void sigalrmHandler (int xx);
#endif

//----------------------------------------------------------------------------
// Global variables.
//----------------------------------------------------------------------------
CdSuTimerHandler *timerHandler;

// thread ids
pthread_t tid1, tid2, tid3;

//tid of mail thread
int maintid;
int counter = TOTAL_THREADS; //number of threads started.

//structure timezone
struct timezone* Timezone;

//structure for storing absolute timeValues for time before StartTimer,
//time after StartTimer and time after Timeout .
struct OutPut
{
	long timeUnits;
	timeval StartTime;
	timeval StopTime;
		
};

//structure to store time difference for each StartTimer and Timeout
struct Result
{
	long timeUnits;
	long stop_timer_diff;
	
};

//array of structures of type OutPut
struct OutPut time_output[MAX_TIMERS];

//array of structures of type Result
struct Result time_result[MAX_TIMERS];


//---------------------------------------------------------------------
//Part for Message Queue;
//---------------------------------------------------------------------
CdSuTimerMsg tmmsg;

Queue<CdSuTimerMsg* > *queue1 ;
Queue<CdSuTimerMsg* > *queue2 ;
Queue<CdSuTimerMsg* > *queue3 ;

//---------------------------------------------------------------------
// Function     : main
// Description  :
//---------------------------------------------------------------------
int
main (int argc, char **argv)
{
	int retVal;
	// create timer handler
	timerHandler = new CdSuTimerHandler;
	printf("TOTAL NUMBER OF TIMERS :%d\n",MAX_THREAD1_TIMERS);
	
	queue1 = new Queue<CdSuTimerMsg* > (MAX_THREAD1_TIMERS);
	queue2 = new Queue<CdSuTimerMsg* > (MAX_THREAD2_TIMERS);
	queue3 = new Queue<CdSuTimerMsg* > (MAX_THREAD3_TIMERS);

	Timezone = (struct timezone*) malloc (sizeof (timezone));
	
	maintid = pthread_self();
	
	//Create new thread 1
	pthread_attr_t thread1Attr;
	pthread_attr_init (&thread1Attr);
	//pthread_attr_setscope (&thread1Attr, PTHREAD_SCOPE_SYSTEM);
	retVal = pthread_create (&tid1, &thread1Attr, startTestTimer1, NULL);
	if (retVal != 0)
		printf ("Thread1 create error\n");
	
	//Create new thread 2
	pthread_attr_t thread2Attr;
	pthread_attr_init (&thread2Attr);
	//pthread_attr_setscope (&thread2Attr, PTHREAD_SCOPE_SYSTEM);
	//retVal = pthread_create (&tid2, &thread2Attr, startTestTimer2, NULL);
	if (retVal != 0)
		printf ("Thread2 create error\n");

	//Create new thread 3
	pthread_attr_t thread3Attr;
	pthread_attr_init (&thread3Attr);
	//pthread_attr_setscope (&thread3Attr, PTHREAD_SCOPE_SYSTEM);
	//retVal = pthread_create (&tid3, &thread3Attr, startTestTimer3, NULL);
	if (retVal != 0)
		printf ("Thread2 create error\n");


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
	timerValue.it_interval.tv_nsec = 10000000;

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
			printf("received SIGINT, count = %d\n", counter);
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
		long one;
		long two;
				
		printf("\t TimerId \tTimeUnits\t  Ab.StopOut Time\t\n");		

		for(int i = 0; i < MAX_THREAD1_TIMERS; i++)
		{
			one = (time_output[i].StartTime.tv_sec * 1000000)+
				(time_output[i].StartTime.tv_usec);

			two = (time_output[i].StopTime.tv_sec * 1000000)+
				(time_output[i].StopTime.tv_usec);

					
			//assigning the calculated values in Result array.
			time_result[i].timeUnits = time_output[i].timeUnits;
			
			time_result[i].stop_timer_diff = two - one;
			
			printf("\t %d \t\t%ld \t\t %ld \t\t \n",i,time_result[i].timeUnits, time_result[i].stop_timer_diff);
		}	
} // main

//---------------------------------------------------------------------
// Function     :startTestTimer1() 
// Description  :
//---------------------------------------------------------------------

void* startTestTimer1 (void*) 
{
	printf ("%s\n", __PRETTY_FUNCTION__);
	int i;
	CdSuTimerId timerId[ MAX_THREAD1_TIMERS ];
	CdSuTimerInfo timerInfo1;
	
	timerInfo1.timeCounts = 12;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId.srvcHlrId.srvcType = MODULE_P;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo1.timerCallBackFn = NULL;
	
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
	
	for (i = 0; i < MAX_THREAD1_TIMERS; i++)
	{
		
		int back = timerHandler->startTimer (timerId[ i ], timerInfo1);
						
		if (back == CDSU_TMR_RV_INV_TMRID)
		{
			printf ("Timer NOT started, id: %d\n", i);
		} else if (back == CDSU_TMR_RV_SUCCESS)
		{
			//printf ("Timer started, id: %d\n", timerId[i]);
		} else if(back == CDSU_TMR_RV_INV_CALLBACK)
		{
			printf ("Invalid Timer CallBack Function for id: %d\n", i);
		}
	} //for
	
	//To Stop all timers after starting them.
	int m;
	struct timeval tz1;
	for(m = 0 ;m < MAX_THREAD1_TIMERS; m++)
	{
		time_output[m].timeUnits = units;
		gettimeofday(&tz1, Timezone);
		
		int back = timerHandler->stopTimer(m);
		
		time_output[m].StartTime = tz1;
		gettimeofday(&tz1, Timezone);
		time_output[m].StopTime = tz1;
		if(back != 0)
			printf("StopTimer Failed for Id :%d\n",m);
	}

	//signal that thread is over
	pthread_kill (maintid, SIGINT);
	return 0;
}

//---------------------------------------------------------------------
// Function     :startTestTimer2() 
// Description  :
//---------------------------------------------------------------------

void* startTestTimer2 (void*) 
{
	printf ("%s\n", __PRETTY_FUNCTION__);

	CdSuTimerId timerId[ MAX_THREAD2_TIMERS ];
	CdSuTimerInfo timerInfo2;

	timerInfo2.timeCounts = 5;
	timerInfo2.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo2.destModuleId.srvcHlrId.srvcType = MODULE_Q;
	timerInfo2.opcode = 7;
	timerInfo2.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo2.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo2.timerCallBackFn = CallBackFunction;
	
	long units;
	switch(timerInfo2.timeUnits)
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
	units = units * timerInfo2.timeCounts;
	
	for (int i = 0; i < MAX_THREAD2_TIMERS; i++)
	{
		struct timeval tz;
		
		gettimeofday(&tz, Timezone);	
		
		int back = timerHandler->startTimer (timerId[i],
				timerInfo2);
		time_output[timerId[i]].StartTime = tz;	
		time_output[timerId[i]].timeUnits = units;
		
		gettimeofday(&tz, Timezone);
		
		time_output[timerId[i]].StopTime = tz;
		
		if (back == CDSU_TMR_RV_INV_TMRID)
		{
			printf ("Timer NOT started, id: %d\n", i);
		} else if (back == CDSU_TMR_RV_SUCCESS)
		{
			//printf ("Timer started, id: %d\n", timerId[i]);
		} else if(back == CDSU_TMR_RV_INV_CALLBACK)
		{
			printf ("Invalid Timer CallBack Function for id: %d\n", i);
		}
	} //for
	struct timeval tz1;
	int count = 0;
	// wait for messages from the TimerHandler
	while(count != MAX_THREAD2_TIMERS)
	{
		if (queue2->empty() == false)
		{
			CdSuTimerMsg* tmmsg = queue2->pop ();
			
			gettimeofday(&tz1, Timezone);

			//time_output[tmmsg->timerId].TimeOutTime = tz1;
			//printf("TO received for queue2:%d\n",tmmsg->timerId);
			delete tmmsg;
			count ++;
		}
		if(count == MAX_THREAD2_TIMERS )
			break;
	}

	pthread_kill (maintid, SIGINT);
	return 0;
}

//---------------------------------------------------------------------
// Function     :startTestTimer3() 
// Description  :
//---------------------------------------------------------------------

void* startTestTimer3 (void*)
{

	printf ("%s\n", __PRETTY_FUNCTION__);
	
	CdSuTimerId timerId[ MAX_THREAD3_TIMERS ];
	CdSuTimerInfo timerInfo3;

	timerInfo3.timeCounts = 6;
	timerInfo3.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo3.destModuleId.srvcHlrId.srvcType = MODULE_R;
	timerInfo3.opcode = 5;
	timerInfo3.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo3.timeoutReportType = CDSU_TMR_TRT_MESSAGE;
	timerInfo3.timerCallBackFn = NULL;
	
	long units;
	switch(timerInfo3.timeUnits)
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
	units  = units * timerInfo3.timeCounts;

	for (int i = 0;i < MAX_THREAD3_TIMERS ; i++)
	{	
		struct timeval tz;
		
		gettimeofday(&tz, Timezone);
				
		int back = timerHandler->startTimer (timerId[i], timerInfo3);
		time_output[timerId[i]].StartTime = tz;	
		time_output[timerId[i]].timeUnits = units;
		
		gettimeofday(&tz, Timezone);
		
		time_output[timerId[i]].StopTime = tz;
		
		if (back == CDSU_TMR_RV_INV_TMRID)
		{
			printf ("Timer NOT started, id: %d\n", i);
		} else if(back == CDSU_TMR_RV_INV_CALLBACK)
		{
			printf ("Invalid Timer CallBack Function for id: %d\n", i);
		}else if (back == CDSU_TMR_RV_SUCCESS)
		{
			//printf ("Timer started, id: %d\n", timerId[i]);
		}
	
	} //for
	
	struct timeval tz1;
	int count = 0;
	// wait for messages from the TimerHandler
	while(count != (MAX_THREAD3_TIMERS))
	{
		if (queue3->empty() == false)
		{
			CdSuTimerMsg* tmmsg = queue3->pop ();
			
			gettimeofday(&tz1, Timezone);

			//time_output[tmmsg->timerId].TimeOutTime = tz1;
						
			//printf("TO received for queue3:%d\n",tmmsg->timerId);
			delete tmmsg;
			count ++;
		}
		if(count == MAX_THREAD3_TIMERS)
			break;
	}

	
	pthread_kill (maintid, SIGINT);
	return 0;
}

//---------------------------------------------------------------------
// Function     : cdSuUserTimeoutHlr
// Description  : user handler function 
//--------------------------------------------------------------------
void cdSuUserTimeoutHlr (CdSuTimeout tempTimeOut)
{
	//CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	CdSuTimerMsg *ptr;
	ptr = new CdSuTimerMsg;
	
	ptr->destId = tempTimeOut.destModuleId;
	ptr->timerId = tempTimeOut.timerId;
	ptr->userInfo = tempTimeOut.userInfo;
	ptr->opcode = tempTimeOut.opcode;

	//add message in queue.
	if(ptr->destId.srvcHlrId.srvcType == MODULE_P){
		queue1->add(ptr);
		
	}
	else if(ptr->destId.srvcHlrId.srvcType == MODULE_Q){
		queue2->add(ptr);
	}
	else if(ptr->destId.srvcHlrId.srvcType == MODULE_R){
		queue3->add(ptr);
	}

//	printf ("Timeout id : %d\n", tempTimeOut.timerId);
	switch (ptr->destId.srvcHlrId.srvcType)
	{
	case MODULE_P:
		//printf ("Timeout for module P\n");
		break;
	case MODULE_Q:
		//printf ("Timeout for module Q\n");
		break;
	case MODULE_R:
		//printf ("Timeout for module R\n");
		break;
	default:
		//printf ("Invalid module\n");
		break;
	}

}	// void userTimeoutHandler ()

//---------------------------------------------------------------------
// Function     : CallBackFunction
// Description  : Used to make a message and send it back to client. 
// Input        : reference timerId and reference UserInfo.
//---------------------------------------------------------------------

void CallBackFunction (CdSuTimeout & tempTimeOut1)
{
	printf ("TimeoutId : %d\n", tempTimeOut1.timerId);
	//printf ("Callback function\n");
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
	//printf ("sigalrmHandler\n");
	//gettimeofday(Timeval, Timezone);
	//timeofday1 [j] = (Timeval->tv_sec + Timeval->tv_usec);
	
	timerHandler->timerTickHandler ();
	//j++;

	
}

//====================================================================
// <End of timertest.cpp>
//====================================================================
