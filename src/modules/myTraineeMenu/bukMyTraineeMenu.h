#ifndef __BUK_MYTRAINEEMENU_H__
#define __BUK_MYTRAINEEMENU_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukMyTraineeMenu.h
// Description	:
// Ref Docs	    :
// Author	    : rohitm
// Contributors	:
// Created On	: 22.07.2022
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
#include <signal.h>
#include <pthread.h>

#include <cdsuthread.h>
#include "cdsunhudpapi.h"
#include "cdsumdapi.h"
#include "cdsutrace.h"
#include "cdsuutiltypes.h"
#include "cdsuinthash.h"
#include "cdsuqueue.h"
#include "cdsutimer.h"

#include "basicinfra.h"
#include "basicinfraparam.h"

#include "bukMyTraineeMenuTraineeMsgs.h"
//#include "bukAdminTraineeMsgs.h"
//#include "bukMentorTraineeMsgs.h"


//#define MAX_TRAINEE 20							//Number of Maximum Trainees

//============================================================================
//Message Opcode for sending message to MyTraineeMenu NH to MyTraineemenu MD
//============================================================================

//typedef struct BukMyTraineeMenuReq : public CdMessage
//{
//	int someId;
//
//	void initMsg (CdModuleId dest, CdModuleId src, int reqId)
//	{
//		msgHdr.dst = dest;
//		msgHdr.src = src;
//		msgHdr.msgLen = sizeof (BukTrGdncRqst);
//		msgHdr.opcode = BUK_UI_TR_GDNCRQST;
//		this->reqId = reqId;
//	}
//
//}BukMyTraineeMenuReq; //struct BukMyTraineeMenuReq

//enum State
//{
//	//--------------------------------------------------------------------
//	// States of Trainee
//	//--------------------------------------------------------------------
//
//	TRAINEE_INIT = 0,
//	TRAINEE_REGISTERED,
//	TRAINEE_TRAINING,
//	TRAINEE_TERMINATED
//};

enum MsgSrc
{
	//--------------------------------------------------------------------
	// Message sources
	//--------------------------------------------------------------------

	TRAINEE = 0
};

//class MyTraineeMenu Main 
class MyTraineeMenuMain:public CdSuThread
{
public:
	static void* applicationStart (void*);			// to start MD and NH
	void msgDistr();            					// to handle all messages receive at MyTraineeMenu MD

protected:

private:
//	void msgFromMenu (CdMessage*);	    			// to handle all messages from Trainee Menu
//	void msgFromAdmin (CdMessage*); 	    		// to handle all messages from Admin
	void msgFromTrainee (CdMessage*);		    	// to handle all messages from Trainee

};

//definition of class MyTraineeMenu
class MyTraineeMenu
{
	public:
		MyTraineeMenu();		    			            // constructor
		~MyTraineeMenu();					                // destructor

		void handleMsg(CdMessage*, int, void*);		// function to handle messages
//		void handleTimer();				            // function to handle timers
	protected:

	private:
//		int mtId;               					// to store mentor id
//		int trId;				                	// to store trainee id
//   		int state;					                // to store trainee state
//
//		int getState ();			            	// to get the state of trainee
//		void setState (int rcvdState);			    // to set the state of trainee
//
//		int getTraineeId ();				        // to get the id of the trainee
//		void setTraineeId (int);	        		// to set the id of the trainee
//
//		int getMentorId ();				            // to get the id of the mentor
//		void setMentorId (int);				        // to set the id of the mentor
//
//		void startPeriodicReportTimer (int);		// to start periodic report timer

}; //class MyTraineeMenu

extern MyTraineeMenuMain* myTraineeMenuMain;				    // TraineeMenu Main to handle all trainees
//extern Trainee trainee[20];					        // Max 20 trainees
extern MyTraineeMenu myTraineeMenuObj;					        // object of class MyTraineeMenu
//
//extern int noOfTrainee;						        // Number of trainees present
//extern int startState;					        	// to check the allover state before registration
//extern int totalAck;						        // total numbers of trCmplt Ack sent to admin

//extern CdSuTimerId reportTimerId[20];				// to store report time id
//extern CdSuTimerHandler* timerHandler;				// timer handler

// for creating applications (threads)
void createApplications ();

// for timer handling
//void sigalrmHandler (int xx);
//void* timerThreadEntry (void*);
//void periodicReportTimerExpires(CdSuTimeout& timeout);

#endif

//============================================================================
// End of bukTrainee.h
//============================================================================
