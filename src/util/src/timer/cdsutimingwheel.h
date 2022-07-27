#ifndef __CDSUTMR_H_
#define __CDSUTMR_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsutimingwheel.h.
// Description	: This file contains definitions for hash function. 
// Ref Docs     : 
// Author	: 
// Created On   : 29/01/2003
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
 
//============================================================================

//---------------------------------------------------------------------------
// Include Files.
//---------------------------------------------------------------------------

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "cdsuinthash.h"
#include "cdsutrace.h"
#include "cdsukeyallocator.h"
#include "cdsutimer.h"


//---------------------------------------------------------------------------
// Forward Declaration.
//---------------------------------------------------------------------------

class CdSuTimerHandler;

//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel.
// Description	: Class definitions for implementing hash function.
// Notes	:
//---------------------------------------------------------------------------

class CdSuTimingWheel
{
public:
	//-------------------------------------------------------------------
	// Public Methods.
	//-------------------------------------------------------------------
	// Constructor and destructor.
	CdSuTimingWheel (CdSuTimerHandler* th, Uint32 hashTableSize);
	~CdSuTimingWheel ();
	void clear ();

  	// Add a request to the que sorted according to the time desired.
        CdSuTnode* add (const CdSuTimerInfo& userInfo, CdSuTimerId& timerId);

	// Delete the node whose pointer is in tnode.
	bool del (CdSuTnode* tnode);

        // For the node next to headNode extract the fields and send
	// msg to the corresponding oid.
        void timeoutHandler (CdSuTnode* node);

        // Decrement timerTicks of node next to headNode and send a 
	// msg if equal to 0.
        void tickDecCount (void);

	//added so that timer can access currentPosition of timingWheel for
	//use in del function.
	friend class CdSuTimerHandler;

private:
	//-------------------------------------------------------------------
	// Private Attributes
	//-------------------------------------------------------------------

	// Size of the hashTable.
	Uint32 hashTableSize;

	// HashTable to store the Timer nodes.

	CdSuTnode* hashTable[CDSU_TIMEWHEEL_HASH_TABLE_SIZE];
	// TimerHandler
	CdSuTimerHandler* timerHandler;

	// Initial start position is the start position of array only.
	Uint32 currentPosition;
	
	//-------------------------------------------------------------------
	// Private Methods.
	//-------------------------------------------------------------------
	Uint32 hashFunction1 (Uint32 keyVal);

	// Delete timer node
	void deleteTnode (CdSuTnode* p );  	

        // Calculate the  number of ticks in units of 10ms.
        Uint32 calcNumTicks (Uint32 timeUnits, Uint32 timeCounts );

}; // class CdSuTimingWheel

//=============================================================================
// End of <CdSuTimingWheel.h>
//=============================================================================
#endif
