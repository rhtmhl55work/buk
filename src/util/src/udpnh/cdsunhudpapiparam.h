#ifndef __CDSU_NHUDPPARAM_H__
#define __CDSU_NHUDPPARAM_H__

//============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhudpparam.h
// Description  : Contains Definitions for all type of Parameter user need to
// 		: know(Exposed parameter types).
// Author       : Vijaya Kumar
// Created On   : Mon Mar  8 16:28:57 IST 2004
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//		  Unix Network Programming by W. Rechard Steavens	
//============================================================================

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------

#include "cdsunhudptypes.h" 	

//----------------------------------------------------------------------------
//	 		SM -> NH Parameters.
//	 		--------------------
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiInitParam
// Description 	: parameter passed to cdSuNhUdpMiInit () function.
//----------------------------------------------------------------------------
struct CdSuNhUdpMiInitParam
{
	CdSuNhUdpMiInitParam ();	// Constructor.

	// Configurable parameter list
	CdSuActorId nhActorId;		// Network Handler Actor Id.
	CdSuActorId smActorId;		// Management Interface Actor Id. Incase of default event handler to post message destination actor id must.
	Uint32 maxNoOfUdpPort;		// Maximum no of Udp Port that NH Must support.
	Uint16 maxBufferLen;		// Maximum receiver buffer length.
	Uint32 dataSelectTimer;		// Time to wait in the select call waiting for data in connection. (microsecs)
	Uint32 noHandleTimer;		// Time to sleep if no fds are there.
	CdSuNhUdpEventHlr smEventHlr;  	// Call back functions pointer for notifying events to application
	
	// Parameter initializing Functions.
	void init (CdSuActorId nhActorId,
			CdSuActorId smActorId,
			Uint32 maxUdpPort = CDSU_NHUDP_MAX_UDPPORT,
		       	Uint16 maxBufferLen = CDSU_NHUDP_DEF_BUFLEN,
		       	Uint32 dataSelectTimer = CDSU_NHUDP_DEF_DATA_SELTMR,
		       	Uint32 noHandleTimer = CDSU_NHUDP_DEF_NOHDL_TMR,
			CdSuNhUdpEventHlr smEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR);
	
}; // struct CdSuNhUdpMiInitParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiDelNhParam
// Description 	: parameter passed to cdSuNhUdpMiDelNhReq () function.
//----------------------------------------------------------------------------

struct CdSuNhUdpMiDelNhParam
{
	
	CdSuNhUdpInstId nhUdpInstId;	// Instance Id of NH to be deleted.	
	
}; //  struct CdSuNhUdpMiDelNhParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiTerminateParam
// Description 	: parameter passed to cdSuNhUdpMiTerminate () function.
//----------------------------------------------------------------------------

struct CdSuNhUdpMiTerminateParam
{
	
	CdSuNhUdpInstId nhUdpInstId;	// Instance Id of NH to be terminated.
	
}; // struct CdSuNhUdpMiTerminateParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiCtrlReqParam
// Description 	: parameter passed to cdSuNhUdpMiCtrlReq () function.
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCtrlReqParam
{
	
	CdSuNhUdpInstId nhUdpInstId;	// Instance ID of NH over which control operation to be done.
	CdSuNhUdpSubOpcode subOpcode;	// To indicate type of control req.

}; // struct CdSuNhUdpMiCtrlReqParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiCfgReqParam
// Description 	: parameter passed to cdSuNhUdpMiCfgReq () function.
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCfgReqParam
{
	CdSuNhUdpInstId nhUdpInstId;	// Instance ID of NH over which configuration to be done.
	CdSuNhUdpSubOpcode subOpcode;	// To indicate whether general or initial config request.
	
}; // struct CdSuNhUdpMiCfgReqParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiStatusReqParam
// Description 	: parameter passed to cdSuNhUdpMiStatusReq () function.
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStatusReqParam
{
	
	CdSuNhUdpInstId nhUdpInstId; 	// Instance ID Of NH of which status required.

}; // struct CdSuNhUdpMiStatusReqParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiStsReqParam
// Description 	: parameter passed to cdSuNhUdpMiStsReq () function.
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStsReqParam 
{

	CdSuNhUdpInstId nhUdpInstId; // Instance ID of NH of which statistics report required.
	
}; // struct CdSuNhUdpMiStsReqParam 

//----------------------------------------------------------------------------
// 				NH -> SM Parameters
// 				-------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiEvent
// Description 	: Base Struct of Mi Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiEvent
{
	virtual ~CdSuNhUdpMiEvent () {}	// Virtual Distructor
	CdSuNhUdpInstId nhInstId;	// UDP network handler identifier
	CdSuActorId smActorId;		// Actor Id Of Management Module
	CdSuNhUdpOpcode opcode;		// Opcode indicates type of event.
	
	void init (CdSuNhUdpInstId nhInstId, 
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode);
}; // struct CdSuNhUdpMiEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiInitRspEvent
// Description 	: Init Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiInitRspEvent: public CdSuNhUdpMiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Init Request is fail or success.
	CdSuNhUdpRspFailCode failCode;	// Indicates fail reason.(valid only if fail)

	void init (CdSuNhUdpInstId nhInstId,
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode,
			CdSuNhUdpRspCode rspCode);

	void init (CdSuNhUdpInstId nhInstId,
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpRspFailCode failCode);

}; // struct CdSuNhUdpMiInitRspEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiDelNhRspEvent
// Description 	: Delete Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiDelNhRspEvent: public CdSuNhUdpMiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Del Nh request is fail or success
	CdSuNhUdpRspFailCode failCode; 	// Indicates fail reason.
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId);
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpRspFailCode failCode);
}; // struct CdSuNhUdpMiDelNhRspEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiCtrlRspEvent
// Description 	: Control Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCtrlRspEvent: public CdSuNhUdpMiEvent 
{
	CdSuNhUdpRspCode rspCode;	// To indicate whether the Control Req is success/fail.
	CdSuNhUdpSubOpcode subOpcode;	// To indicate response for which type of control req.
	CdSuNhUdpRspFailCode failCode; 	// fail reason.
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpSubOpcode subOpcode);
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpSubOpcode subOpcode,
			CdSuNhUdpRspFailCode failCode);
}; // struct CdSuNhUdpMiCtrlRspEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiCfgRspEvent
// Description 	: Config Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCfgRspEvent: public CdSuNhUdpMiEvent
{
	CdSuNhUdpRspCode rspCode;	// Tells Configuration request is fail or success.
	CdSuNhUdpSubOpcode subOpcode;	// Configuration Type (initial or general)
	CdSuNhUdpRspFailCode failCode; 	// fail reason.
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpSubOpcode subOpcode);
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpSubOpcode subOpcode,
			CdSuNhUdpRspFailCode failCode);
}; // struct CdSuNhUdpMiCfgRspEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiStatusRspEvent
// Description 	: Status Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStatusRspEvent: public CdSuNhUdpMiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Status Request is fail or success
	CdSuNhUdpState nhState;		// State Of the NH. Valid Only if success.
	CdSuNhUdpRspFailCode failCode; 	// fail reason.
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpState nhState);
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode,
			CdSuNhUdpRspCode rspCode,
			CdSuNhUdpRspFailCode failCode);
}; // struct CdSuNhUdpMiStatusRspEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiStsRspEvent
// Description 	: Statistics Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStsRspEvent: public CdSuNhUdpMiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Statistics request is fail or success.
	Uint32 counters[ CDSU_NHUDP_MAX_CTR ];	// statistics counters. Valid only if success.
	CdSuNhUdpRspFailCode failCode; 	// fail reason.
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			const Ulong* counters);
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpRspFailCode failCode);
};

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpMiUStaIndEvent
// Description 	: Unsolicated Status Indication Event
//----------------------------------------------------------------------------

struct CdSuNhUdpMiUStaIndEvent: public CdSuNhUdpMiEvent
{
	CdSuNhUdpState nhState;		// State of NH
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId smActorId,
			CdSuNhUdpOpcode opcode,
			CdSuNhUdpState nhState);
}; // struct CdSuNhUdpMiUStaIndEvent


//----------------------------------------------------------------------------
//			 User -> NH Parameters
//			 ---------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiAddUdpPortReqParam
// Description 	: Add Udp Port Request Param.
//----------------------------------------------------------------------------

struct CdSuNhUdpUiAddUdpPortReqParam
{
	CdSuNhUdpUiAddUdpPortReqParam ();	// constructor

	CdSuNhUdpInstId  nhUdpInstId;		// Nh Instance Id over which Udp port to be added.
	CdSuActorId applActorId;		// Application Actor ID.

	CdSuIpv4UdpPortNo localPortNo;		// local port number.
	CdSuIpv4Addr localAddr;			// local ipv4 address.
	
	CdSuIpv4UdpPortNo destPortNo;		// Invalid means, udp port not connected.
 	CdSuIpv4Addr destAddr;			// destination ipv4 address.	
	
	CdSuNhUdpEventHlr applEventHlr;		// Callback function pointer.

	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId applActorId,
			CdSuIpv4UdpPortNo localPortNo,
			CdSuIpv4Addr localAddr, 
			CdSuNhUdpEventHlr applEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR,
			CdSuIpv4UdpPortNo destPortNo = CDSU_NHUDP_INV_UDPPORT,
			CdSuIpv4Addr destAddr = 0);

}; // struct CdSuNhUdpUiAddUdpPortReqParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiAddUdpPortReqParam
// Description 	: Remove Udp Port Request Param.
//----------------------------------------------------------------------------

struct CdSuNhUdpUiRemUdpPortReqParam
{
	CdSuNhUdpUiRemUdpPortReqParam ();	// Constructor
	
	CdSuNhUdpPortKey udpPortKey;		// Each Udp port idenitified by key(Nh Instance Id + Udp Port Id).
	CdSuActorId applActorId;		// Application Actor ID.
	CdSuNhUdpEventHlr applEventHlr; 	// Callback Function Pointer. 

	void init (CdSuNhUdpPortKey udpPortKey,
			CdSuActorId applActorId,
			CdSuNhUdpEventHlr applEventHlr =  (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR);
	
}; // struct CdSuNhUdpUiRemUdpPortReqParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiDataReqParam
// Description 	: Data Request Param
//----------------------------------------------------------------------------

struct CdSuNhUdpUiDataReqParam
{
	CdSuNhUdpUiDataReqParam ();	// Constructor
	
	CdSuNhUdpPortKey udpPortKey;	// Each Udp port identified by key (Nh Instance Id + Udp Port Id). 
	CdSuActorId applActorId;	// Application Actor ID.
	char* dataPtr;			// data pointer.
	Uint16 dataLen;			// data length.
	Uint32 opaqueData;		// opaque data.
	CdSuIpv4UdpPortNo destPortNo;	// Destination Port Number
	CdSuIpv4Addr destAddr;		// Destination Address.
	bool isAckReq;			// Is Response Required.
	CdSuNhUdpEventHlr applEventHlr; // check whether it is required. 
	
	void init (CdSuNhUdpPortKey udpPortKey,
			CdSuActorId applActorId,
			char* dataPtr,
			Uint16 dataLen,
			Uint32 opaqueData = 0,
			bool isAckReq = true,
			CdSuNhUdpEventHlr applEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR,
			CdSuIpv4UdpPortNo destPortNo = CDSU_NHUDP_INV_UDPPORT,
			CdSuIpv4Addr destAddr = 0);
	
	// Overloading opeartor =  
	void operator = (const CdSuNhUdpUiDataReqParam&);
	
}; // struct CdSuNhUdpUiDataReqParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiSendReqParam
// Description 	: Send Request Param (To be Removed)
//----------------------------------------------------------------------------

struct CdSuNhUdpUiSendReqParam
{
	CdSuNhUdpUiSendReqParam ();	// Constructor
	
	CdSuNhUdpInstId nhUdpInstId; 	// Instance ID.
	CdSuActorId applActorId;	// Where to response.
	char* dataPtr;			// data pointer.
	Uint16 dataLen;			// data length.
	Uint32 opaqueData;		// opaque data.
	CdSuIpv4UdpPortNo destPortNo;	// Destination Port Number
	CdSuIpv4Addr destAddr;		// Destination Address.
	CdSuNhUdpEventHlr applEventHlr; // Callback function Pointer.
       	
	void operator = (const CdSuNhUdpUiSendReqParam&);
	
}; // struct CdSuNhUdpUiDataReqParam

//----------------------------------------------------------------------------
// 				NH-->User Parameters
// 				--------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiEvent
// Description 	: Base Struct of UI Event
//----------------------------------------------------------------------------

struct CdSuNhUdpUiEvent
{
	virtual ~CdSuNhUdpUiEvent () {}
	
	CdSuNhUdpPortKey nhUdpPortKey;	// Each Udp port identified by key (Nh Instance ID + Udp Port Key).
	CdSuActorId applActorId;	// Application Actor ID.
	CdSuNhUdpOpcode opcode;		// Indicates type of Event. 

	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			CdSuNhUdpOpcode opcode);

	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId applActorId,
			CdSuNhUdpOpcode opcode);

}; // struct CdSuNhUdpUiEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiAddUdpPortRspEvent
// Description 	: Add Udp Port Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpUiAddUdpPortRspEvent : public CdSuNhUdpUiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Add Udp Port Request Is Fail or Success.
	CdSuIpv4UdpPortNo localPortNo;	// If ephermal port then to tell udp port selected by system.
	CdSuNhUdpRspFailCode failCode; 	// fail reason.
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			CdSuIpv4UdpPortNo nhUdpPort);
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			CdSuIpv4UdpPortNo nhUdpPort,
			CdSuNhUdpRspFailCode failCode);

}; // CdSuNhUdpUiAddUdpPortRspEventParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiRemUdpPortRspEvent
// Description 	: Rem Udp Port Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpUiRemUdpPortRspEvent : public CdSuNhUdpUiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Rem Udp Port Request Is Fail or Success
	CdSuNhUdpRspFailCode failCode;	// fail reason.
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId);
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			CdSuNhUdpRspFailCode failCode);

}; // CdSuNhUdpUiRemUdpPortRspEvent

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiDataRspEvent
// Description 	: Data Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpUiDataRspEvent : public CdSuNhUdpUiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Data Request is fail or success.
	Uint32 opaqueData;		// To reflect opaque of data in data request back to user.
	CdSuNhUdpRspFailCode failCode; 	// fail reason.

	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			Uint32 opaqueData);
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			Uint32 opaqueData,
			CdSuNhUdpRspFailCode failCode);

}; // CdSuNhUdpUiDataRspEventParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiSendRspEvent
// Description 	: Send Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpUiSendRspEvent : public CdSuNhUdpUiEvent
{
	CdSuNhUdpRspCode rspCode;	// Indicates Send Request Is fail or success.
	Uint32 opaqueData;		// opaque data.
	CdSuNhUdpRspFailCode failCode; 	// fail reason.

	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId applActorId,
			Uint32 opaqueData);
	
	void init (CdSuNhUdpInstId nhUdpInstId,
			CdSuActorId applActorId,
			Uint32 opaqueData,
			CdSuNhUdpRspFailCode failCode);

}; // CdSuNhUdpUiSendRspEventParam

//----------------------------------------------------------------------------
// Stuct 	: CdSuNhUdpUiDataIndEvent
// Description 	: Send Response Event
//----------------------------------------------------------------------------

struct CdSuNhUdpUiDataIndEvent : public CdSuNhUdpUiEvent
{
	char* dataPtr;			// data pointer.
	Uint16 dataLen;			// data length.
	CdSuIpv4Addr localAddr;		// local address on which data received.
	CdSuIpv4UdpPortNo localPortNo; 	// local port on whicd data received.
	CdSuIpv4UdpPortNo destPortNo; 	// peer end port number from which data received.
	CdSuIpv4Addr destAddr;		// peer end address from which data received.
	
	CdSuNhUdpRspCode rspCode;	// Indicates Data Indication is error type or success type
	CdSuNhUdpRspFailCode failCode; 	// fail reason.
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			char* dataPtr,
			Uint16 dataLen,
			CdSuIpv4UdpPortNo localPortNo,
			CdSuIpv4Addr localAddr,
			CdSuIpv4UdpPortNo destPortNo,
			CdSuIpv4Addr destAddr);
	
	void init (CdSuNhUdpPortKey nhUdpPortKey,
			CdSuActorId applActorId,
			char* dataPtr,
			Uint16 dataLen,
			CdSuIpv4UdpPortNo localPortNo,
			CdSuIpv4Addr localAddr,
			CdSuIpv4UdpPortNo destPortNo,
			CdSuIpv4Addr destAddr,
			CdSuNhUdpRspFailCode failCode);

}; // CdSuNhUdpUiDataIndEvent 

//============================================================================
// <End of cdsunhudpapiparam.h>
//============================================================================
#endif
