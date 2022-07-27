//==============================================================================
// BUK assignment - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	    : bukMentor.cpp
// Description	:
// Ref Docs	    :
// Author	    : Trainee - 2013
// Created On   : 13.11.2013
// Last Modified:
//==============================================================================

#include "bukMentor.h"

Mentor mentor[20];			//Maximum number of mentor objects
int noOfMentors;			//Number of mentors present
int startState;				//State of mentor before registration

//for timer handling
CdSuTimerId workTimerId[20];
CdSuTimerId reportTimerId[20];
CdSuTimerHandler *timerHandler;
struct timezone* Timezone;

//initialize flags
int trCmpltFlag = 0;
int count = 0;
int totalAck = 0;

int main ()
{
	CdSuTrace::setLevel (5);

	//signal handling for timer
	sigset_t set;
	sigfillset (&set);
	sigaddset (&set, SIGINT);

	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
	{
	CDSU_TRACE (1, "Signal mask error\n");
	}

	//initialize number of mentors and mentor-state
	noOfMentors = 0;
	startState = MENTOR_INIT;

	//for registration of mentor MD and NH
	basicinfraMentor();

	//for starting mentor application
	createApplications ();

	//timer handler
	timerHandler = new CdSuTimerHandler;

	// create and start the timer thread
	pthread_t tThreadId;
	pthread_attr_t tThreadAttr;
	pthread_attr_init (&tThreadAttr);

	CDSU_TRACE (1, "~~~~~~~~ Creating Timer thread~~~~~~~~~ \n");

	if (pthread_create (&tThreadId, &tThreadAttr, timerThreadEntry, NULL) != 0)
	{
		CDSU_TRACE (1, "Timer thread create error\n");
		return 0;
	}

	while (1)
	{
		CdSuTsQueue <int> queue (5);
		int i = 0;
		queue.popBlocking (i);
	}
	while (1)
	{
		; // Intentional
	}

} // main ()


//---------------------------------------------------------------------------
// createApplications ()
// Description  : Creation of application Thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

MentorMain* mentorMain;

void createApplications ()
{
	// Create Application Thread
	mentorMain = new MentorMain;
	mentorMain->spawn (MentorMain::applicationStart);
} // createApplication ()

//============================================================================
// End of bukMentor.cpp
//============================================================================
