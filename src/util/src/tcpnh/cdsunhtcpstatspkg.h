#ifndef __CDSU_NHSTSDB_H
#define __CDSU_NHSTSDB_H

//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpstatspkg.h
// Description  : This file contains the Statistics Package of the Network
// 		  Handler.
// Ref Docs     : 
// Author       : Seema Jain 
// Created On   : Feb 13th, 2003
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

class CdSuNhTcpStatisticsPkg
{
public:
	CdSuNhTcpStatisticsPkg ()
	{
		resetAllCtrs ();
	}
	~CdSuNhTcpStatisticsPkg ()
	{
	}

	void resetAllCtrs ()
	{
		for (int i = 0; i < CDSU_NHTCP_MAX_CTR; i++)
		{
			counters [i] = 0;
		}
	} 
	void resetCtr (CdSuNhTcpCtrType ctrType)
	{
		counters [ctrType] = 0;

	}

	void incrementCtr (CdSuNhTcpCtrType ctrType)
	{
		++counters [ctrType];

	}

	void decrementCtr (CdSuNhTcpCtrType ctrType)
	{
		--counters [ctrType];
	}

	void setCtr (CdSuNhTcpCtrType ctrType, Uint32 value)
	{
		counters [ctrType] = value;
	}

	Ulong* getCounters ()
	{
		return counters;
	}

private:	

	Ulong counters [CDSU_NHTCP_MAX_CTR];

}; // class CdSuNhTcpStatisticsPkg

//----------------------------------------------------------------------------
// < End of file >
//----------------------------------------------------------------------------

#endif


