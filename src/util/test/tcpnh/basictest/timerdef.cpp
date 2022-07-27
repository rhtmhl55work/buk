//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : timerdef.cpp
// Description  : Timer definitions 
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include "timerdef.h"
#include "cdsutrace.h"

#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/thread.inl>
#endif

// globals
pthread_t cdTimerThreadId;
pthread_attr_t cdTimerThreadAttr;
CdSuTimerHandler* cdSuTimerHandler;

#ifdef __ECOS
// for eCos native timer
#define TIMER_ANY 0xFFFFFFFF
inline void *operator new (size_t size,  cyg_uint8 *ptr) { return (void *)ptr; };
static cyg_uint8 alarm_obj [sizeof(Cyg_Alarm)];
void alarm_action (Cyg_Alarm*, CYG_ADDRWORD data);
externC cyg_tick_count cyg_timespec_to_ticks (const struct timespec *tp,
		cyg_bool roundup = false);
cyg_flag_t timerFlag;
#endif

//----------------------------------------------------------------------------
// Function	: cdCreateTimerHandler
// Description	: create the timer handler
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

int cdCreateTimerHandler ()
{
	// cdSuTimerHandler = new CdSuTimerHandler;
	return 1;
}

//----------------------------------------------------------------------------
// Function	: cdDeleteTimerHandler
// Description	: delete the timer handler
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

int cdDeleteTimerHandler ()
{
	delete cdSuTimerHandler;
	return 1;
}

//----------------------------------------------------------------------------
// Function	: cdCreateTimerThread
// Description	: creates the timer thread
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

int cdCreateTimerThread ()
{
	// create timer thread
	pthread_attr_init (&cdTimerThreadAttr);
#ifdef sun
	pthread_attr_setscope (&cdTimerThreadAttr, PTHREAD_SCOPE_SYSTEM);
#endif
	if (pthread_create (&cdTimerThreadId, &cdTimerThreadAttr, cdTimerThreadEntry, NULL) != 0)
	{
		CDSU_TRACE (1, "timer thread create error\n");
		return -1;
	}
	return 1;
}

//----------------------------------------------------------------------------
// Function	: cdDeleteTimerThread
// Description	: deletes the timer thread
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

int cdDeleteTimerThread ()
{
	if (pthread_cancel (cdTimerThreadId) != 0)
	{
		CDSU_TRACE (1, "timer thread cancel error\n");
		return -1;
	}
	return 1;
}

//---------------------------------------------------------------------------
// Function	: sigalrmHandler () 
// Description  : sigalrm handler
// Inputs       : None
// Return Value : None
// Side Effects : None 
// Algorithm    :
//---------------------------------------------------------------------------

#if defined (__ECOS) || defined (sun)
void sigalrmHandler (int signum, siginfo_t* info, void* context)
#else
void sigalrmHandler (int signum)
#endif
{
	cdSuTimerHandler->timerTickHandler();
#if !defined (__ECOS) && !defined (sun)
        signal (SIGALRM, sigalrmHandler);
#endif
} // sigalrmHandler ()

//---------------------------------------------------------------------------
// Function	: cdTimerThreadEntry () 
// Description  : timer thread
// Inputs       : None
// Return Value : None
// Side Effects : None 
// Algorithm    :
//---------------------------------------------------------------------------

void* cdTimerThreadEntry (void*)
{
// NOTE: timer create/start and sigwait should be in the same thread in linux
#ifdef __ECOS
	// start native timer
	Cyg_Alarm* myalarm = new (alarm_obj) Cyg_Alarm( Cyg_Clock::real_time_clock,
				  alarm_action, (cyg_addrword_t) NULL);
	// timer values
	struct itimerspec timerValue;
	timerValue.it_value.tv_sec           = 0;
	timerValue.it_value.tv_nsec          = 10000000; // 10 milli-sec
	timerValue.it_interval.tv_sec        = 0;
	timerValue.it_interval.tv_nsec       = 10000000;

	cyg_tick_count trigger = cyg_timespec_to_ticks
				(&timerValue.it_value, true);
	cyg_tick_count interval = cyg_timespec_to_ticks
				(&timerValue.it_interval, true);
	trigger += Cyg_Clock::real_time_clock->current_value();

	// Set the alarm running
	myalarm->initialize( trigger, interval );
#elif sun
	// install signal handler
	// block all signals when signal handler is active
	sigset_t mask;	
	sigfillset (&mask );
	struct sigaction sa;
	sa.sa_sigaction = sigalrmHandler; // on signal handler function
	sa.sa_mask = mask;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_flags |= SA_RESTART;
	
	// specify the action associated with a signal
	sigaction (SIGALRM, &sa, NULL);

	// notification type
	struct sigevent sev;
	sev.sigev_notify                = SIGEV_SIGNAL;
	sev.sigev_signo                 = SIGALRM;
	sev.sigev_value.sival_int       = 0xABCDEF01;
		
	// create and start timer
	timer_t timerId;
	if (timer_create (CLOCK_REALTIME, &sev, &timerId) < 0)
	{
		CDSU_TRACE (1, "timer create error\n");
	}

	// timer values
	struct itimerspec timerValue;
	timerValue.it_value.tv_sec           = 0;
	timerValue.it_value.tv_nsec          = 10000000;
	timerValue.it_interval.tv_sec        = 0;
	timerValue.it_interval.tv_nsec       = 10000000;

	if (timer_settime (timerId, 0, &timerValue, NULL ) < 0)
	{
		CDSU_TRACE (1, "timer settime error\n");
	}
#else	// linux
	
	// start timer
	itimerval itval, otval;
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_usec = 10000;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_usec = 10000;

	// if (setitimer (ITIMER_REAL, &itval, &otval) < 0)
	// {
	// 	CDSU_TRACE (1, "setitimer error\n");
	// }
#endif

	while (1)
	{
#ifdef __ECOS
		// wait for the flag and then call signal handler
		int flag = cyg_flag_wait (&timerFlag, TIMER_ANY,
			CYG_FLAG_WAITMODE_OR|CYG_FLAG_WAITMODE_CLR);
		sigalrmHandler (SIGALRM, 0, 0);
#elif sun 
		// unblock SIGALRM  and signal handler will be invoked directly
		// TODO: change this to sigwait
		sigset_t set;
		sigemptyset (&set);
		sigaddset (&set, SIGALRM);
		if (pthread_sigmask (SIG_UNBLOCK, &set, NULL) != 0)
		{
			CDSU_TRACE (1, "signal mask error\n");
		}
#else		// linux, wait for SIGALRM
		sigset_t set;
		sigemptyset (&set);
		sigaddset (&set, SIGALRM);
		int signo;
		int retVal;
		do
		{
			retVal = sigwait (&set, &signo);
			if (retVal == 0)
			{
				sigalrmHandler (signo);
			}

		} while (retVal == -1 &&  errno == EINTR);
		if (retVal != 0)
		{
			CDSU_TRACE (1, "sigwait error\n");
		}

#endif
	}
	return NULL;	// placate compiler	
} // cdTimerThreadEntry

//---------------------------------------------------------------------------
// Function	: cdSuTimerHandler () 
// Description  :
// Inputs       : None.
// Return Value : None.
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------

void cdSuUserTimeoutHlr (CdSuTimeout toData)
{
//--------------------------< BEGIN >-----------------------------------------
// TODO: Add timeout handling here
//---------------------------< END >------------------------------------------
} // cdSuUserTimeoutHlr

//----------------------------------------------------------------------------
// Function	: alarm_action
// Description	: Alarm action called every 1 Sec from the native eCos
//----------------------------------------------------------------------------

#ifdef __ECOS
void alarm_action (Cyg_Alarm *ca, CYG_ADDRWORD data)
{
	cyg_flag_setbits (&timerFlag, 1);
}
#endif

//============================================================================
// <End of timerdef.cpp
//============================================================================
