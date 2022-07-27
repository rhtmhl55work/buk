#ifndef CDSU_MSGDISTR_H_
#define CDSU_MSGDISTR_H_

//============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsumsgdistributer.h
// Description	: Distributes the msg within the same processor or other
// 		  processors.
// Ref Docs	:
// Author       : Seema Chandak 
// Created On   : 
// Last Modified: Mon Jul 21 16:46:39 IST 2003
//============================================================================

//------------------------------------------------------------------------------
//	Include Files
//------------------------------------------------------------------------------

#include <pthread.h>
#include "cdsuutiltypes.h"
#include "cdsumdapi.h"
#include "cdsumbhash.h"
#include "cdsunhudpglobals.h"

enum
{
	CDSU_MD_MAX_NUM_MESSAGES = 1024,
	CDSU_UDP_LISTEN_PORT_NO = 8001
};

//------------------------------------------------------------------------------
// Class	: CdSuMsgDistributor
// Description	: Distributes message to another card.
//------------------------------------------------------------------------------

class CdSuMsgDistributor
{
public:
	CdSuMsgDistributor (const CdSuMdInitParam& initParam);
	~CdSuMsgDistributor ();

	bool start ();	// Activates Message Distributor
	
	bool registerMid (const CdModuleId& mid);
	bool registerMid (const CdModuleId& mid, const CdSuMdIpcpInfo& ipcpInfo);
	bool unregisterMid (const CdModuleId& mid);
	//
	bool sendMsg (CdMessage* msg, int priority = 0 );
	//
	bool recvMsg (CdMessage** msg, const CdModuleId& modId);
	bool recvMsgNonBlock (CdMessage** msg, const CdModuleId& modId);
	
private:
	//--------------------------------------------------------------------
	// Enumerations and typedefs.
	//-------------------------------------------------------------------- 
	struct DestModInfo
	{
		CdModuleId modId;
		CdSuMdIpcpInfo ipcpInfo;
	};
	
	//--------------------------------------------------------------------
	// Helper Functions.
	//--------------------------------------------------------------------
	void createUdpNwHlrThread (CdSuMdUdpNhInitParam* initParam);
	//--------------------------------------------------------------------
	// Attributes.
	//--------------------------------------------------------------------
	Uint32 selfIpAddr;
	CdModuleId selfMid;
	Uint16 udpLpn;
	CdSuMultiByteHash<DestModInfo*>* moduleInfoTable;
	pthread_mutex_t mdMutex;

	//--------------------------------------------------------------------
	// UDP Network Handler attributes.
	//--------------------------------------------------------------------
	CdSuMdUdpNhInitParam* nhInitParam;
	CdModuleId nwHlrMid;
	CdSuMdUdpPortInfo localUdpInfo;
	CdSuTsQueue <CdMessage*>* udpNhQueue;
	pthread_t udpNhThreadId;

}; // class CdSuMsgDistributor

#endif

//============================================================================
//	<End of cdsumd.h>
//============================================================================
