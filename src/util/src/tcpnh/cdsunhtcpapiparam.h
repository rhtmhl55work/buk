#ifndef __CDSU_NHTCPAPI_PARAM_H
#define __CDSU_NHTCPAPI_PARAM_H

//============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhtcpapiparam.h
// Description  : API for the Generic Network Handler.
// Author       : Seema Jain
// Created On   : 4th Feb, 2004
// Last Modified: 1. By Sumit Bijwe  on Wed Feb 25 10:21:38 IST 2004
//               	+ Added CdSuNhTcpMiTerminateParam.
//               	+ Added CdSuNhTcpConnKey and CdSuNhTcpLstnKey.
//
//               2.Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//============================================================================

//============================================================================
// Include Files.
//============================================================================

#include "cdsuutiltypes.h"
#include "cdsunhtcptypes.h"
#include "cdsunhtcpstatspkg.h"

//----------------------------------------------------------------------------
// SM -> NH Parameters.
//----------------------------------------------------------------------------

struct CdSuNhTcpMiInitParam
{	
	CdSuActorId nhActorId;			// Network Handler Actor Id.
	CdSuActorId smActorId;			// Management Interface Actor Id.

	Uint32 maxNoOfConnections;		// Maximum no of connections.
	Uint32 maxNoOfLstns;			// Maximum no of Lstns.
	Uint16 maxBufferLen;			// Maximum buffer length.
	Uint32 listenSelectTimer;		// Time to wait in the select call waiting for connections. (microsecs)
	Uint32 dataSelectTimer;			// Time to wait in the select call waiting for data in connection. (microsecs)
	Uint32 noHandleTimer;			// Time to sleep if no fds are there.
	Uint32 listenQSize;			// Maximum length of the queue of pending connections.


	CdSuNhTcpEventHlr smEventHlr;
	
	void init ( CdSuActorId nhActorId, 
			CdSuActorId smActorId,
			CdSuNhTcpEventHlr smEventHlr,
			Uint32 maxNoOfConnections = CDSU_NHTCP_DEF_NOCONNS, 
	        	Uint32 maxNoOfLstns = CDSU_NHTCP_DEF_NOLSTNS,
		 	Uint16 maxTxBufferLen = CDSU_NHTCP_DEF_TCPBUFLEN,
			Uint32 listenSelectTimer = CDSU_NHTCP_DEF_LSTN_SELTMR,
			Uint32 dataSelectTimer = CDSU_NHTCP_DEF_DATA_SELTMR,
			Uint32 noHandleTimer = CDSU_NHTCP_DEF_NOHDL_TMR,
			Uint32 listenQSize = CDSU_NHTCP_DEF_LSTNQ_SIZE);


	void operator = (const CdSuNhTcpMiInitParam& initParam);
}; 

struct CdSuNhTcpMiDelNhParam
{
	CdSuNhTcpInstId nhTcpInstId;		
	
	void init (CdSuNhTcpInstId nhTcpInstId);
	void operator = (const CdSuNhTcpMiDelNhParam& delNhParam);
}; 


struct CdSuNhTcpMiTerminateParam
{
	CdSuNhTcpInstId nhTcpInstId;		
	
	void init (CdSuNhTcpInstId nhTcpInstId);
	void operator = (const CdSuNhTcpMiTerminateParam& termParam);
}; 

struct CdSuNhTcpMiCtrlReqParam
{
	CdSuNhTcpInstId nhTcpInstId;
	CdSuNhTcpSubOpcode subOpcode;		// To indicate type of control req.	

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuNhTcpSubOpcode subOpcode);
	void operator = (const CdSuNhTcpMiCtrlReqParam& ctrlReqParam);
};

struct CdSuNhTcpMiCfgReqParam
{
	CdSuNhTcpInstId nhTcpInstId;
	CdSuNhTcpSubOpcode subOpcode;		// To indicate whether general or initial config request.

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuNhTcpSubOpcode subOpcode);
	void operator = (const CdSuNhTcpMiCfgReqParam& cfgReqParam);

};

struct CdSuNhTcpMiStatusReqParam
{
	CdSuNhTcpInstId nhTcpInstId;

	void init (CdSuNhTcpInstId nhTcpInstId);
			
	void operator = (const CdSuNhTcpMiStatusReqParam& statusReqParam);

};

struct CdSuNhTcpMiStsReqParam 
{
	CdSuNhTcpInstId nhTcpInstId;

	void init (CdSuNhTcpInstId nhTcpInstId);
			
	void operator = (const CdSuNhTcpMiStsReqParam& stsReqParam);

};


//----------------------------------------------------------------------------
// NH -> SM Parameters
//----------------------------------------------------------------------------

struct CdSuNhTcpMiEvent
{
	CdSuNhTcpInstId nhTcpInstId;		// TCP network handler identifier
	CdSuActorId smActorId;			// Report Events to this actor.
	CdSuNhTcpOpcode opcode;

	void init (CdSuNhTcpInstId nhTcpInstId, 
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode);
};

struct CdSuNhTcpMiInitRspEvent: public CdSuNhTcpMiEvent
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode);

	void operator = (const CdSuNhTcpMiInitRspEvent&);

};

struct CdSuNhTcpMiDelNhRspEvent: public CdSuNhTcpMiEvent
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode);

	void operator = (const CdSuNhTcpMiDelNhRspEvent&);
};

struct CdSuNhTcpMiCtrlRspEvent: public CdSuNhTcpMiEvent 
{
	CdSuNhTcpSubOpcode subOpcode;		// To indicate response for which type of control req.
	CdSuNhTcpRspCode rspCode;		// To indicate whether the resp is success/fail.
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode,
			CdSuNhTcpSubOpcode subOpcode);

	void operator = (const CdSuNhTcpMiCtrlRspEvent&);

}; 

struct CdSuNhTcpMiUStaIndEvent: public CdSuNhTcpMiEvent
{
	CdSuNhTcpState nhState;
	CdSuNhTcpRspCode rspCode;	
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpState nhState);

	void operator = (const CdSuNhTcpMiUStaIndEvent&);
};	

struct CdSuNhTcpMiCfgRspEvent: public CdSuNhTcpMiEvent
{
	CdSuNhTcpSubOpcode subOpcode;		// Initial or General
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode,
			CdSuNhTcpSubOpcode subOpcode);

	void operator = (const CdSuNhTcpMiCfgRspEvent&);

};

struct CdSuNhTcpMiStatusRspEvent: public CdSuNhTcpMiEvent
{
	CdSuNhTcpRspCode rspCode;	
	CdSuNhTcpFailCode failCode;
	CdSuNhTcpState nhState;

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode,
			CdSuNhTcpState nhState);

	void operator = (const CdSuNhTcpMiStatusRspEvent&);

};



struct CdSuNhTcpMiStsRspEvent: public CdSuNhTcpMiEvent
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;
	Uint32 counters [CDSU_NHTCP_MAX_CTR ];
	

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId smActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode,
			Ulong* counters);

	void operator = (const CdSuNhTcpMiStsRspEvent&);

};

//----------------------------------------------------------------------------
// Listener & Connection Key.
//----------------------------------------------------------------------------

struct CdSuNhTcpConnKey
{
	CdSuNhTcpConnId connId;
	CdSuNhTcpInstId nhTcpInstId;
	
	void init (CdSuNhTcpConnId cid, CdSuNhTcpInstId nid);

	void operator = (const CdSuNhTcpConnKey&);

}; // struct CdSuNhTcpConnKey

struct CdSuNhTcpLstnKey
{
	CdSuNhTcpLstnId lstnId;
	CdSuNhTcpInstId nhTcpInstId;

	void init (CdSuNhTcpLstnId lid, CdSuNhTcpInstId nid);

	void operator = (const CdSuNhTcpLstnKey&);

}; // struct CdSuNhTcpLstnKey


//----------------------------------------------------------------------------
// UserInterface->NH Parameters
//----------------------------------------------------------------------------

struct CdSuNhTcpUiAddLstnReqParam
{

	CdSuNhTcpInstId nhTcpInstId;		// TCP NH Instance Id.
	CdSuActorId applActorId;		// Report events to this actor.
	CdSuNhTcpEventHlr listenEventHlr;	// Event Hlr for any event on the Lstn
	CdSuNhTcpEventHlr conEventHlr;		// Event Hlr for any event on the connections.
	
	CdSuIpv4Addr ipv4addr;			// IPv4 address of the Lstn.
	CdSuIpv4TcpPortNo tcpPortNo;		// Port no of the Lstn.
	bool tpkt;				// Per Lstn we will have TPKT.

	void init (CdSuNhTcpInstId nhTcpInstId,
			CdSuActorId applActorId,
			CdSuNhTcpEventHlr listenEventHlr,
			CdSuNhTcpEventHlr conEventHlr,
			CdSuIpv4Addr ipv4addr,
			CdSuIpv4TcpPortNo portNo,
			bool tpkt);

	void operator =  (const CdSuNhTcpUiAddLstnReqParam&);
};

struct CdSuNhTcpUiRemLstnReqParam
{
	CdSuNhTcpLstnKey lstnKey;		// Listener Key.
	CdSuActorId applActorId;		// Report events to this actor.
	CdSuNhTcpEventHlr listenEventHlr;	// Event Hlr for any event on the Lstn

	void init (CdSuNhTcpLstnKey lkey,
			CdSuActorId applActorId,
			CdSuNhTcpEventHlr lEvtHlr);
	
	void operator = (const CdSuNhTcpUiRemLstnReqParam&);
};



struct CdSuNhTcpUiConReqParam
{
	CdSuNhTcpInstId nhTcpInstId;		// TCP NH Instance Id.
	CdSuActorId applActorId;		// Report Events to this actor.
	CdSuNhTcpEventHlr conEventHlr;		// Event Hlr for any event on the connections.

	CdSuIpv4Addr dstIpv4Addr;		// Destination Ipaddress for the connection.
	CdSuIpv4TcpPortNo dstPortNo;		// Destination port no for the connection.
	bool tpkt;				// Per Connection we will have TPKT info.
	
	void init (CdSuNhTcpInstId nhTcpInstId,	
			CdSuActorId applActorId,
			CdSuNhTcpEventHlr conEventHlr,
			CdSuIpv4Addr dstIpv4Addr,
			CdSuIpv4TcpPortNo dstPortNo,
			bool tpkt);

	void operator = (const CdSuNhTcpUiConReqParam&);

}; // struct CdSuNhTcpUiConReqParam 

struct CdSuNhTcpUiCloseConReqParam
{
	CdSuNhTcpConnKey connKey;		// Connection Key.
	CdSuActorId applActorId;		// Report Events to this actor.
	CdSuNhTcpCloseConReason closeConReason;	
	CdSuNhTcpEventHlr conEventHlr;		// Event Hlr for any event on the connections.

	void init (CdSuNhTcpConnKey ckey,
			CdSuActorId applActorId,
			CdSuNhTcpCloseConReason closeConReason);

	void operator = (const CdSuNhTcpUiCloseConReqParam&);

}; // struct CdSuNhTcpUiCloseConReqParam 

struct CdSuNhTcpUiDataReqParam
{
	CdSuNhTcpConnKey connKey;		// Connection Key.
	CdSuActorId applActorId;		// Report Events to this actor.
	CdSuNhTcpEventHlr conEventHlr;		// Event Hlr for any event on the connections.
	char* dataPtr;
	Uint16 dataLen;
	
	CdSuNhTcpUiDataReqParam ();
	~CdSuNhTcpUiDataReqParam ();

	void init (CdSuNhTcpConnKey ckey,
			CdSuActorId applActorId,
			CdSuNhTcpEventHlr conEventHlr,
			char* dataPtr,
			Uint16 dataLen);

	void operator = (const CdSuNhTcpUiDataReqParam&);

}; // struct CdSuNhTcpUiDataReqParam

//----------------------------------------------------------------------------
// NH -> User Interface Events.
//----------------------------------------------------------------------------
struct CdSuNhTcpUiLstnEvent
{
	CdSuNhTcpLstnKey lstnKey;		// Listner Key.
	CdSuActorId applActorId;		// Report Events to this actor.
	CdSuNhTcpOpcode opcode;
	
	void init (CdSuNhTcpLstnKey lkey,
			 CdSuActorId applActorId,
			 CdSuNhTcpOpcode opcode);

}; // struct CdSuNhTcpUiLstnEvent

struct CdSuNhTcpUiConnEvent
{
	CdSuNhTcpConnKey connKey;		// Connection Key.
	CdSuActorId applActorId;		// Report Events to this actor.
	CdSuNhTcpOpcode opcode;
	
	void init (CdSuNhTcpConnKey ckey,
			CdSuActorId applActorId,
			CdSuNhTcpOpcode opcode);

}; // struct CdSuNhTcpUiConnEvent


struct CdSuNhTcpUiAddLstnRspEvent : public CdSuNhTcpUiLstnEvent 
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpLstnKey lkey,
			CdSuActorId applActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode);
	
	void operator = (const CdSuNhTcpUiAddLstnRspEvent&);

}; // struct CdSuNhTcpUiAddLstnRspEvent

struct CdSuNhTcpUiRemLstnRspEvent : public CdSuNhTcpUiLstnEvent
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpLstnKey lkey,
			CdSuActorId applActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpRspCode rspCode,
			CdSuNhTcpFailCode failCode);
	
	void operator = (const CdSuNhTcpUiRemLstnRspEvent&);

}; // struct CdSuNhTcpUiRemLstnRspEvent

struct CdSuNhTcpUiConRspEvent: public CdSuNhTcpUiConnEvent 
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;
	
	void init (CdSuNhTcpConnKey ckey,
				CdSuActorId applActorId,
				CdSuNhTcpOpcode opcode,
				CdSuNhTcpRspCode rspCode,
				CdSuNhTcpFailCode failCode);
	
	void operator = (const CdSuNhTcpUiConRspEvent&);

}; // struct CdSuNhTcpUiConRspEvent   

struct CdSuNhTcpUiCloseConRspEvent : public CdSuNhTcpUiConnEvent
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpConnKey ckey,
				CdSuActorId applActorId,
				CdSuNhTcpOpcode opcode,
				CdSuNhTcpRspCode rspCode,
				CdSuNhTcpFailCode failCode);
	
	void operator = (const CdSuNhTcpUiCloseConRspEvent&);

}; // struct CdSuNhTcpUiCloseConRspEvent

struct CdSuNhTcpUiDataRspEvent : public CdSuNhTcpUiConnEvent
{
	CdSuNhTcpRspCode rspCode;
	CdSuNhTcpFailCode failCode;

	void init (CdSuNhTcpConnKey ckey,
				CdSuActorId applActorId,
				CdSuNhTcpOpcode opcode,
				CdSuNhTcpRspCode rspCode,
				CdSuNhTcpFailCode failCode);
	
	void operator = (const CdSuNhTcpUiDataRspEvent&);


}; // struct CdSuNhTcpUiDataRspEvent


struct CdSuNhTcpUiDataIndEvent : public CdSuNhTcpUiConnEvent
{
	char* dataPtr;
	Uint16 dataLen;
	bool tpkt;

	CdSuNhTcpUiDataIndEvent ();
	~CdSuNhTcpUiDataIndEvent ();

	void init (CdSuNhTcpConnKey ckey,
				CdSuActorId applActorId,
				CdSuNhTcpOpcode opcode,
				char* dataPtr,
				Uint16 dataLen,
				bool tpkt);
	
	void operator = (const CdSuNhTcpUiDataIndEvent&);

}; // struct CdSuNhTcpUiDataIndEvent

struct CdSuNhTcpUiConIndEvent : public CdSuNhTcpUiLstnEvent
{
	CdSuNhTcpConnKey connKey;
	bool tpkt;
	CdSuIpv4Addr dstIpv4Addr;	// Destination Ipaddress for the connection.
	CdSuIpv4TcpPortNo dstPortNo;	// Destination port no for the connection.
	
	void init (CdSuNhTcpConnKey ckey,
			CdSuActorId applActorId,
			CdSuNhTcpOpcode opcode,
			CdSuNhTcpLstnKey lkey,
			bool tpkt,
			CdSuIpv4Addr dstIpAddress,
			CdSuIpv4TcpPortNo dstPortNo);

	void operator = (const CdSuNhTcpUiConIndEvent&);

}; // struct CdSuNhTcpUiConIndEvent

struct  CdSuNhTcpUiCloseConIndEvent : public CdSuNhTcpUiConnEvent
{
	void init (CdSuNhTcpConnKey ckey,
				CdSuActorId applActorId,
				CdSuNhTcpOpcode opcode);

	void operator = (const CdSuNhTcpUiCloseConIndEvent&);


}; // struct CdSuNhTcpUiCloseConIndEvent

//----------------------------------------------------------------------------
//	<End of file>
//----------------------------------------------------------------------------
#endif

