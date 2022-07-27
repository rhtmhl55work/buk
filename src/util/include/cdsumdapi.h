#ifndef CDSU_MDAPI_H
#define CDSU_MDAPI_H
//============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsumdapi.h
// Description	: API for CdMessageDistributor class.
// Ref Docs	:
// Author       : Seema Chandak 
// Created On   : 
// Last Modified: Tue Jul 22 10:42:56 IST 2003
//============================================================================

#include "cdsutsqueue.h"
#include "cdsuutiltypes.h"

//----------------------------------------------------------------------------
// Enumerations and Typedefs.
//----------------------------------------------------------------------------
enum CdSuMdIpcpMech
{
	CDSU_IPCP_MECH_INTRA = 1,	// If for same card.
	CDSU_IPCP_MECH_UDP,		// If to another card via UDP.
	CDSU_IPCP_MECH_TCP,		// If to another card via TCP.
	CDSU_IPCP_MECH_PROP,		// If to another card via Proprietary means.
	CDSU_IPCP_MECH_INVALID

}; 

//----------------------------------------------------------------------------
// Udp Info of Remote Card. 
//----------------------------------------------------------------------------

struct CdSuMdUdpPortInfo
{
	Uint32 ipaddr;
	Uint16 portNo;

}; // struct CdSuMdUdpPortInof


//----------------------------------------------------------------------------
// IpcpInfo: Info required to route messages (Inter and Intra card.)
//----------------------------------------------------------------------------

struct CdSuMdIpcpInfo
{
	CdSuMdIpcpMech ipcpMech;
	union
	{
		CdSuTsQueue<CdMessage*>* tsMsgQueue;
		CdSuMdUdpPortInfo remoteUdpInfo;
		// TODO: Add TCP + internal IPCP mechanisms here.
	} u;

}; // struct CdSuMdIpcpInfo

//----------------------------------------------------------------------------
// Parameters needed to initialize the message distributor.
//----------------------------------------------------------------------------

struct CdSuMdInitParam
{
	CdModuleId selfMid;
	Uint32 ipaddr;
	Uint16 udpLpn;

}; // struct CdSuMdInitParam

//----------------------------------------------------------------------------
// Declaration of API: Functions called by the external entity to use the
// functions of message distributor.
//----------------------------------------------------------------------------

bool cdSuMdInit (const CdSuMdInitParam&);
bool cdSuMdTerminate ();
bool cdSuMdRegisterMid (const CdModuleId& modId);
bool cdSuMdRegisterMid (const CdModuleId& modId, CdSuMdIpcpInfo ipcpInfo);
bool cdSuMdRegisterHwUnit (const CdHardwareUnitId& hwid, CdSuMdIpcpInfo ipcpInfo);
bool cdSuMdUnregisterMid (const CdModuleId& modId);

bool cdSuMdSendMsg (CdMessage* msg, int prioriy = 0);

bool cdSuMdRecvMsg (CdMessage** msg, const CdModuleId& mid);
bool cdSuMdRecvMsgNonBlock (CdMessage** msg, const CdModuleId& mid);


//----------------------------------------------------------------------------
// 		Message Buffer handling functions.
//		----------------------------------
//----------------------------------------------------------------------------

inline void* cdSuGetMsgBuf (int size) { return new char [size];}
inline void cdSuFreeMsgBuf (void* buf) { delete [] (char*) buf; }

#define CDSU_GETMSGBUF(msgType) ((msgType*) cdSuGetMsgBuf (sizeof (msgType)))
#define CDSU_FREEMSGBUF(msgPtr) (cdSuFreeMsgBuf ((void*) msgPtr))

//=============================================================================
//	<End of cdsuapi.h>
//=============================================================================
#endif
