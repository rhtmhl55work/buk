//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : sup-test.cpp
// Description  : Timer support test file
// Author       : Indu Mishra
// Created On   : Wed Feb 12 15:29:15 IST 2003
// Last Modified:
//============================================================================
#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/infra/diag.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

#if defined (__ECOS)
#define printf diag_printf
#endif

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

#if defined (__ECOS) || defined (sun)
extern void sigalrmHandler (int signo, siginfo_t* info, void* context);
#else
extern void sigalrmHandler (int xx);
#endif

//----------------------------------------------------------------------------
// Declarations specific to eCos native timer
//----------------------------------------------------------------------------
#ifdef __ECOS
#define TIMER_ANY 0xFFFFFFFF
static cyg_uint8 alarm_obj [sizeof(Cyg_Alarm)];
Cyg_Alarm* myalarm;			// eCos alarm object
struct itimerspec timerValue;		// Timeout value on eCos
void alarm_action (Cyg_Alarm*, CYG_ADDRWORD data);
inline void *operator new(size_t size,  cyg_uint8 *ptr) { return (void *)ptr; };
externC cyg_tick_count cyg_timespec_to_ticks( const struct timespec *tp,
                                         cyg_bool roundup = false);
cyg_flag_t timerFlag;
#endif

//----------------------------------------------------------------------------
// Function definitions 
//----------------------------------------------------------------------------
void* timerThreadEntry (void*);

//----------------------------------------------------------------------------
// Function Name : timer thread
// Description   : Used to initialise the timer
// ---------------------------------------------------------------------------
void* timerThreadEntry (void*)
{
// NOTE: timer create/start and sigwait should be in the same thread in linux

#ifdef __ECOS
	// create timers
	struct itimerspec timerValue;	// Timeout value on eCos
	myalarm = new (alarm_obj) Cyg_Alarm( Cyg_Clock::real_time_clock,
				  alarm_action, (cyg_addrword_t) NULL);
	// timer values
	timerValue.it_value.tv_sec           = 0;
	timerValue.it_value.tv_nsec          = 10000000; // = 100 milli-sec
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
	sa.sa_sigaction = sigalrmHandler; // on signal this fn gets invoked.
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
		printf("timer create error\n");
	}

	// timer values
	struct itimerspec timerValue;
	timerValue.it_value.tv_sec           = 0;
	timerValue.it_value.tv_nsec          = 10000000;
	timerValue.it_interval.tv_sec        = 0;
	timerValue.it_interval.tv_nsec       = 10000000;

	if (timer_settime (timerId, 0, &timerValue, NULL ) < 0)
	{
		printf("timer settime error\n");
	}
#else // linux
	
	// start timer
	itimerval itval, otval;
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_usec = 10000;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_usec = 10000;

	if (setitimer (ITIMER_REAL, &itval, &otval) < 0)
	{
		printf("setitimer error\n");
	}
#endif

	while (1)
	{
#ifdef __ECOS
		int flag = cyg_flag_wait (&timerFlag, TIMER_ANY,
			CYG_FLAG_WAITMODE_OR|CYG_FLAG_WAITMODE_CLR);
		sigalrmHandler (SIGALRM, 0, 0);
#elif sun 
	// unblock SIGALRM 
	sigset_t set;
	sigemptyset (&set);
	sigaddset (&set, SIGALRM);
	if (pthread_sigmask (SIG_UNBLOCK, &set, NULL) != 0)
	{
		printf("signal mask error\n");
	}
	// signal handler will be invoked directly
#else	// linux
		sigset_t set;
		// wait for SIGALRM
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

			printf("sigwait error\n");
		}
#endif
	}
	return NULL;	// placate compiler	

}// void* timerThreadEntry(void*)


#ifdef __ECOS
//----------------------------------------------------------------------------
// Alaram action called every 1 Sec from the native eCos
//----------------------------------------------------------------------------
void alarm_action (Cyg_Alarm *ca, CYG_ADDRWORD data)
{
	cyg_flag_setbits (&timerFlag, 1);
}
#endif

