//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpinstmgr.cpp
// Description	: Function declaration of Network Handler TCP DB class.
// Author       : Seema Jain
// Created On   : Wed Feb 11 18:06:29 IST 2004 
// Last Modified: By Sumit Bijwe on Wed Feb 25 10:21:38 IST 2004
//               + Added remove tcp NH instance method.
//============================================================================

//============================================================================
// Include Files
//============================================================================

#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsunhtcpdb.h"
#include "cdsuthread.h"
#include "cdsunhtcp.h"
#include "cdsunhtcpinstmgr.h"

//-----------------------------------------------------------------------------
// static variables 
//-----------------------------------------------------------------------------

CdSuNhTcpInstMgr* CdSuNhTcpInstMgr::nhTcpInstMgr = (CdSuNhTcpInstMgr*) NULL;

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpInstMgr::CdSuNhTcpInstMgr ()
// Description  : The constructor in this class is a protected funcitons.
// 		  This is done to achieve the singleton functionality.
// Inputs       : InitParam.
// Return Value : CdSuNhTcpInstId.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpInstMgr::CdSuNhTcpInstMgr ()
{
	// Init mutex.
	if (pthread_mutex_init (&dbLock, NULL) == -1)
	{
		CDSU_TRACE (1, "Init Mutex Error\n");
	}
	
	maxNhInst = CDSU_NHTCP_MAX_INST;

	// No Network Handler Instances right now.
	noOfNhInst = 0;

	// No need to put mutex lock here as this is in the constructor. No
	// other thread can update this now.
	for (Uint32 i = 0 ; i < CDSU_NHTCP_MAX_INST; i++)
	{
		nhTcpDbTable [i] = NULL;	
	}


} // CdSuNhTcpInstMgr::CdSuNhTcpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpInstMgr::~CdSuNhTcpInstMgr ()
// Description  : The destructor in this class. 
// Inputs       :
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpInstMgr::~CdSuNhTcpInstMgr ()
{
	nhTcpInstMgr = (CdSuNhTcpInstMgr*) NULL;

} // CdSuNhTcpInstMgr::~CdSuNhTcpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpInstMgr::createNhTcpInstMgr ()
// Description  : This function creates the Network Handler TCP Instance
// 		  Manager when called for the first time. When called more
// 		  than once it simply returns the pointer to the Network
// 		  Handler instance manager.
// Inputs       : InitParam.
// Return Value : CdSuNhTcpInstId.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpInstMgr* CdSuNhTcpInstMgr::createNhTcpInstMgr ()
{
	if (nhTcpInstMgr == NULL)
	{
		nhTcpInstMgr = new CdSuNhTcpInstMgr ();
		return (nhTcpInstMgr);
	} else 
	{
		return (nhTcpInstMgr);
	}

} // CdSuNhTcpInstMgr::createNhTcpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpInstMgr::addNhTcpInst ()
// Description  : Adds a Network Handler TCP Instance.
// Inputs       : InitParam.
// Return Value : CdSuNhTcpInstId.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpInstId CdSuNhTcpInstMgr::addNhTcpInst (const CdSuNhTcpMiInitParam& iparam)
{
	CdSuNhTcpMiInitParam initParam = iparam;

	pthread_mutex_lock (&dbLock);

	for (Uint32 i = 0; i < maxNhInst; i++)
	{
		if (nhTcpDbTable [i] == NULL)
		{
			// Create the NhDb.
			// Create the Network Handler Active Instance.
			noOfNhInst++;
			nhTcpDbTable [i] = new CdSuNhTcpDb (initParam, i);
			CdSuNhTcp* nhTcp = new CdSuNhTcp (nhTcpDbTable [i]);	
			// Set the nhtcp pointer in db.
			(nhTcpDbTable[i])->setNhTcpPtr (nhTcp);

			nhTcp->run ();

			pthread_mutex_unlock (&dbLock);
			return (i);
		}
	}

	pthread_mutex_unlock (&dbLock);
	return (CDSU_NHTCP_INV_NHINSTID);

} // CdSuNhTcpInstMgr::addNhTcpInst ()

//---------------------------------------------------------------------------
// Name		: CdSuNhTcpInstMgr::removeNhTcpInst ()
// Description  : Removes a Network Handler TCP Instance.
// Inputs       : 
// Return Value : boolean.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcpInstMgr::removeNhTcpInst (const CdSuNhTcpInstId instId)
{
	pthread_mutex_lock (&dbLock);

	if (nhTcpDbTable [instId] != NULL)
	{
		// Get and delete the nhtcp pointer. 
		CdSuNhTcp* nhTcp = (nhTcpDbTable[instId])->getNhTcpPtr();
		delete nhTcp;
		nhTcp = NULL;

		// Delete the NhDb.
		delete (nhTcpDbTable [instId]);
		nhTcpDbTable [instId] = NULL;

		noOfNhInst--;
		pthread_mutex_unlock (&dbLock);
		return (true);
	}

	pthread_mutex_unlock (&dbLock);
	return (false);

} // CdSuNhTcpInstMgr::removeNhTcpInst ()


//==============================================================================
// <End of cdsunhtcpinstmgr.cpp>
//==============================================================================

