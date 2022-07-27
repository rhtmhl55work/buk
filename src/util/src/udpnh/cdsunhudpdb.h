#ifndef __CDSU_NHUDPDB_H
#define __CDSU_NHUDPDB_H

//=============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsuudpnhdb.h
// Description  : This file contains the DB for the generic Network Handler
// 		  class.
// Ref Docs     : 
// Author       : Vijaya Kumar 
// Created On   : Tue Mar  9 14:11:08 IST 2004
// Last Modified: 
//============================================================================

//----------------------------------------------------------------------------
// Include files.
//----------------------------------------------------------------------------

#include "cdsunhudptypes.h"
#include "cdsunhudpapiparam.h"
#include "cdsunhudpstatspkg.h"
#include "cdsuinthash.h"

// Forward Declaration.
class CdSuNhUdp;

//----------------------------------------------------------------------------
// Class	: CdSuNhUdpDb
// Description	: Network Handler DB class.
//----------------------------------------------------------------------------

class CdSuNhUdpDb
{
	
//----------------------------------------------------------------------------
// Public Inteface
//----------------------------------------------------------------------------
public:
	// Constructor
	CdSuNhUdpDb (const CdSuNhUdpMiInitParam&, CdSuNhUdpInstId);
	// Destructor
	~CdSuNhUdpDb ();

	// Allocate Udp Port ID
	inline CdSuNhUdpPortId allocUdpPortId () 
	{
		return (udpPortIdAllocator->allocUdpPortId ());
	}
	// Free Allocated Udp Port ID
	inline bool freeUdpPortId (CdSuNhUdpPortId udpPortId)
	{
		return (udpPortIdAllocator->freeUdpPortId (udpPortId));
	}
	
	// Get NH Actor ID
	inline CdSuActorId getNhActorId () { return (nhActorId); }
	// Get MI interface Callback function pointer.
	inline CdSuNhUdpEventHlr getSmEventHlr () { return (smEventHlr);}

	// Get Max No Of Udp Port Allowed
	inline Uint32 getMaxNoOfUdpPort () { return (maxNoOfUdpPort); }
	// Get Max Buffer Length.
	inline Uint16 getMaxBufferLen () { return (maxBufferLen); }
	
	// Get Data Select Timeout Value
	inline Uint32 getDataSelTmr () { return (dataSelTmr); }
	// Get Sleep Time when there is no fd to read in select
	inline Uint32 getNoUdpHandleTmr () { return (noUdpHandleTmr); }
	
	// Get Nh Instance ID
	inline CdSuNhUdpInstId getNhInstId() { return (nhInstId); }
	// Get SM Actor ID.
	inline CdSuActorId getSmActorId () { return (smActorId); }

	// Set NH Udp Pointer
	inline void setNhUdpPtr (CdSuNhUdp* nhUdp) { nhUdpPtr = nhUdp;}
	// Get Nh Udp Pointer.
	inline CdSuNhUdp* getNhUdpPtr () { return (nhUdpPtr); }

//----------------------------------------------------------------------------
// Private Functions and Variable
//----------------------------------------------------------------------------
private:
	
	//--------------------------------------------------------------------
	// Class	: Nh Udp Port Id Allocator
	// Discription	: Allocates Udp Port Id.
	//--------------------------------------------------------------------
	
	class NhUdpPortIdAllocator 
	{
	
	//--------------------------------------------------------------------
	// Public Interface.
	//--------------------------------------------------------------------
	public:
		// Constructor
		NhUdpPortIdAllocator (Uint32 maxSize);
		// Destructor
		~NhUdpPortIdAllocator ();
	
		// Allocate Udp  Port Id
		CdSuNhUdpPortId allocUdpPortId ();
		// Free Udp Port ID
		bool freeUdpPortId (const CdSuNhUdpPortId udpPortId);

	//--------------------------------------------------------------------
	// Private Interface
	//--------------------------------------------------------------------
	private:
		enum
		{
			PIA_NUM_ATTEMPTS = 10
		};
		
		
		//------------------------------------------------------------
		// Attributes
		//------------------------------------------------------------
		// Udp Port Id Mutex
		pthread_mutex_t udpPortIdAllocMutex;
		// Counter
		Uint32 udpPortIdCounter;
		// Hash Table To store allocate UDp Port ID
		CdSuIntHash<CdSuNhUdpPortId>* udpPortIdAllocTable;
	};


	//--------------------------------------------------------------------
	// Attributes
	//--------------------------------------------------------------------
	
	// Network Handler Configuration Parameters	
	Uint32 maxNoOfUdpPort;		// Max No Of Udp Port Allowed	
	Uint16 maxBufferLen;		// Max Buffer Length			
	Uint32 dataSelTmr;		// Data Select Timeout
	Uint32 noUdpHandleTmr;		// Time to sleep when there is no fd to read.
	
	// Logical Id of the Network Handler Instance.
	CdSuNhUdpInstId nhInstId;
	// Pointer to udp port id
	NhUdpPortIdAllocator* udpPortIdAllocator;

	// Nh actor ID
	CdSuActorId nhActorId;			
	// Sm Actor ID
	CdSuActorId smActorId;			
	
	// SM Interface Callback function pointer.
	CdSuNhUdpEventHlr smEventHlr;

	// Pointer To UDP NH
	CdSuNhUdp* nhUdpPtr;
}; // class CdSuNhUdpDb

//============================================================================
// < End of file cdsunhudpdb.h >
//============================================================================
#endif
