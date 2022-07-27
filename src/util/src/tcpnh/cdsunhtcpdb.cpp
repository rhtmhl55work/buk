//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpdb.cpp
// Description	: Function declaration of Network Handler TCP DB class.
// Author       : Seema Jain
// Created On   :  
// Last Modified: By Sumit Bijwe Sat Feb 14 12:06:29 IST 2004
//
// Modified by 	: Vijaya Kumar
// Mofified On 	: Wed Aug  4 20:03:48 IST 2004
//============================================================================

//============================================================================
// Include Files
//============================================================================
#include <pthread.h> 

#include "cdsuutiltypes.h"
#include "cdsunhtcptypes.h"
#include "cdsunhtcpdb.h"
#include "cdsunhtcpapiparam.h"

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpDb::CdSuNhTcpDb ()
// Description  : Constructor of Network Handler DB class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpDb::CdSuNhTcpDb (const CdSuNhTcpMiInitParam& nhInitParam, CdSuNhTcpInstId nhId)
{
	maxNoOfConn = nhInitParam.maxNoOfConnections;
	maxNoOfLstn = nhInitParam.maxNoOfLstns;
	maxBufferLen = nhInitParam.maxBufferLen;
	nhInstId = nhId;

	lstnSelTmr = nhInitParam.listenSelectTimer;
	dataSelTmr = nhInitParam.dataSelectTimer;	
	noTcpHandleTmr = nhInitParam.noHandleTimer;
	lnqSize = nhInitParam.listenQSize;

	nhActorId = nhInitParam.nhActorId;
	smActorId = nhInitParam.smActorId;
	smEventHlr = nhInitParam.smEventHlr;
	
	lstnIdAllocator = new NhTcpLstnIdAllocator (maxNoOfLstn);
	connIdAllocator = new NhTcpConnIdAllocator (maxNoOfConn);

	statsPkg = new CdSuNhTcpStatisticsPkg ();

} // CdSuNhTcpDb::CdSuNhTcpDb ()

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpDb::~CdSuNhTcpDb ()
// Description  : Destructor of Network Handler DB class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpDb::~CdSuNhTcpDb ()
{
	delete (lstnIdAllocator);
	delete (connIdAllocator);
	delete (statsPkg);

} // CdSuNhTcpDb::~CdSuNhTcpDb ()

//---------------------------------------------------------------------------
// Name		: NhTcpConnIdAllocator::NhTcpConnIdAllocator ()
// Description  : Constructor
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpDb::NhTcpConnIdAllocator::NhTcpConnIdAllocator (Uint32 maxNoOfConns)
{
	connIdCounter = 0;
	connIdAllocTable = new CdSuIntHash<CdSuNhTcpConnId> (CdSuIntHash<CdSuNhTcpConnId>::TZ_2039);	
	pthread_mutex_init (&connIdAllocMutex, NULL);

} // NhTcpConnIdAllocator::NhTcpConnIdAllocator ()

//---------------------------------------------------------------------------
// Name		: NhTcpConnIdAllocator::~NhTcpConnIdAllocator ()
// Description  : Destructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpDb::NhTcpConnIdAllocator::~NhTcpConnIdAllocator ()
{
	delete connIdAllocTable;

} // NhTcpConnIdAllocator::NhTcpConnIdAllocator ()


//---------------------------------------------------------------------------
// Name		: NhTcpConnIdAllocator::allocConnId ()
// Description  : Allocate ConnectionId.
// Inputs       : 
// Return Value : Returns Connection Id.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpConnId CdSuNhTcpDb::NhTcpConnIdAllocator::allocConnId ()
{
	pthread_mutex_lock (&connIdAllocMutex);

	CdSuNhTcpConnId tcpConnId = CDSU_NHTCP_INV_CONNID;

	for (int i = 0; i < CIA_NUM_ATTEMPTS; i++)
	{
		++connIdCounter;
		if (connIdAllocTable->lookup (connIdCounter, tcpConnId) == false)
		{
			tcpConnId = connIdCounter;
			connIdAllocTable->insert (connIdCounter, tcpConnId);
			pthread_mutex_unlock (&connIdAllocMutex);
			return (connIdCounter);
		}
	}

	pthread_mutex_unlock (&connIdAllocMutex);

	return (CDSU_NHTCP_INV_CONNID);

} // NhTcpConnIdAllocator::allocConnId ()

//---------------------------------------------------------------------------
// Name		: NhTcpConnIdAllocator::freeConnId ()
// Description  : Free ConnectionId.
// Inputs       : 
// Return Value : Returns Connection Id.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcpDb::NhTcpConnIdAllocator::
				freeConnId (CdSuNhTcpConnId connId)
{	
	CdSuNhTcpConnId tcpConnId = CDSU_NHTCP_INV_CONNID;

	if (connIdAllocTable->lookup (connId, tcpConnId) != false)
	{
		if (connIdAllocTable->remove (connId) != false)
		{
			pthread_mutex_unlock (&connIdAllocMutex);
			return (true);
		}
	}
	return (false);

} // NhTcpConnIdAllocator::freeConnId ()

//---------------------------------------------------------------------------
// Name		: NhTcpLstnIdAllocator::NhTcpLstnIdAllocator ()
// Description  : Constructor
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpDb::NhTcpLstnIdAllocator::NhTcpLstnIdAllocator (Uint32 maxNoOfLstns)
{
	lstnIdCounter = 0;
	lstnIdAllocTable = new CdSuIntHash<CdSuNhTcpConnId> (CdSuIntHash<CdSuNhTcpConnId>::TZ_2039);	
	pthread_mutex_init (&lstnIdAllocMutex, NULL);

} // NhTcpLstnIdAllocator::NhTcpLstnIdAllocator ()

//---------------------------------------------------------------------------
// Name		: NhTcpLstnIdAllocator::~NhTcpLstnIdAllocator ()
// Description  : Destructor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpDb::NhTcpLstnIdAllocator::~NhTcpLstnIdAllocator ()
{
	delete lstnIdAllocTable;

} // NhTcpLstnIdAllocator::NhTcpLstnIdAllocator ()

//---------------------------------------------------------------------------
// Name		: NhTcpLstnIdAllocator::allocLstnId ()
// Description  : Allocate ConnectionId.
// Inputs       : 
// Return Value : Returns Connection Id.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpLstnId CdSuNhTcpDb::NhTcpLstnIdAllocator::allocLstnId ()
{
	pthread_mutex_lock (&lstnIdAllocMutex);

	CdSuNhTcpLstnId tcpLstnId = CDSU_NHTCP_INV_LSTNID;
	for (int i = 0; i < LIA_NUM_ATTEMPTS; i++)
	{
		++lstnIdCounter;
		if (lstnIdAllocTable->lookup (lstnIdCounter, tcpLstnId) != true)
		{
			tcpLstnId = lstnIdCounter;
			if (lstnIdAllocTable->insert (lstnIdCounter, tcpLstnId) != false)
			{
				pthread_mutex_unlock (&lstnIdAllocMutex);
				return (tcpLstnId);
			} 
		}
	}

	pthread_mutex_unlock (&lstnIdAllocMutex);

	return (CDSU_NHTCP_INV_LSTNID);

} // NhTcpLstnIdAllocator::allocLstnId ()

//---------------------------------------------------------------------------
// Name		: NhTcpLstnIdAllocator::freeLstnId ()
// Description  : Free ConnectionId.
// Inputs       : 
// Return Value : Returns Connection Id.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcpDb::NhTcpLstnIdAllocator:: freeLstnId (
	CdSuNhTcpLstnId lstnId)
{
	CdSuNhTcpConnId tcpLstId = CDSU_NHTCP_INV_LSTNID;  

	if (lstnIdAllocTable->lookup (lstnId, tcpLstId) != false)
	{
		if (lstnIdAllocTable->remove (lstnId) != false)
		{
			pthread_mutex_unlock (&lstnIdAllocMutex);
			return (true);
		}
	}
	return (false);

} // NhTcpLstnIdAllocator::freeLstnId ()

//----------------------------------------------------------------------------
// <End of cdsunhtcpdb.cpp>
//----------------------------------------------------------------------------




