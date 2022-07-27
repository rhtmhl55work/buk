//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : timer.cpp
// Description  : Timer functions.
// Ref Docs     : 
// Author       : Indu Mishra
// Contributor  : 
// Created On   :  
// Last Modified: 3rd December, 2003.
// Modified By  : Seema Jain.
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
// Notes        : NIL
//============================================================================

//============================================================================
// Include Header Files
//============================================================================
#include "cdsutimer.h"
#include "cdsutimingwheel.h"


//----------------------------------------------------------------------------
// Name		: CdSuTimerMsg::initMsg ()
// Description	: Initialization of CdSuTimerMsg.
//----------------------------------------------------------------------------
void CdSuTimerMsg::initMsg (const CdModuleId src, const CdModuleId dst,
		const CdSuTimerId timerId,
		const Uint32* timerInfo )
		
{
	this->msgHdr.msgLen = sizeof (CdSuTimerMsg);
	this->msgHdr.dst = dst;
	this->msgHdr.src = src;
	this->timerId = timerId;
	memcpy((this->userInfo), timerInfo, CDSU_TMR_MAX_USERINFO_NUM );

} // CdSuTimerMsg::initMsg ()

//============================================================================
// Method Definitions of CdSuTimerHandler Class.                            
//============================================================================
//----------------------------------------------------------------------------
// Function     : CdSuTimerHandler::CdSuTimerHandler ()
// Description  : Constructor of CdSuTimerHandler.
//----------------------------------------------------------------------------

CdSuTimerHandler::CdSuTimerHandler ()
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	timeWheel = new CdSuTimingWheel (this, CDSU_TIMEWHEEL_HASH_TABLE_SIZE);
	timerHash = new CdSuIntHash <CdSuTnode*> (CdSuIntHash<CdSuTnode*>::TZ_4093);
	keyAlloc = new CdSuKeyAllocator ();

	// Initialise Mutex.
	pthread_mutex_init (&timerMutex, NULL);
	
}// CdSuTimerHandler::CdSuTimerHandler ()

//----------------------------------------------------------------------------
// Function     : CdSuTimerHandler::~CdSuTimerHandler
// Description  : Destructor of CdSuTimerHandler.
//----------------------------------------------------------------------------

CdSuTimerHandler::~CdSuTimerHandler ()
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

        delete timeWheel;
	delete timerHash;
	delete keyAlloc;

}// CdSuTimerHandler::~CdSuTimerHandler ()	

//----------------------------------------------------------------------------
// Function     : CdSuTimerHandler::startTimer
// Description  : Start a timer.
// Inputs	: TimerId reference, UserInfo reference	
// Algorithm:
//	0. Lock the mutex.
// 	1. Verify for the validity of the Inputs parameters.
//	2. Determine keyval from Hash function. 
//	3. Add keyval and timerInfo in Tnode in timeque.
//	4. Add new node address into the hash position with that keyval. 
//      5. Return with return values.They are handled in the Client code.
//	   Don't forget to unlock the mutex.
//----------------------------------------------------------------------------

CdSuTmrRetVal CdSuTimerHandler::startTimer (CdSuTimerId& timerId, 
				const CdSuTimerInfo& userTimerInfo)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	//--------------------------------------------------------------------
	// Lock the Mutex first before doing anything.
	//--------------------------------------------------------------------
	int mutexRet1 = pthread_mutex_lock (&timerMutex);
	if (mutexRet1 != 0)
	{
		//CDSU_TRACE1 (1, "CdSuTimerHandler::startTimer(): Error! Mutex lock failed:%d\n",
		//	mutexRet1);
		return (CDSU_TMR_RV_INV_MUTEXLOCK);
	}
	
	//--------------------------------------------------------------------
	// Check the UserTimerInfo for validity of parameters.
	// In case of invalidity, unlock mutex and return.
	//--------------------------------------------------------------------
  	Uint32 keyVal = CDSU_TMR_INV_TIMER_ID;
	CdSuTmrRetVal retVal = CDSU_TMR_RV_SUCCESS;

	if ((userTimerInfo.timeCounts <= 0))
	{ 
		retVal = CDSU_TMR_RV_INV_TIME_COUNT;
	}
	
	if ((userTimerInfo.timeUnits < CDSU_TMR_TU_TENMS) || 
		(userTimerInfo.timeUnits > CDSU_TMR_TU_HOUR ))
	{
		retVal = CDSU_TMR_RV_INV_TIME_UNIT;
	}
	
	if ((userTimerInfo.timerType != CDSU_TMR_TT_TRIGGER_ONCE) &&
			(userTimerInfo.timerType != CDSU_TMR_TT_CYCLIC))
	{
		retVal = CDSU_TMR_RV_INV_TMRTYPE;
	}
	
	if ((userTimerInfo.timeoutReportType != CDSU_TMR_TRT_CALLBACK) &&
			(userTimerInfo.timeoutReportType != CDSU_TMR_TRT_MESSAGE))
	{
		retVal = CDSU_TMR_RV_INV_TMRREPTYPE;
	}
	
	if (userTimerInfo.timeoutReportType == CDSU_TMR_TRT_CALLBACK)
	{
		if (userTimerInfo.timerCallBackFn == NULL)
			retVal = CDSU_TMR_RV_INV_CALLBACK;
	}
	if (retVal != CDSU_TMR_RV_SUCCESS)
	{
		int mutexRet2 = pthread_mutex_unlock (&timerMutex);
		if (mutexRet2 != 0)
		{
			// CDSU_TRACE1 (1, "CdSuTimerHandler::startTimer(): Error! Mutex unlock failed:%d\n",
			//	mutexRet2);
		}
		return (retVal);
	}
		
		
	//--------------------------------------------------------------------
	// Get an unused timerId.
	//--------------------------------------------------------------------
	bool found = true;
	while (found)
	{
		//CDSU_TRACE(4, "Key Gen\n");
		// Get keyVal and check whether keyval is already in use in 
		// hashtable or not.
		CdSuTnode* valueP;

		keyVal = keyAlloc->KeyGenerator ();
		if (timerHash->lookup ((Uint32)keyVal, valueP )) 
		{
			//CDSU_TRACE1 (4, "TimerHandler::startTimer(): KeyId = %d already in use. Trying again.\n", keyVal);
		}else 
		{
			found = false;
		}
	} // while

	//--------------------------------------------------------------------
	// * Add and entry into the "timing Wheel".
	// * Add the node address returned by the timing wheel into the
	//   timerId -> timingwheelNode Table.
	// * Set the timerId.
	//--------------------------------------------------------------------
	// TimerId: return value.
	timerId = keyVal;
	
	CdSuTnode* nodeVal = timeWheel->add (userTimerInfo, timerId);
	
	if(timerHash->insert (keyVal, nodeVal))
	{
		//CDSU_TRACE(4, "StartTimer : timerHash insert Success\n");
	}else
	{
		//CDSU_TRACE(4, "StartTimer : timerHash insert Failure\n");
		retVal = CDSU_TMR_RV_INV_TMRHASHINSERT;
	}

	//--------------------------------------------------------------------
	// Through with everything. Unlock mutex and return.
	//--------------------------------------------------------------------
	int mutexRet2 = pthread_mutex_unlock (&timerMutex);
	if (mutexRet2 != 0)
	{
		//CDSU_TRACE1 (1, "CdSuTimerHandler::startTimer(): Error! Mutex unlock failed:%d\n",
		//	mutexRet2);
	}

	return (retVal);

} // CdSuTimerHandler::startTimer ()

//----------------------------------------------------------------------------
// Function     : CdSuTimerHandler::stopTimer
// Description  : Cancel a timer.
// Inputs	: TimerId
// Algorithm:
//      1. Using TimerId determine node address from hashtable.
//	2. Using node address, delete it from the TimingWheeel.
//	3. Delete entry from hashtable.
//	4. Return return values which may be success or denoting Error No.
//----------------------------------------------------------------------------

CdSuTmrRetVal CdSuTimerHandler::stopTimer (const CdSuTimerId timerId)
{
	// CDSU_TRACE2 (4, "%s: timerId = %d\n", __PRETTY_FUNCTION__, timerId);

	//--------------------------------------------------------------------
	// Lock the Mutex first before doing anything.
	//--------------------------------------------------------------------
	int mutexRet1 = pthread_mutex_lock (&timerMutex);
	if (mutexRet1 != 0)
	{
		//CDSU_TRACE1 (1, "CdSuTimerHandler::stopTimer(): Error! Mutex lock failed:%d\n",
		//	mutexRet1);
		return (CDSU_TMR_RV_INV_MUTEXLOCK);
	}

	//--------------------------------------------------------------------
        // Check for timerId in hashtable. In case it is present, then 
	// delete it from TimingWheel as well as from hashtable.
	//--------------------------------------------------------------------
	CdSuTmrRetVal retVal = CDSU_TMR_RV_INV_TMRID;
	CdSuTnode* tmpvalue ;
	
	if (timerHash->lookup (timerId, tmpvalue))
	{
		//CDSU_TRACE2 (4, "StopTimer : node address: %d, timerId is %d\n",
		//	tmpvalue, timerId);
	
		bool queueDel = timeWheel->del (tmpvalue);
		bool isDelete = timerHash->remove (timerId);
		
		if(isDelete && queueDel)
		{
			retVal = CDSU_TMR_RV_SUCCESS;
		}else if (!queueDel){
			retVal = CDSU_TMR_RV_INV_TMRWHEELDEL;
			}
		else if (!isDelete){
			retVal = CDSU_TMR_RV_INV_TMRHASHDEL;
			}
		
	}else 
	{
		retVal = CDSU_TMR_RV_INV_TMRID;
	}

	//mutex unlock
	int mutexRet2 = pthread_mutex_unlock (&timerMutex);
	if (mutexRet2 != 0)
	{
		//CDSU_TRACE1 (1, "CdSuTimerHandler::stopTimer(): Error! Mutex unlock failed:%d\n",
		//	mutexRet2);
	}

	return (retVal);

} // CdSuTimerHandler::stopTimer ()

//----------------------------------------------------------------------------
// Function     : timerTickHandler
// Description  :
//----------------------------------------------------------------------------

void CdSuTimerHandler::timerTickHandler (void)
{
	//CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	// Lock Mutex.
	if(pthread_mutex_lock (&timerMutex))
	{
		//CDSU_TRACE (1, "timerTickHandler : Error! Mutex lock failed\n");
	}
	// Call tickDecrement Count Function.
        timeWheel->tickDecCount ();

	// Unlock Mutex.
	if(pthread_mutex_unlock (&timerMutex))
	{
		//CDSU_TRACE (1, "timerTickHandler : Error! Mutex unlock failed\n");	
	}

} // CdSuTimerHandler::timerTickHandler ()

//============================================================================
//   Tnode
//============================================================================

//----------------------------------------------------------------------------
// Function  : CdSuTnode () 
// Algorithm :
// Each timer node is a struct of Tnode type. 
//      timeoutDestId   : Id of entity making the timer request
//      timerId2        : timerId
//      tickCount	: Its the difference of Quotient of the two nodes.
//      userInfo        : Info specified by the user.
//	opcode		: opcode used by destination
//	timerType	: cyclic or trigger once or invalid.
//	timeoutReportType : what to perform usinf timeouthlr is decide by this
//			   parameter.
//----------------------------------------------------------------------------

CdSuTnode::CdSuTnode (const CdSuTimerInfo& timerInfo, CdSuTimerId timerId, Uint32 tempCount)
{	
	previous = (CdSuTnode*) NULL;
	next = (CdSuTnode*) NULL;
	
	//timeUnits and timeCounts are stored for Cyclic timers. 
	initTimeUnits = timerInfo.timeUnits;
	initTimeCounts = timerInfo.timeCounts;
 	
	offset = 0;		//offset is the hash position of timingwheel 
				//will be assigned when a node is created.
	
	tickCount = tempCount;         //quotient of ticks and size of hash.
	memcpy ((char*) &userInfo[0], (char*) &timerInfo.userInfo[0], 
		sizeof (userInfo));
	timeoutDestId = (timerInfo.destModuleId);
	opcode = timerInfo.opcode;
	this->timerId = timerId;
	timerType = timerInfo.timerType;
	timeoutReportType = timerInfo.timeoutReportType;
	timerCallBackFn = timerInfo.timerCallBackFn;

} // CdSuTnode::CdSuTnode ()

//-----------------------------------------------------------------------
// Function    : CdSuTnode::~CdSuTnode ()
// Description : CdSuTnode destructor.      
//---------------------------------------------------------------------------

CdSuTnode::~CdSuTnode ()
{
	//CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

} // CdSuTnode::~CdSuTnode ()

//============================================================================
// End of <cdsutimer.cpp>
//============================================================================
