#ifndef __BUK_ADMIN_H__
#define __BUK_ADMIN_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	    : bukAdmin.h
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

#include <cdsuthread.h>
#include "cdsunhudpapi.h"
#include "cdsumdapi.h"
#include "cdsutrace.h"
#include "cdsuutiltypes.h"
#include "cdsuinthash.h"
#include "cdsuqueue.h"
#include "basicinfra.h"
#include "basicinfraparam.h"

#include "bukUiAdminMsgs.h"
#include "bukAdminMentorMsgs.h"
#include "bukAdminTraineeMsgs.h"

using namespace std;

//============================================================================
//Message Opcodes
//============================================================================

typedef struct BukAdWrktoMtMsg : public CdMessage
{
	int mId;

	void initMsg (CdModuleId dest, CdModuleId src, int mid)
	{
		msgHdr.dst = dest;
		msgHdr.src = src;
		msgHdr.msgLen = sizeof (BukAdWrktoMtMsg);
		msgHdr.opcode = BUK_UI_AD_WRKTOMT;
		mId = mid;
	}

}BukAdWrktoMtMsg; //struct BukAdWrktoMtMsg

enum State
{
	//--------------------------------------------------------------------
	// States of Admin
	//--------------------------------------------------------------------

	ADMIN_INIT = 0,
	ADMIN_MONITOR,
	ADMIN_WAIT_TERMINATED,
	ADMIN_TERMINATED
};

enum MsgSrc
{
	//--------------------------------------------------------------------
	// Message sources
	//--------------------------------------------------------------------

	MENU = 0,
	MENTOR,
	TRAINEE
};

//============================================================================
//Queue and Hash
//============================================================================
extern CdSuIntHash<int> intHash ;
extern CdSuQueue <int> queueTr;
extern CdSuQueue <int> queueMr;
extern CdSuQueue<int>::iterator iter1;
extern CdSuQueue<int>::iterator iter2;

//..........................Class Admin.....................
class Admin:public CdSuThread
{
public:
	Admin(); 			   //constructor
	~Admin();
	void handleMsg(CdMessage*);

private:
	int state;

	int getState ();
	void setState (int rcvdState);

	void msgFromMenu(CdMessage*);
	void msgFromMentor(CdMessage*);
	void msgFromTrainee(CdMessage*);

	int mapMtTrFunction ();
	void sendMapping ();
};

//using CdSuThread
class AdminMain:public CdSuThread
{
public:
	static void* applicationStart (void*);
	void msgDistr();				// for distributing messages

private:
};

extern AdminMain* adminMain;
extern Admin admin;

extern int totalAck;

// functions for network handler
void smMgmtEventHlr (void*);
void applEventHlr (void*);

// for creating applications (threads)
void createApplications ();

#endif

//==============================================================================
// <End of bukAdmin.h>
//==================:============================================================
