#ifndef __CDSU_NHTCPDB_H
#define __CDSU_NHTCPDB_H

//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsutcpnhdb.h
// Description  : This file contains the DB for the generic Network Handler
// 		  class.
// Ref Docs     : 
// Author       : Seema Jain 
// Created On   : Feb 5th, 2004
// Last Modified: By Sumit Bijwe on Mon Feb 16 12:42:01 IST 2004
//		 + Add setNhTcpPtr and getNhTcpPtr.
//============================================================================

//============================================================================
// Include files.
//============================================================================

#include "cdsunhtcptypes.h"
#include "cdsunhtcpapiparam.h"
#include "cdsunhtcpstatspkg.h"
#include "cdsuinthash.h"

enum
{
	CDSU_NHTCP_MAX_CONN = 32768
};

// Forward Declaration.
class CdSuNhTcp;

class CdSuNhTcpDb
{
public:
	CdSuNhTcpDb (const CdSuNhTcpMiInitParam&, CdSuNhTcpInstId nhId);
	~CdSuNhTcpDb ();

	CdSuNhTcpStatisticsPkg* statsPkg;

	inline CdSuNhTcpConnId allocConnId () 
	{
		return (connIdAllocator->allocConnId ());

	}
	inline CdSuNhTcpLstnId allocLstnId ()
	{
		return (lstnIdAllocator->allocLstnId ());
	}
	inline bool freeConnId (CdSuNhTcpConnId connId)
	{
		return (connIdAllocator->freeConnId (connId));
	}

	inline bool freeLstnId (CdSuNhTcpLstnId lstnId)
	{
		return (lstnIdAllocator->freeLstnId (lstnId));

	}
	
	inline CdSuActorId getNhActorId () { return (nhActorId); }
	inline CdSuNhTcpEventHlr getSmEventHlr () { return (smEventHlr);}

	inline Uint32 getMaxNoOfConn () { return (maxNoOfConn); }
	inline Uint32 getMaxNoOfLnrs () { return (maxNoOfLstn); }
	inline Uint16 getMaxBufferLen () { return (maxBufferLen); }
	
	inline Uint32 getLnqSize() { return (lnqSize); }
	inline Uint32 getDataSelTmr () { return (dataSelTmr); }
	inline Uint32 getLstnSelTmr () { return (lstnSelTmr); }
	inline Uint32 getNoTcpHandleTmr () { return (noTcpHandleTmr); }


	inline CdSuNhTcpInstId getNhInstId() { return (nhInstId); }
	inline CdSuActorId getSmActorId () { return (smActorId); }

	inline void setNhTcpPtr (CdSuNhTcp* nhTcp) { nhTcpPtr = nhTcp;}
	inline CdSuNhTcp* getNhTcpPtr () { return (nhTcpPtr); }

private:

	class NhTcpConnIdAllocator 
	{
	public:
		NhTcpConnIdAllocator (Uint32 maxSize);
		~NhTcpConnIdAllocator ();
	
		CdSuNhTcpConnId allocConnId ();
		bool freeConnId (const CdSuNhTcpConnId connId);

	private:
		enum
		{
			CIA_NUM_ATTEMPTS = 10
		};
		pthread_mutex_t connIdAllocMutex;
		Uint32 connIdCounter;
		CdSuIntHash<CdSuNhTcpConnId>* connIdAllocTable;
		int tmparray [1000];
	};

	class NhTcpLstnIdAllocator 
	{
	public:
		NhTcpLstnIdAllocator (Uint32 maxSize);
		~NhTcpLstnIdAllocator ();
	
		CdSuNhTcpLstnId allocLstnId ();
		bool freeLstnId (const CdSuNhTcpLstnId lstnId);
		// void freeAllConnId ();

	private:
		enum
		{
			LIA_NUM_ATTEMPTS = 10
		};
		pthread_mutex_t lstnIdAllocMutex;
		Uint32 lstnIdCounter;
		CdSuIntHash<CdSuNhTcpConnId>* lstnIdAllocTable;
		int tmparray [1000];
	};

	// Network Handler Database Parameters	
	Uint32 maxNoOfConn;		
	Uint32 maxNoOfLstn;		
	Uint16 maxBufferLen;			
	Uint32 lstnSelTmr;
	Uint32 dataSelTmr;
	Uint32 noTcpHandleTmr;
	Uint32 lnqSize;	
	
	// Logical Id of the Network Handler Instance.
	CdSuNhTcpInstId nhInstId;
	NhTcpConnIdAllocator* connIdAllocator;
	NhTcpLstnIdAllocator* lstnIdAllocator;

	CdSuActorId nhActorId;			
	CdSuActorId smActorId;			
	
	CdSuNhTcpEventHlr smEventHlr;

	CdSuNhTcp* nhTcpPtr;
};

//----------------------------------------------------------------------------
// < End of file >
//----------------------------------------------------------------------------
#endif
