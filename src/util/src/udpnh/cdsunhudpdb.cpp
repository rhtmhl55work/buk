//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpdb.cpp
// Description	: Function declaration of Network Handler UDP DB class.
// Author       : Vijaya Kumar
// Created On   :  
//============================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <pthread.h> 
#include "cdsuutiltypes.h"
#include "cdsunhudptypes.h"
#include "cdsunhudpdb.h"
#include "cdsunhudpapiparam.h"
#include "cdsutrace.h"
//---------------------------------------------------------------------------
// Externs of Default Event Handler
//---------------------------------------------------------------------------
extern void cdSuNhUdpDefSmEventHlr (void* arg);

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpDb::CdSuNhUdpDb ()
// Description  : Constructor of Network Handler DB class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpDb::CdSuNhUdpDb (const CdSuNhUdpMiInitParam& nhInitParam,
	       			CdSuNhUdpInstId nhInstId)
{
	// Set Configure parameters.
	maxNoOfUdpPort = nhInitParam.maxNoOfUdpPort;
	maxBufferLen = nhInitParam.maxBufferLen;
	
	dataSelTmr = nhInitParam.dataSelectTimer;	
	noUdpHandleTmr = nhInitParam.noHandleTimer;
	
	this->nhInstId = nhInstId;
	
	nhActorId = nhInitParam.nhActorId;	
	smActorId = nhInitParam.smActorId;
	
	// If callback function is invalid then set default callback funcion.
	if ( nhInitParam.smEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
	{
		smEventHlr = cdSuNhUdpDefSmEventHlr;
	} else
	{
		smEventHlr = nhInitParam.smEventHlr;
	}
	// Port ID Allocator
	udpPortIdAllocator = new NhUdpPortIdAllocator (maxNoOfUdpPort);

} // CdSuNhUdpDb::CdSuNhUdpDb ()

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpDb::~CdSuNhUdpDb ()
// Description  : Destructor of Network Handler DB class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpDb::~CdSuNhUdpDb ()
{
	delete udpPortIdAllocator;

} // CdSuNhUdpDb::~CdSuNhUdpDb ()

//---------------------------------------------------------------------------
// Name		: NhUdpPortIdAllocator::NhUdpPortIdAllocator ()
// Description  : Constructor
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpDb::NhUdpPortIdAllocator::NhUdpPortIdAllocator (Uint32 maxNoOfUdpPort)
{
	udpPortIdCounter = 0;
	udpPortIdAllocTable = new CdSuIntHash<CdSuNhUdpPortId> (CdSuIntHash<CdSuNhUdpPortId>::TZ_2039);	
	// Init mutex.
	if (pthread_mutex_init (&udpPortIdAllocMutex, NULL) == -1)
	{
		CDSU_TRACE (1, "Init Mutex Error\n");
	}


} // NhUdpPortIdAllocator::NhUdpPortIdAllocator ()

//---------------------------------------------------------------------------
// Name		: NhUdpPortIdAllocator::~NhUdpPortIdAllocator ()
// Description  : Destructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpDb::NhUdpPortIdAllocator::~NhUdpPortIdAllocator ()
{
	delete udpPortIdAllocTable;

} // NhUdpPortIdAllocator::NhUdpUdpPortIdAllocator ()


//---------------------------------------------------------------------------
// Name		: NhUdpPortIdAllocator::allocUdpPortId ()
// Description  : Allocate Udp Port Id.
// Inputs       : 
// Return Value : Returns UdpPort Id.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpPortId CdSuNhUdpDb::NhUdpPortIdAllocator::allocUdpPortId ()
{
	pthread_mutex_lock (&udpPortIdAllocMutex);

	CdSuNhUdpPortId udpPortId = CDSU_NHUDP_INV_UDPPORTID;

	for (int i = 0; i < PIA_NUM_ATTEMPTS; i++)
	{
		++udpPortIdCounter;
		// if udp port id going to be allocated is invalid one or always
		// opened type then dont allocate.
		if ((udpPortIdCounter == CDSU_NHUDP_INV_UDPPORTID) ||
			(udpPortIdCounter == CDSU_NHUDP_ALWOP_UDPPORTID))
		{
			++udpPortIdCounter;
		}
		if (udpPortIdAllocTable->lookup (udpPortIdCounter, udpPortId) == false)
		{
			udpPortId = udpPortIdCounter;
			udpPortIdAllocTable->insert (udpPortIdCounter, udpPortId);
			pthread_mutex_unlock (&udpPortIdAllocMutex);
			return (udpPortIdCounter);
		}
	}

	pthread_mutex_unlock (&udpPortIdAllocMutex);

	return (CDSU_NHUDP_INV_UDPPORTID);

} // NhUdpPortIdAllocator::allocUdpPortId ()

//---------------------------------------------------------------------------
// Name		: NhUdpConnIdAllocator::freeConnId ()
// Description  : Free ConnectionId.
// Inputs       : 
// Return Value : Returns bool
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhUdpDb::NhUdpPortIdAllocator::
				freeUdpPortId (CdSuNhUdpPortId udpPortId)
{	
	pthread_mutex_lock (&udpPortIdAllocMutex);

	CdSuNhUdpPortId upId = CDSU_NHUDP_INV_UDPPORTID;
	if (udpPortIdAllocTable->lookup (udpPortId, upId) != false)
	{
		if (udpPortIdAllocTable->remove (udpPortId) != false)
		{
			pthread_mutex_unlock (&udpPortIdAllocMutex);
			return (true);
		}
	}
	pthread_mutex_unlock (&udpPortIdAllocMutex);
	return (false);

} // NhUdpPortIdAllocator::freeUdpPortId ()

//============================================================================
// <End of cdsunhudpdb.cpp>
//============================================================================




