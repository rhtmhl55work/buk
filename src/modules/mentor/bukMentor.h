#ifndef __BUK_MENTOR_H__
#define __BUK_MENTOR_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename 	: bukiMentor.h
// Description	:
// Ref Docs	    :
// Author	    : Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified:
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include<time.h>
#include<sys/time.h>

#include <cdsuthread.h>
#include "cdsunhudpapi.h"
#include "cdsumdapi.h"
#include "cdsutrace.h"
#include "cdsuutiltypes.h"
#include "cdsuinthash.h"
#include "cdsuqueue.h"
#include "cdsutimer.h"
#include <signal.h>
#include <pthread.h>

#include "basicinfra.h"
#include "basicinfraparam.h"

#include "bukUiMentorMsgs.h"
#include "bukAdminMentorMsgs.h"
#include "bukMentorTraineeMsgs.h"


#define MAX_MENTOR 20
#define MAX_LOST_REPORT 3

//============================================================================
//Message Opcodes
//============================================================================

typedef struct BukMtRprtRqst : public CdMessage
{
	int mId;

	void initMsg (CdModuleId dest, CdModuleId src, int mid)
	{
		msgHdr.dst = dest;
		msgHdr.src = src;
		msgHdr.msgLen = sizeof (BukMtRprtRqst);
		msgHdr.opcode = BUK_UI_MT_RPRTRQST;
		mId = mid;
	}

}BukMtRprtRqst; //struct BukMtRprtRqst

enum State
{
	//--------------------------------------------------------------------
	// States of Mentor
	//--------------------------------------------------------------------

	MENTOR_INIT = 0,
	MENTOR_REGISTERED,
	MENTOR_WORKING,
	MENTOR_WORKING_TRAINING,
	MENTOR_TRAINING,
	MENTOR_TERMINATED
};

enum MsgSrc
{
	//--------------------------------------------------------------------
	// Message sources
	//--------------------------------------------------------------------

	MENU = 0,
	ADMIN,
	TRAINEE
};

//definition of class mentor
class Mentor
{
	public:
		Mentor();                               //constructor
		~Mentor();
		void handleMsg(CdMessage*, int, void*);
		void handleTimer(int);

	protected:
	private:
		int mtId;
		int trId;
		int state;

		int getState ();
		void setState (int rcvdState);

		int getTraineeId ();
		void setTraineeId (int);

		int getMentorId ();
		void setMentorId (int);

		void timerReset();

		void startPeriodicReportTimer (int);
		void startWorkTimer (int);
}; //class Mentor

//using CdSuThread
class MentorMain:public CdSuThread
{
public:
	static void* applicationStart (void*);
	void msgDistr ();

private:
	void msgFromMenu (CdMessage*);
	void msgFromAdmin (CdMessage*);
	void msgFromTrainee (CdMessage*);
};

extern MentorMain* mentorMain;
extern Mentor mentor[20];

extern int noOfMentors;
extern int startState;

extern CdSuTimerId workTimerId[20];
extern CdSuTimerId reportTimerId[20];

extern CdSuTimerHandler* timerHandler;

extern int trCmpltFlag;			// to indicate training completion while in working_training state
extern int count;				// to keep count of periodic report timer expiry
extern int totalAck;            // to keep count of total ack send to admin

void smMgmtEventHlr (void*);
void applEventHlr (void*);

// for creating applications (threads)
void createApplications ();

void workTimerExpires(CdSuTimeout& timeout);
void periodicReportTimerExpires(CdSuTimeout& timeout);

void* timerThreadEntry(void*);
void sigalrmHandler (int);

#endif

//==============================================================================
// <End of bukMentor.h>
//==================:============================================================
