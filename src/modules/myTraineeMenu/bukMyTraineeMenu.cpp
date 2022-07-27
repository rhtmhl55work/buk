//==============================================================================
// BUK assignment - (c) Copyright 2022, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	    : bukMyTraineeMenu.cpp
// Description	:
// Ref Docs	    :
// Author	    : rohitm
// Created On   : 22.07.2022
// Last Modified:
//==============================================================================

#include "bukMyTraineeMenu.h"

MyTraineeMenuMain* myTraineeMenuMain;			// Trainee Main
MyTraineeMenu myTraineeMenuObj;

//Trainee trainee[20];				// Maximum 20 trainees
//int noOfTrainee;			    	// Number of trainees present
//int startState;					    // to check the state of trainee main before registration
//int totalAck;					    // total number of trCmplt Acks sent to admin (total no. of trainees)

// for timer handling
//CdSuTimerId reportTimerId[20];
//CdSuTimerHandler *timerHandler;

int main ()
{
	CdSuTrace::setLevel (5);

//	sigset_t set;
//	sigfillset (&set);
//	sigaddset (&set, SIGINT);
//
//	if (pthread_sigmask (SIG_BLOCK, &set, NULL) != 0)
//	{
//	CDSU_TRACE (1, "signal mask error\n");
//	}

	// intializing number of trainees and state of trainee Main
//	noOfTrainee = 0;
//	startState = TRAINEE_INIT;

	// registering MyTraineeMenu MD and MyTraineeMenu NH Module ids
	basicinfraMyTraineeMenu();

	// starting MyTraineeMenu application
	createApplications ();

	//--------------------------------------------------------------------
	// Create Timer Thread.
	//--------------------------------------------------------------------

//	timerHandler = new CdSuTimerHandler;
//
//	// start the timer thread
//	pthread_t tThreadId;
//	pthread_attr_t tThreadAttr;
//	pthread_attr_init (&tThreadAttr);
//
//	CDSU_TRACE (1, "~~~~~~~~ Creating Timer thread~~~~~~~~~ \n");
//
//	if (pthread_create (&tThreadId, &tThreadAttr, timerThreadEntry, NULL) != 0)
//	{
//		CDSU_TRACE (1, "Timer thread create error\n");
//		return 0;
//	}

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

void createApplications ()
{
	myTraineeMenuMain = new MyTraineeMenuMain;
	myTraineeMenuMain->spawn (MyTraineeMenuMain::applicationStart);
} // createApplication ()

//============================================================================
// end of bukMyTraineeMenu.cpp
//============================================================================
