#ifndef __CDSU_SANITY_H__
#define __CDSU_SANITY_H__

//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsusanity.h
// Description  : This header file includes the declaration of the class
// 		  implementing the sanity object
// Ref Docs     : ITS-MDD-PM_PM-001
// Author       : Dipanjan Dutta
// Contributor	: 
// Created On   : 14 Jun, 2002
// Last Modified: Thu Aug  1 12:46:17 IST 2002
// Added to Utility Kit from SS7AMF on Wed Apr 19 18:19:26 IST 2006
//============================================================================

#ifdef SM_MEMTEST
#include <stdlib.h>
#endif


#include <stdio.h>
#include "cdsutrace.h"
#include "cdsusanitymsgs.h"
#include "cdsumdapi.h"
#include "cdsutimer.h"

//----------------------------------------------------------------------------
// Global data type
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Cnfg params for a sanity object
//----------------------------------------------------------------------------

struct SanityParams
{
	// This tag will be inserted in the body of the sanity failure
	// notification msg for any management purpose. The meaning attached
	// to this tag will be determined by the recepient of the
	// notification.
	Uint8	sanityTag;

	// Id of Self, Peer with which sanity exchanged and Entity to which
	// sanity failure is notified.
	CdModuleId src;
	CdModuleId dst;
	CdModuleId fdst;

	// Id of the link over which sanity msgs should be exchanged
	Uint8 link;

	// Max No. of consecutive sanity interval elapses before we notify
	// SanityFailure
	Uint8 sanityMissThreshold;

	// The opcodes used for the tx and rx sanity msgs and of the sanity
	// failure msg.
	Uint32	txSanityMsgOpcode;
	Uint32	rxSanityMsgOpcode;
	Uint32	sanityFailureMsgOpcode;

	// The opcodes to be used in the timeout notification msgs. 
	Uint32  timeoutOpcode;
	Uint32	txTimerMsgOpcode; // txTimeoutTag
	Uint32	rxTimerMsgOpcode; // rxTimeoutTag

	// The timeout values.
	Uint8	txTimeout;
	Uint8	rxTimeout;
};

//----------------------------------------------------------------------------
// Class	: SanityModule
// Description	: This is the sanity object responsible for maintaining
// 		  sanity with a configured peer. 
//		  All sanity objects in the system are created/deleted by the
//		  SanityManager. 
//		  Each of the sanity objects can be used by an application
//		  entity which wants to maintain sanity with another
//		  application entity.
// 		  The user of the sanity object (the application entity) must
// 		  obtain the object pointer from the SanityManager.
//----------------------------------------------------------------------------

class SanityModule
{
public:
	SanityModule (const SanityParams&, CdSuTimerHandler*);
	~SanityModule ();

	// SanityModule Configuration interface
	void reset (void);
	void setSanityParams (const SanityParams&);
	void getSanityParams (SanityParams&);

	// SanityModule Control interface
	void startTx (void);
	void stopTx (void);
	void startRx (void);
	void stopRx (void);
	void handleMessage (CdMessage*);

	// SanityModule Query interface
	Uint8 getMissedSanityCount (void);
protected:
	// Empty Section
	
private:
	//------------------------------------------------------------------
	// Methods:
	//------------------------------------------------------------------
	void sanityHlr (void);
	void timeoutHlr (CdMessage* msg);
	void txTimeoutHlr (void);
	void rxTimeoutHlr (void);
	void print ();
	
	// Sanity Configuration info
	SanityParams cnfgInfo;

	// Convenience references for src, dst, failure ServiceHandler Ids.
	Uint32 srcShId;
	Uint32 dstShId;
	Uint32 fdstShId;

	// Flags to maintain state
	bool txSanityFlag;
	bool rxSanityFlag;
	bool resetSanityFlag;
	bool estdSanityFlag;

	// Count of the no. of elapsed intervals since the last rxd sanity msg.
	Uint8 missedSanityCount;

	// Timers for interrupting us when sanity interval elapses
	CdSuTimerHandler* timerHandler;
	CdSuTimerId txSanityTimer;
	CdSuTimerInfo txSanityTI;
	CdSuTimerId rxSanityTimer;
	CdSuTimerInfo rxSanityTI;

	// ???????
	SanityMsg* txSanityMsg;

#ifdef SM_MEMTEST
	int count;	// To print the memory usage every one minute
#endif
};

#endif
//============================================================================
// <end of sanity.h>
//============================================================================
