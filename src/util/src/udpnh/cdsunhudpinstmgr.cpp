//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpinstmgr.cpp
// Description	: Implements UDP Instance Manager.
// Author       : Vijaya Kumar
// Created On   : Tue Mar  9 14:44:26 IST 2004
// Last Modified: 
//============================================================================

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsunhudpdb.h"
#include "cdsuthread.h"
#include "cdsunhudp.h"
#include "cdsunhudpinstmgr.h"

//-----------------------------------------------------------------------------
// static variables Initilization
//-----------------------------------------------------------------------------

CdSuNhUdpInstMgr* CdSuNhUdpInstMgr::nhUdpInstMgr = (CdSuNhUdpInstMgr*) NULL;

//---------------------------------------------------------------------------
// Name		: Constructor of CdSuNhUdpInstMgr ()
// Description  : The constructor in this class is a protected funcitons.
// 		  This is done to achieve the singleton functionality.
//---------------------------------------------------------------------------

CdSuNhUdpInstMgr::CdSuNhUdpInstMgr ()
{
	// Init mutex.
	if (pthread_mutex_init (&dbLock, NULL) == -1)
	{
		CDSU_TRACE (1, "Init Mutex Error\n");
	}
	
	// Initilize maximum NH instance ID allowed.
	maxNhInst = CDSU_NHUDP_MAX_INST;

	// No Network Handler Instances right now.
	noOfNhInst = 0;

	// Initilize the item in DB table to NULL.	
	for (Uint32 i = 0 ; i < CDSU_NHUDP_MAX_INST; i++)
	{
		nhUdpDbTable [i] = NULL;	
	}

} // CdSuNhUdpInstMgr::CdSuNhUdpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpInstMgr::~CdSuNhUdpInstMgr ()
// Description  : The destructor in this class. 
// Inputs       :
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpInstMgr::~CdSuNhUdpInstMgr ()
{
	
} // CdSuNhUdpInstMgr::~CdSuNhUdpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpInstMgr::createNhUdpInstMgr ()
// Description  : This function creates the Network Handler UDP Instance
// 		  Manager when called for the first time. When called more
// 		  than once it simply returns the pointer to the Network
// 		  Handler instance manager.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpInstMgr* CdSuNhUdpInstMgr::createNhUdpInstMgr ()
{
	// if instance Manager is null then create instance Manager
	// else simply return instacne Manager
	if (nhUdpInstMgr == NULL)
	{
		nhUdpInstMgr = new CdSuNhUdpInstMgr ();
		return (nhUdpInstMgr);
	} else 
	{
		return (nhUdpInstMgr);
	} // if () 

} // CdSuNhUdpInstMgr::createNhUdpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpInstMgr::deleteNhUdpInstMgr ()
// Description  : This function deletes the Network Handler UDP Instance
// 		  Manager when nh instance counter is 0.
// 		  If counter already 0 then ignores.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpInstMgr* CdSuNhUdpInstMgr::deleteNhUdpInstMgr ()
{
	// if no of instance of NH = 0 then delete and return NULL
	if (nhUdpInstMgr->noOfNhInst == 0)
	{
		delete nhUdpInstMgr; 
		nhUdpInstMgr = NULL;
	}
	return nhUdpInstMgr;

} // CdSuNhUdpInstMgr::deleteNhUdpInstMgr ()

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpInstMgr::addNhUdpInst ()
// Description  : Adds a Network Handler UDP Instance.
// Inputs       : InitParam.
// Return Value : CdSuNhUdpInstId.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpInstId CdSuNhUdpInstMgr::addNhUdpInst (const CdSuNhUdpMiInitParam& initParam)
{
	pthread_mutex_lock (&dbLock);

	for (Uint32 i = 0; i < maxNhInst; i++)
	{
		if (nhUdpDbTable [i] == NULL)
		{
			noOfNhInst++;
			// Create the NhDb.
			nhUdpDbTable [i] = new CdSuNhUdpDb (initParam, i);
			// Create the Network Handler Active Instance.
			CdSuNhUdp* nhUdp = new CdSuNhUdp (nhUdpDbTable [i]);	
			// Set the nhudp pointer in db.
			(nhUdpDbTable[i])->setNhUdpPtr (nhUdp);
			// Run active class.
			nhUdp->run ();
			pthread_mutex_unlock (&dbLock);
			// Return Instance ID.
			return (i);
		} // if ()
	} // for ()

	pthread_mutex_unlock (&dbLock);
	return (CDSU_NHUDP_INV_NHINSTID);

} // CdSuNhUdpInstMgr::addNhUdpInst ()

//---------------------------------------------------------------------------
// Name		: CdSuNhUdpInstMgr::removeNhUdpInst ()
// Description  : Removes a Network Handler UDP Instance.
// Inputs       : 
// Return Value : boolean.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType CdSuNhUdpInstMgr::removeNhUdpInst (const CdSuNhUdpInstId instId)
{
	// instance id must not be greater or equal to maxNhInst ID.
	if (instId >= maxNhInst) 
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}
	// Lock DB.
	pthread_mutex_lock (&dbLock);
	
	// If nhInstance ID Invalid then following statement returns null
	if (nhUdpDbTable [ instId ] == NULL)
	{
		// Unlock lock return error.
		pthread_mutex_unlock (&dbLock);
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}
	
	// Before deleting make sure that NH is in deleted state.  
	if ((nhUdpDbTable [ instId ])->getNhUdpPtr()->isDeleted ())
	{
		// Get and delete the nhudp pointer. 
		CdSuNhUdp* nhUdp = (nhUdpDbTable[ instId ])->getNhUdpPtr();
		delete nhUdp;

		// Delete the NhDb.
		delete (nhUdpDbTable [ instId ]);
		nhUdpDbTable [ instId ] = NULL;

		noOfNhInst--;
		// Unlock mutex
		pthread_mutex_unlock (&dbLock);
		return (CDSU_NHUDP_API_RT_SUCCESS);
	} else
	{
		// Unlock Mutex
		pthread_mutex_unlock (&dbLock);
		return (CDSU_NHUDP_API_RT_OP_INV);
	}
	
} // CdSuNhUdpInstMgr::removeNhUdpInst ()


//==============================================================================
// <End of cdsunhudpinstmgr.cpp>
//==============================================================================

