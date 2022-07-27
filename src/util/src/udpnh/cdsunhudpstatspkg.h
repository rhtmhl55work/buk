#ifndef __CDSU_NHSTSDB_H
#define __CDSU_NHSTSDB_H

//=============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpstatspkg.h
// Description  : This file contains the Statistics Package of the Network
// 		  Handler.
// Ref Docs     : 
// Author       : Vijaya Kumar 
// Created On   : Tue Mar  9 14:34:37 IST 2004
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

#include "cdsunhudptypes.h"

//----------------------------------------------------------------------------
// Class	: CdSuNhUdpStatisticsPkg
// Description	: Statistics Pakcage.
//----------------------------------------------------------------------------

class CdSuNhUdpStatisticsPkg
{
	
//----------------------------------------------------------------------------
// Public Interface.
//----------------------------------------------------------------------------
public:
	// Constructor
	CdSuNhUdpStatisticsPkg ()
	{
		resetAllCtrs ();
	}
	// Destructor
	~CdSuNhUdpStatisticsPkg ()
	{
	}

	// Resetting  all counters to 0
	void resetAllCtrs ()
	{
		for (int i = 0; i < CDSU_NHUDP_MAX_CTR; i++)
		{
			counters [i] = 0;
		} // for
	} // void resetAllCtrd 
	
	// Reseting perticular counter to 0
	void resetCtr (CdSuNhUdpCtrType ctrType)
	{
		counters [ctrType] = 0;
	}
	
	// increment Counter.
	void incrementCtr (CdSuNhUdpCtrType ctrType)
	{
		++counters [ctrType];

	}
	// decrement counter.
	void decrementCtr (CdSuNhUdpCtrType ctrType)
	{
		--counters [ctrType];
	}

	// Statistics Counters
	Ulong counters [CDSU_NHUDP_MAX_CTR];

private:	


}; // class CdSuNhUdpStatisticsPkg

//============================================================================
// < End of file >
//============================================================================
#endif


