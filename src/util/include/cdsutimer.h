#ifndef _CDSU_TIMER_H_
#define _CDSU_TIMER_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group

// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : timer.h
// Description  : Timer functions.
// Ref Docs     : 
// Author       : 
// Contributor  : 
// Created On   : April 3rd, 2002
// Last Modified By : Indu Mishra
// Last Modified: For checkin, CdsuTimer Msg to be changed.
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

//----------------------------------------------------------------------------
// 		Instructions for using the timer.
//		---------------------------------
//
// The user of the timer library should instantiate an object of the 
// TimerHandler. 
//
// The user of the timer should provide the "userTimeoutHandler ()" routine. 
// Whenever a timeout happens, this routine will be called by the 
// TimerHandler.
//
// The user of the timer should also provide a "tick" mechanism, by 
// which the timerHandler is given a trigger every 10 milliseconds.
// This routine currently is TimerHandler::timerTickHandler ()
//
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Include Files.
//-----------------------------------------------------------------------------

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsuinthash.h"

//----------------------------------------------------------------------------
// Timer typedefs.
//----------------------------------------------------------------------------

typedef Uint32 CdSuTimeCountType;
typedef Uint32 CdSuTimeoutDestId;
typedef Uint32 CdSuTimerId;


//----------------------------------------------------------------------------
// Forward declaration.
//----------------------------------------------------------------------------

class CdSuKeyAllocator;

//----------------------------------------------------------------------------
// Enumerations and typedefs. 
//----------------------------------------------------------------------------

enum 
{
	// CDSU_TIMEOUT = 0,				// Timer TimeOut opcode.
	CDSU_TMR_MAX_USERINFO_NUM = 8,	    		// Maximum number of 
	CDSU_TMR_INV_TIMER_ID = 0xffffffff, 		// Invalid Timer Id 
	CDSU_TIMEWHEEL_HASH_TABLE_SIZE = 6151
};

//----------------------------------------------------------------------------
// Time Units: The time value is specified in this resolution while starting
// 	       timer.
//----------------------------------------------------------------------------

enum CdSuTimeUnits
{
        CDSU_TMR_TU_TENMS = 1,      // 10ms 
        CDSU_TMR_TU_HUNDREDMS,      // 100ms
        CDSU_TMR_TU_SECOND,         // 1s
        CDSU_TMR_TU_MINUTE,         // 60s
        CDSU_TMR_TU_HOUR,           // 60 minutes
	CDSU_TMR_TU_INVALID

}; // CdSuTimeUnits

//----------------------------------------------------------------------------
// Timer Type
// 	* Trigger once.
//	* Cyclic timer. 
//----------------------------------------------------------------------------

enum CdSuTimerType
{
	CDSU_TMR_TT_TRIGGER_ONCE = 1,
	CDSU_TMR_TT_CYCLIC,
	CDSU_TMR_TT_INVALID

}; // CdSuTimerType

//----------------------------------------------------------------------------
// Timeout Report Mechanism:
//	After timeout, the timer utility will 
//	* either invoke the callback function provided during 
//	  start of the timer.
//	* It will form a message and invoke the "cdSuUserTimeoutHlr()"
//	  function.
//----------------------------------------------------------------------------

enum CdSuTimeoutReportType
{
	CDSU_TMR_TRT_CALLBACK = 1,	// Invoke call back function on timeout.
	CDSU_TMR_TRT_MESSAGE,		// Mail a message on timeout.
	CDSU_TMR_TRT_INVALID
};

//----------------------------------------------------------------------------
// Function return values. 
// Type of error returned by startTimer() and stopTimer()of CdSuTimerHandler
//----------------------------------------------------------------------------

enum CdSuTmrRetVal
{
	CDSU_TMR_RV_SUCCESS = 0,	// Success.
	CDSU_TMR_RV_INV_TIME_UNIT,	// Invalid Time Unit.
	CDSU_TMR_RV_INV_TIME_COUNT,	// Invalid Time Count
	CDSU_TMR_RV_INV_MODID,		// ??
	CDSU_TMR_RV_INV_TMRID, 		// Invalid Timer Id.
	CDSU_TMR_RV_INV_TMRWHEELDEL,	// Invalid Timer Wheel Delete.
	CDSU_TMR_RV_INV_TMRTYPE,	// Invalid Timer type
	CDSU_TMR_RV_INV_TMRREPTYPE,	// Invalid TimeOut Rep. Type.
	CDSU_TMR_RV_INV_TMRHASHDEL,	// Invalid Timer Hash Delete.
	CDSU_TMR_RV_INV_MUTEXLOCK,	// Invalid Mutex Lock.
	CDSU_TMR_RV_INV_MUTEXUNLOCK,	// Invalid Mutex Unlock.
	CDSU_TMR_RV_INV_CALLBACK,	// Invalid Call Back Function.
	CDSU_TMR_RV_INV_TMRHASHINSERT
	
};

//----------------------------------------------------------------------------
// Forward declarations.
//----------------------------------------------------------------------------

class CdSuTimerMsg;
class CdSuTnode;
class CdSuTimeque;
class CdSuTimerHandler;
class CdSuTimeout;
class CdSuTimingWheel;
struct CdSuTimerInfo;

//----------------------------------------------------------------------------
// Extern Declarations.
//----------------------------------------------------------------------------

extern void cdSuUserTimeoutHlr (CdSuTimeout);
extern void CallBackFunction (CdSuTimeout&);
//----------------------------------------------------------------------------
// 			Structures and classes.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class	: CdSuTimerInfo:
// Description	: This structure is used to start a timer.
//----------------------------------------------------------------------------

struct CdSuTimerInfo
{
	CdSuTimeUnits timeUnits;	// Time Unit resolution(10ms, 100ms etc) 
	Uint16 timeCounts;		// No. 
	CdModuleId destModuleId;
	Uint16 opcode;
	Uint32 userInfo [CDSU_TMR_MAX_USERINFO_NUM];

	CdSuTimerType timerType;
	CdSuTimeoutReportType timeoutReportType;
	void (*timerCallBackFn) (CdSuTimeout&);

}; // CdSuTimerInfo

//----------------------------------------------------------------------------
// Class	: CdSuTimeout
// Description	: Structure to be sent into the user callback function.
//----------------------------------------------------------------------------

struct CdSuTimeout
{
	CdSuTimerId timerId;
	CdModuleId destModuleId;
	Uint16 opcode;
	Uint32 userInfo [CDSU_TMR_MAX_USERINFO_NUM];

}; // CdSuTimeout

//----------------------------------------------------------------------------
// Class	: CdSuTimerMsg
// Description	: Structure to be mailed to the thread.
// Note		: When the timeout happens, the  CdSuUserTimeoutHlr() is called,
//		  and the CdSuTimerMsg structure is passed as a parameter.
//----------------------------------------------------------------------------

struct CdSuTimerMsg : public CdMessage
{
	void initMsg (const CdModuleId dst, const CdModuleId src, 
		const CdSuTimerId timerId, const Uint32* timerInfo);
	CdSuTimerId timerId;
	Uint32 userInfo [CDSU_TMR_MAX_USERINFO_NUM];

};// CdSuTimerMsg


//--------------------------------------------------------------------------
// Class        : CdSuTimerHandler
// Declaration  : Class to start and stop timer
//----------------------------------------------------------------------------

class CdSuTimerHandler
{
public:
	CdSuTimerHandler ();
	~CdSuTimerHandler ();
	
	CdSuTmrRetVal startTimer (CdSuTimerId& timerId, 
				  const CdSuTimerInfo& userTimerInfo);
	CdSuTmrRetVal stopTimer (const CdSuTimerId);
	void timerTickHandler (); 
	
	friend class CdSuTimingWheel;

protected:
	CdSuTimingWheel* timeWheel;

private:
	CdSuIntHash<CdSuTnode*>* timerHash;
	CdSuKeyAllocator* keyAlloc;
	pthread_mutex_t  timerMutex;
	
}; // CdSuTimerHandler

//----------------------------------------------------------------------------
// 		Internals of the timer implementation.
//
// The user of the timer should not be concerned about the routines given 
// below.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Structure    : CdSuTnode
// Description  : Struct CdSuTnode.Timer Que consist of nodes of this type.
//----------------------------------------------------------------------------

struct CdSuTnode
{
	
        CdSuTnode* previous ;	// Pointer to previous request.  NULL if none.
        CdSuTnode* next;    	// Pointer to next request. NULL if none.

	Uint32 tickCount;       // Stores the quotient of the numTicks / size
				// of timing Wheel.
	Uint32 offset;		// Used to store the hash position in
				// timingwheel hash.

	CdSuTimeUnits initTimeUnits;// Time Unit resolution(10ms, 100ms etc) 
	Uint16 initTimeCounts;      // Timer time for which timer is started.

	// Info required on timeout.
	CdModuleId timeoutDestId; 	
	Uint16 opcode;			
        CdSuTimerId timerId;  
        Uint32 userInfo [CDSU_TMR_MAX_USERINFO_NUM]; // Some user specific info .

	CdSuTimerType timerType;
	CdSuTimeoutReportType timeoutReportType;
	void (*timerCallBackFn) ( CdSuTimeout&);
	
	// Methods.
	CdSuTnode (const CdSuTimerInfo& userTimerInfo, 
		CdSuTimerId timerId, 
		Uint32 tempCount);
	~CdSuTnode ();

}; // CdSuTnode
#endif



//============================================================================
// <End of <cdsutimer.h>
//============================================================================

