#ifndef __CDSU_NHUDP_INSTMGR_
#define __CDSU_NHUDP_INSTMGR_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpinstmgr.h
// Description	: It contains  definitions for UDP Nh Inst Mgr class.
// Author       : Vijay Kumar
// Created On   : Tue Mar  9 14:10:12 IST 2004
// Last Modified: 
//============================================================================


//----------------------------------------------------------------------------
// Include files.
//----------------------------------------------------------------------------

#include <pthread.h>
#include "cdsunhudptypes.h"
#include "cdsunhudpdb.h"

//----------------------------------------------------------------------------
// Class	: CdSuNhUdpInstMgr
// Description	: It is the Managing entity of all the UDP Network Handlers
// 		  in the system. It has pointers to the data base of all the
// 		  UDP Network Handlers. It is a Singleton object.
//----------------------------------------------------------------------------

class CdSuNhUdpInstMgr
{
public:
	// Destroctor
	~CdSuNhUdpInstMgr ();

	// Method to add Udp Nh Instance.
	CdSuNhUdpInstId addNhUdpInst (const CdSuNhUdpMiInitParam&);

	// Method to remove UDP Nh instance.
	CdSuNhUdpApiRetType removeNhUdpInst (const CdSuNhUdpInstId);

	// Method to get singleton object of instance manager.
	static CdSuNhUdpInstMgr* createNhUdpInstMgr ();
	
	// Method to delete singleton object of instance manager.
	static CdSuNhUdpInstMgr* deleteNhUdpInstMgr ();
	
	// Method to get UDP Nh Db associated with UDP Nh.
	inline CdSuNhUdpDb* getNhUdpDb (CdSuNhUdpInstId nhInstId) 
	{
		return (nhUdpDbTable [nhInstId]);
	}

private:

	//--------------------------------------------------------------------
	// Private methods.
	//--------------------------------------------------------------------
	
	CdSuNhUdpInstMgr ();	// Constructor

	// Allocate Instance ID
	int allocateNhInstId ();

	//--------------------------------------------------------------------
	// Private attributes
	//--------------------------------------------------------------------
	
	// Static Pointer to Singelton Object
	static CdSuNhUdpInstMgr* nhUdpInstMgr;

	// DB Lock
	pthread_mutex_t dbLock;
	
	// Table Containing Pointer NH Instance
	CdSuNhUdpDb* nhUdpDbTable[ CDSU_NHUDP_MAX_INST ];
	
	// Maximum No Of NH Instance.
	Uint32 maxNhInst;
	// Current No Of NH Instance.
	Uint32 noOfNhInst;

}; // class CdSuNhUdpInstMgr

//============================================================================
// < End of file>
//============================================================================
#endif
