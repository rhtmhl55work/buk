#ifndef __CDSU_NHUDP_DB_H__
#define __CDSU_NHUDP_DB_H__
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhudpdb.h 
// Description	: 
// Ref Docs     : 
// Author       : Sumit Bijwe 
// Created On   : Mon Jul 21 18:15:10 IST 2003
// Last Modified:
// Reference	:
//============================================================================

//----------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------


#include "cdsutsqueue.h"
#include "cdsunhudpglobals.h"
#include "cdsunhudptxptsrvc.h"


class CdSuNwhlrDb
{
public:
	CdSuNwhlrDb () 
	{
		udpTxptServ = (CdSuUdpTxptServices*) NULL;
		nwQ = (CdSuTsQueue<CdMessage*>*) NULL;
		applQ = (CdSuTsQueue<CdMessage*>*) NULL;
	}
	~CdSuNwhlrDb () { }

	// Network handler queue.
	void setNetworkMsgQ (CdSuTsQueue<CdMessage*>* q) { nwQ = q; }
	CdSuTsQueue<CdMessage*>* getUdpNtwkMesgQ () { return (nwQ); }

	// Application queue.
	void setApplMesgQ (CdSuTsQueue<CdMessage*>* q) { applQ = q; }
	CdSuTsQueue<CdMessage*>* getApplMesgQ () { return (applQ); }


	void setTxptServUdp (CdSuUdpTxptServices* tsrv) { udpTxptServ = tsrv; }
	CdSuUdpTxptServices* getTxptServUdp () { return udpTxptServ; } 
	
	void setSelfIpAddr (Uint32 ipa) { ipAddr = ipa; }
	Uint32 getSelfIpAddr () { return (ipAddr); }
	
	void setUdpListenPort (Uint16 port) { portNo = port; }
	Uint32 getUdpListenPort () { return (portNo); }


	void setUdpApplMid (CdModuleId apid) { applMid = apid; }
	CdModuleId getUdpApplMid () { return (applMid); }


	void setUdpNwHlrMid (CdModuleId nwid) { nwHlrMid = nwid; }
	CdModuleId getUdpNwHlrMid () { return (nwHlrMid); }

private:
	
	CdSuUdpTxptServices* udpTxptServ; 
	CdSuTsQueue<CdMessage*>* nwQ;
	CdSuTsQueue<CdMessage*>* applQ;
	CdModuleId applMid;
	CdModuleId nwHlrMid;
	
	Uint32 ipAddr;
	Uint16 portNo;
};

#endif

//============================================================================
// End of cdsunhudpdb.h
//============================================================================
