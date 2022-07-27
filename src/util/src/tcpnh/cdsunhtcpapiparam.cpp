//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpapiparam.cpp
// Description	: This contains the definitions of the functions of the
// 		  various parameters and events used to fill the messages.
// Author       : Seema Jain
// Created On   : Thu Feb 12 19:20:30 IST 2004
// Last Modified: 1. By Sumit Bijwe  on Wed Feb 25 10:21:38 IST 2004
//               	+ Added CdSuNhTcpMiTerminateParam.
//		: 2.  Tue Apr 18 16:06:20 IST 2006
//			By : Vijaya Kumar
//   			Following bugs entered in bugzilla are fixed
//			 - 84	
//		  3. Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

//============================================================================
// Include files.
//============================================================================

#include "string.h"
#include "cdsuutiltypes.h"
#include "cdsunhtcpapiparam.h"

//============================================================================
// Function Declaration of Parameters in SM->NH Interface.
//============================================================================

//============================================================================
// Function	: CdSuNhTcpMiInitParam::init ()
// Description	: Initializes the Parameters require to create and
// 		  initialize a Network Handler Instance.
//============================================================================

void CdSuNhTcpMiInitParam::init ( CdSuActorId nhAid,
				CdSuActorId smAid,
				CdSuNhTcpEventHlr smEvHlr,
				Uint32 maxCon,
				Uint32 maxLstn,
				Uint16 maxBuf,
				Uint32 lstnSelTmr,
				Uint32 dataSelTmr,
				Uint32 noHdlTmr,
				Uint32 lqs)
{
	nhActorId = nhAid;
	smActorId = smAid;
	smEventHlr = smEvHlr;
	
	maxNoOfConnections = maxCon;
	maxNoOfLstns = maxLstn;
	maxBufferLen = maxBuf;
	listenSelectTimer = lstnSelTmr;
	dataSelectTimer = dataSelTmr;
	noHandleTimer = noHdlTmr;
	listenQSize = lqs;

} // void CdSuNhTcpMiInitParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiInitParam::= ()
// Description	: Overload Equal To Operator for miInitParam.
//============================================================================

void CdSuNhTcpMiInitParam::operator = (const CdSuNhTcpMiInitParam& initParam)
{
	nhActorId = initParam.nhActorId;
	smActorId = initParam.smActorId;
	smEventHlr = initParam.smEventHlr;

	maxNoOfConnections = initParam.maxNoOfConnections;
	maxNoOfLstns = initParam.maxNoOfLstns;
	maxBufferLen = initParam.maxBufferLen;
	listenSelectTimer = initParam.listenSelectTimer;
	dataSelectTimer = initParam.dataSelectTimer;
	noHandleTimer = initParam.noHandleTimer;
	listenQSize = initParam.listenQSize;

} // void CdSuNhTcpMiInitParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiDelNhParam::init ()
// Description	: Initializes the parameters required for deletion of
// 		  Network Handler Instance.
//============================================================================

void CdSuNhTcpMiDelNhParam::init (CdSuNhTcpInstId nhid)
{
	nhTcpInstId = nhid;

} // void CdSuNhTcpMiDelNhParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiDelNhParam::= ()
// Description	: Overload Equal To Operator for DelNhParam.
//============================================================================

void CdSuNhTcpMiDelNhParam::operator = (const CdSuNhTcpMiDelNhParam& delNhParam)
{
	nhTcpInstId = delNhParam.nhTcpInstId;

} //CdSuNhTcpMiDelNhParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiTerminateParam::init ()
// Description	: Initializes the parameters required for deletion of
// 		  Network Handler Instance.
//============================================================================

void CdSuNhTcpMiTerminateParam::init (CdSuNhTcpInstId nhid)
{
	nhTcpInstId = nhid;

} // void CdSuNhTcpMiTerminateParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiTerminateParam::= ()
// Description	: Overload Equal To Operator for TerminateParam.
//============================================================================

void CdSuNhTcpMiTerminateParam::operator = (const CdSuNhTcpMiTerminateParam& delNhParam)
{
	nhTcpInstId = delNhParam.nhTcpInstId;

} //CdSuNhTcpMiTerminateParam::operator = ()


//============================================================================
// Function	: CdSuNhTcpMiCtrlReqParam::init ()
// Description	: Initializes the parameters required for CtrlReq.
//============================================================================

void CdSuNhTcpMiCtrlReqParam::init (CdSuNhTcpInstId nhid, 
					CdSuNhTcpSubOpcode subopc)
{
	nhTcpInstId = nhid;
	subOpcode = subopc;

} // void CdSuNhTcpMiCtrlReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiCtrlReqParam::= ()
// Description	: Overload Equal To Operator for CtrlReqParam.
//============================================================================

void CdSuNhTcpMiCtrlReqParam::operator = (const CdSuNhTcpMiCtrlReqParam& ctrlReqParam)
{
	nhTcpInstId = ctrlReqParam.nhTcpInstId;
	subOpcode = ctrlReqParam.subOpcode;

} //CdSuNhTcpMiCtrlReqParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiCfgReqParam::init ()
// Description	: Initializes the parameters required for CfgReq.
//============================================================================

void CdSuNhTcpMiCfgReqParam::init (CdSuNhTcpInstId nhid, 
					CdSuNhTcpSubOpcode subopc)
{
	nhTcpInstId = nhid;
	subOpcode = subopc;

} // void CdSuNhTcpMiCfgReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiCfgReqParam::= ()
// Description	: Overload Equal To Operator for CfgReqParam.
//============================================================================

void CdSuNhTcpMiCfgReqParam::operator = (const CdSuNhTcpMiCfgReqParam& cfgReqParam)
{
	nhTcpInstId = cfgReqParam.nhTcpInstId;
	subOpcode = cfgReqParam.subOpcode;

} //CdSuNhTcpMiCfgReqParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiStatusReqParam::init ()
// Description	: Initializes the parameters required for StatusReq.
//============================================================================

void CdSuNhTcpMiStatusReqParam::init (CdSuNhTcpInstId nhid)
					
{
	nhTcpInstId = nhid;

} // void CdSuNhTcpMiStatusReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiStatusReqParam::= ()
// Description	: Overload Equal To Operator for StatusReqParam.
//============================================================================

void CdSuNhTcpMiStatusReqParam::operator = (const CdSuNhTcpMiStatusReqParam& statusReqParam)
{
	nhTcpInstId = statusReqParam.nhTcpInstId;

} //CdSuNhTcpMiStatusReqParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiStsReqParam::init ()
// Description	: Initializes the parameters required for StsReq.
//============================================================================

void CdSuNhTcpMiStsReqParam::init (CdSuNhTcpInstId nhid)
{
	nhTcpInstId = nhid;

} // void CdSuNhTcpMiStsReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpMiStsReqParam::= ()
// Description	: Overload Equal To Operator for StsReqParam.
//============================================================================

void CdSuNhTcpMiStsReqParam::operator = (const CdSuNhTcpMiStsReqParam& stsReqParam)
{
	nhTcpInstId = stsReqParam.nhTcpInstId;

} //CdSuNhTcpMiStsReqParam::operator = ()

//============================================================================
// Function Declaration of parameters in the NH->SM Interface. 
//============================================================================

//============================================================================
// Function	: CdSuNhTcpMiEvent::init ()
// Description	: Initializes the parameters of the base class MiEvent.
//============================================================================

void CdSuNhTcpMiEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc)
{
	nhTcpInstId = nhid;
	smActorId = smid;
	opcode = opc;

} // void CdSuNhTcpMiEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiInitRspEvent::init ()
// Description	: To be filled for Init Response Event.
//============================================================================
void CdSuNhTcpMiInitRspEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc)
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	rspCode = rspc;
	failCode = failc;	

} // void CdSuNhTcpMiInitRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiInitRspEvent::= ()
// Description	: Overload Equal To Operator for Init Response Event.
//============================================================================

void CdSuNhTcpMiInitRspEvent::operator = 
			(const CdSuNhTcpMiInitRspEvent& initRspEvent)
{
	nhTcpInstId = initRspEvent.nhTcpInstId;
	smActorId = initRspEvent.smActorId;
	opcode = initRspEvent.opcode;
	rspCode = initRspEvent.rspCode;
	failCode = initRspEvent.failCode;

} //CdSuNhTcpMiInitRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiDelNhRspEvent::init ()
// Description	: To be filled for Delete Network Handler Response Event.
//============================================================================
void CdSuNhTcpMiDelNhRspEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc)
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	rspCode = rspc;
	failCode = failc;

} // void CdSuNhTcpMiDelNhRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiDelNhRspEvent::= ()
// Description	: Overload Equal To Operator for Init Response Event.
//============================================================================

void CdSuNhTcpMiDelNhRspEvent::operator = 
			(const CdSuNhTcpMiDelNhRspEvent& delNhRspEvent)
{
	nhTcpInstId = delNhRspEvent.nhTcpInstId;
	smActorId = delNhRspEvent.smActorId;
	opcode = delNhRspEvent.opcode;
	rspCode = delNhRspEvent.rspCode;
	failCode = delNhRspEvent.failCode;

} //CdSuNhTcpMiDelNhRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiCtrlRspEvent::init ()
// Description	: To be filled for Control Response Event.
//============================================================================
void CdSuNhTcpMiCtrlRspEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc,
				CdSuNhTcpSubOpcode subopc)
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	rspCode = rspc;
	failCode = failc;
	subOpcode = subopc;

} // void CdSuNhTcpMiCtrlRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiCtrlRspEvent::= ()
// Description	: Overload Equal To Operator for Control Response Event.
//============================================================================

void CdSuNhTcpMiCtrlRspEvent::operator = 
			(const CdSuNhTcpMiCtrlRspEvent& ctrlRspEvent)
{
	nhTcpInstId = ctrlRspEvent.nhTcpInstId;
	smActorId = ctrlRspEvent.smActorId;
	opcode = ctrlRspEvent.opcode;
	rspCode = ctrlRspEvent.rspCode;
	failCode = ctrlRspEvent.failCode;
	subOpcode = ctrlRspEvent.subOpcode;

} //CdSuNhTcpMiCtrlRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiCfgRspEvent::init ()
// Description	: To be filled for Config Response Event.
//============================================================================
void CdSuNhTcpMiCfgRspEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc,
				CdSuNhTcpSubOpcode subopc)
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	rspCode = rspc;
	failCode = failc;
	subOpcode = subopc;

} // void CdSuNhTcpMiCfgRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiCfgRspEvent::= ()
// Description	: Overload Equal To Operator for Config Response Event.
//============================================================================

void CdSuNhTcpMiCfgRspEvent::operator = 
			(const CdSuNhTcpMiCfgRspEvent& cfgRspEvent)
{
	nhTcpInstId = cfgRspEvent.nhTcpInstId;
	smActorId = cfgRspEvent.smActorId;
	opcode = cfgRspEvent.opcode;
	rspCode = cfgRspEvent.rspCode;
	failCode = cfgRspEvent.failCode;
	subOpcode = cfgRspEvent.subOpcode;

} //CdSuNhTcpMiCfgRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiStatusRspEvent::init ()
// Description	: To be filled for Status Response Event.
//============================================================================
void CdSuNhTcpMiStatusRspEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc,
				CdSuNhTcpState state)
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	rspCode = rspc;
	failCode = failc;
	nhState = state;

} // void CdSuNhTcpMiStatusRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiStatusRspEvent::= ()
// Description	: Overload Equal To Operator for Config Response Event.
//============================================================================

void CdSuNhTcpMiStatusRspEvent::operator = 
			(const CdSuNhTcpMiStatusRspEvent& statusRspEvent)
{
	nhTcpInstId = statusRspEvent.nhTcpInstId;
	smActorId = statusRspEvent.smActorId;
	opcode = statusRspEvent.opcode;
	rspCode = statusRspEvent.rspCode;
	failCode = statusRspEvent.failCode;
	nhState = statusRspEvent.nhState;

} //CdSuNhTcpMiStatusRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiUStaIndEvent::init ()
// Description	: To be filled for Unsolicited Status Indication Event.
//============================================================================
void CdSuNhTcpMiUStaIndEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpState state)
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	nhState = state;

} // void CdSuNhTcpMiUStaRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiUStaRspEvent::= ()
// Description	: Overload Equal To Operator for Config Response Event.
//============================================================================

void CdSuNhTcpMiUStaIndEvent::operator = 
			(const CdSuNhTcpMiUStaIndEvent& ustaIndEvent)
{
	nhTcpInstId = ustaIndEvent.nhTcpInstId;
	smActorId = ustaIndEvent.smActorId;
	opcode = ustaIndEvent.opcode;
	nhState = ustaIndEvent.nhState;

} //CdSuNhTcpMiUStaIndEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpMiStsRspEvent::init ()
// Description	: To be filled for Statistics Response.
//============================================================================
void CdSuNhTcpMiStsRspEvent::init (CdSuNhTcpInstId nhid, 
				CdSuActorId smid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc,
				Ulong* ctrs)
				
{
	((CdSuNhTcpMiEvent*) this)->init (nhid, smid, opc);
	rspCode = rspc;
	failCode = failc;
	if (ctrs != NULL)
	{
		for (int i = 0 ; i < CDSU_NHTCP_MAX_CTR; ++i)
		{
			counters [i] = ctrs [i];
		}
	}
	

} // void CdSuNhTcpMiStsRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpMiStsRspEvent::= ()
// Description	: Overload Equal To Operator for Config Response Event.
//============================================================================

void CdSuNhTcpMiStsRspEvent::operator = 
			(const CdSuNhTcpMiStsRspEvent& stsRspEvent)
{
	nhTcpInstId = stsRspEvent.nhTcpInstId;
	smActorId = stsRspEvent.smActorId;
	opcode = stsRspEvent.opcode;
	rspCode = stsRspEvent.rspCode;
	failCode = stsRspEvent.failCode;

	for (int i = 0; i < CDSU_NHTCP_MAX_CTR; ++i)
	{
		counters [i] = stsRspEvent.counters [i];
	}

} //CdSuNhTcpMiStsRspEvent::operator = ()

//============================================================================
// Function Declaration of Parameters in Application->Network Handler Interface.
//============================================================================

//============================================================================
// Function	: CdSuNhTcpUiAddLstnReqParam::init ()
// Description	: Initializes the parameters required for Adding Listeners.
//============================================================================

void CdSuNhTcpUiAddLstnReqParam::init (CdSuNhTcpInstId nhid, 
					CdSuActorId aid,
					CdSuNhTcpEventHlr lstnEvHlr,
					CdSuNhTcpEventHlr conEvHlr,
					CdSuIpv4Addr ipa,
					CdSuIpv4TcpPortNo portno,
					bool tp)
{
	nhTcpInstId = nhid;
	applActorId = aid;
	listenEventHlr = lstnEvHlr;
	conEventHlr = conEvHlr;
	ipv4addr = ipa;
	tcpPortNo = portno;
	tpkt = tp;

} // void CdSuNhTcpUiAddLstnReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpUiAddLstnReqParam::= ()
// Description	: Overload Equal To Operator for AddLstnReqParam.
//============================================================================

void CdSuNhTcpUiAddLstnReqParam::operator = 
		(const CdSuNhTcpUiAddLstnReqParam& addLstnReqParam)
{
	nhTcpInstId = addLstnReqParam.nhTcpInstId;
	applActorId = addLstnReqParam.applActorId;
	listenEventHlr = addLstnReqParam.listenEventHlr;
	conEventHlr = addLstnReqParam.conEventHlr;
	ipv4addr = addLstnReqParam.ipv4addr;
	tcpPortNo = addLstnReqParam.tcpPortNo;
	tpkt = addLstnReqParam.tpkt;

} //CdSuNhTcpMiAddLstnReqParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiRemLstnReqParam::init ()
// Description	: Initializes the parameters required for Removing Listeners.
//============================================================================

void CdSuNhTcpUiRemLstnReqParam::init (CdSuNhTcpLstnKey lkey, 
					CdSuActorId aid,
					CdSuNhTcpEventHlr lEvtHlr)
{
	lstnKey = lkey;
	applActorId = aid;
	listenEventHlr = lEvtHlr;

} // void CdSuNhTcpUiRemLstnReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpUiRemLstnReqParam::= ()
// Description	: Overload Equal To Operator for RemLstnReqParam.
//============================================================================

void CdSuNhTcpUiRemLstnReqParam::operator = 
		(const CdSuNhTcpUiRemLstnReqParam& remLstnReqParam)
{
	lstnKey = remLstnReqParam.lstnKey;
	applActorId = remLstnReqParam.applActorId;
	listenEventHlr = remLstnReqParam.listenEventHlr;

} //CdSuNhTcpMiRemLstnReqParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiConReqParam::init ()
// Description	: Initializes the parameters required for Opening Connections.
//============================================================================

void CdSuNhTcpUiConReqParam::init (CdSuNhTcpInstId nhid, 
					CdSuActorId aid,
					CdSuNhTcpEventHlr conEvtHlr,
					CdSuIpv4Addr ipa,
					CdSuIpv4TcpPortNo portno,
					bool tp)
{
	nhTcpInstId = nhid;
	applActorId = aid;
	conEventHlr = conEvtHlr,
	dstIpv4Addr = ipa;
	dstPortNo = portno;
	tpkt = tp;

} // void CdSuNhTcpUiConReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpUiConReqParam::= ()
// Description	: Overload Equal To Operator for ConReqParam.
//============================================================================

void CdSuNhTcpUiConReqParam::operator = 
		(const CdSuNhTcpUiConReqParam& conReqParam)
{
	nhTcpInstId = conReqParam.nhTcpInstId;
	applActorId = conReqParam.applActorId;
	conEventHlr = conReqParam.conEventHlr,
	dstIpv4Addr = conReqParam.dstIpv4Addr;
	dstPortNo = conReqParam.dstPortNo;
	tpkt = conReqParam.tpkt;

} //CdSuNhTcpMiConReqParam::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiCloseConReqParam::init ()
// Description	: Initializes the parameters required for Closing Connections.
//============================================================================

void CdSuNhTcpUiCloseConReqParam::init (CdSuNhTcpConnKey ckey, 
					CdSuActorId aid,
					CdSuNhTcpCloseConReason reason)
{
	connKey = ckey,
	applActorId = aid;
	closeConReason = reason;
	conEventHlr = NULL;

} // void CdSuNhTcpUiCloseConReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpUiCloseConReqParam::= ()
// Description	: Overload Equal To Operator for CloseConReqParam.
//============================================================================

void CdSuNhTcpUiCloseConReqParam::operator = 
		(const CdSuNhTcpUiCloseConReqParam& closeConReqParam)
{
	connKey = closeConReqParam.connKey,
	applActorId = closeConReqParam.applActorId;
	closeConReason = closeConReqParam.closeConReason;
	conEventHlr = closeConReqParam.conEventHlr;

} //CdSuNhTcpMiCloseConReqParam::operator = ()


//============================================================================
// Function	: CdSuNhTcpUiDataReqParam::CdSuNhTcpUiDataReqParam ()
// Description	: Constructor
//============================================================================

CdSuNhTcpUiDataReqParam::CdSuNhTcpUiDataReqParam ()
{
	dataPtr = NULL;
	dataLen = 0;

} // CdSuNhTcpUiDataReqParam::CdSuNhTcpUiDataReqParam ()

//============================================================================
// Function	: CdSuNhTcpUiDataReqParam::~CdSuNhTcpUiDataReqParam ()
// Description	: Destructor
//============================================================================

CdSuNhTcpUiDataReqParam::~CdSuNhTcpUiDataReqParam ()
{
	// Nothing
} // CdSuNhTcpUiDataReqParam::~CdSuNhTcpUiDataReqParam ()


//============================================================================
// Function	: CdSuNhTcpUiDataReqParam::init ()
// Description	: Initializes the parameters required for send Data Requests.
//============================================================================

void CdSuNhTcpUiDataReqParam::init (CdSuNhTcpConnKey ckey, 
					CdSuActorId aid,
					CdSuNhTcpEventHlr conEvtHlr,
					char* txBuf,
					Uint16 txBufLen)
{
	connKey = ckey,
	applActorId = aid;
	conEventHlr = conEvtHlr;
	dataLen = txBufLen;
	dataPtr = txBuf;
} // void CdSuNhTcpUiDataReqParam::init ()

//============================================================================
// Function	: CdSuNhTcpUiDataReqParam::= ()
// Description	: Overload Equal To Operator for DataReqParam.
//============================================================================

void CdSuNhTcpUiDataReqParam::operator = 
		(const CdSuNhTcpUiDataReqParam& dataReqParam)
{
	connKey = dataReqParam.connKey,
	applActorId = dataReqParam.applActorId;
	conEventHlr = dataReqParam.conEventHlr;
	dataPtr = new char [dataReqParam.dataLen];
	memcpy (dataPtr, dataReqParam.dataPtr, dataReqParam.dataLen);
	dataLen = dataReqParam.dataLen;
} //CdSuNhTcpMiDataReqParam::operator = ()

//============================================================================
// Function Declaration of Events in the NH->UI Interface. 
//============================================================================

//============================================================================
// Function	: CdSuNhTcpUiLstnEvent::init ()
// Description	: Initializes the parameters of the base class UiLstnEvent.
//============================================================================

void CdSuNhTcpUiLstnEvent::init (CdSuNhTcpLstnKey lkey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc)
{
	lstnKey = lkey;
	applActorId = aid;
	opcode = opc;

} // CdSuNhTcpUiLstnEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiConnEvent::init ()
// Description	: Initializes the parameters of the base class UiConnEvent.
//============================================================================

void CdSuNhTcpUiConnEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc)
{
	connKey = ckey;
	applActorId = aid;
	opcode = opc;

} // CdSuNhTcpUiConnEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiAddLstnRspEvent::init ()
// Description	: To be filled for AddLstn Response Event.
//============================================================================
void CdSuNhTcpUiAddLstnRspEvent::init (CdSuNhTcpLstnKey lkey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc)
{
	((CdSuNhTcpUiLstnEvent*) this)->init (lkey, aid, opc);
	rspCode = rspc;
	failCode = failc;

} // CdSuNhTcpUiAddLstnRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiAddLstnRspEvent::= ()
// Description	: Overload Equal To Operator for AddLstn Response Event.
//============================================================================

void CdSuNhTcpUiAddLstnRspEvent::operator = 
			(const CdSuNhTcpUiAddLstnRspEvent& addLstnRspEvent)
{
	lstnKey = addLstnRspEvent.lstnKey;
	applActorId = addLstnRspEvent.applActorId;
	opcode = addLstnRspEvent.opcode;
	rspCode = addLstnRspEvent.rspCode;
	failCode = addLstnRspEvent.failCode;

} // CdSuNhTcpUiAddLstnRspEvent::operator = ()


//============================================================================
// Function	: CdSuNhTcpUiRemLstnRspEvent::init ()
// Description	: To be filled for RemLstn Response Event.
//============================================================================
void CdSuNhTcpUiRemLstnRspEvent::init (CdSuNhTcpLstnKey lkey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
			        CdSuNhTcpFailCode failc)

{
	((CdSuNhTcpUiLstnEvent*) this)->init (lkey, aid, opc);
	rspCode = rspc;
	failCode = failc;

} // CdSuNhTcpUiRemLstnRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiRemLstnRspEvent::= ()
// Description	: Overload Equal To Operator for RemLstn Response Event.
//============================================================================

void CdSuNhTcpUiRemLstnRspEvent::operator = 
			(const CdSuNhTcpUiRemLstnRspEvent& remLstnRspEvent)
{
	lstnKey = remLstnRspEvent.lstnKey;
	applActorId = remLstnRspEvent.applActorId;
	opcode = remLstnRspEvent.opcode;
	rspCode = remLstnRspEvent.rspCode;
	failCode = remLstnRspEvent.failCode;

} // CdSuNhTcpUiRemLstnRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiConRspEvent::init ()
// Description	: To be filled for Con Response Event.
//============================================================================
void CdSuNhTcpUiConRspEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc)
{
	((CdSuNhTcpUiConnEvent*) this)->init (ckey, aid, opc);
	rspCode = rspc;
	failCode = failc;

} // CdSuNhTcpUiConRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiConRspEvent::= ()
// Description	: Overload Equal To Operator for Con Response Event.
//============================================================================

void CdSuNhTcpUiConRspEvent::operator = 
			(const CdSuNhTcpUiConRspEvent& conRspEvent)
{
	connKey = conRspEvent.connKey;
	applActorId = conRspEvent.applActorId;
	opcode = conRspEvent.opcode;
	rspCode = conRspEvent.rspCode;
	failCode = conRspEvent.failCode;

} // CdSuNhTcpUiConRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiCloseConRspEvent::init ()
// Description	: To be filled for CloseCon Response Event.
//============================================================================
void CdSuNhTcpUiCloseConRspEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc)

{
	((CdSuNhTcpUiConnEvent*) this)->init (ckey, aid, opc);
	rspCode = rspc;
	failCode = failc;

} // CdSuNhTcpUiCloseConRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiCloseConRspEvent::= ()
// Description	: Overload Equal To Operator for Close Con Response Event.
//============================================================================

void CdSuNhTcpUiCloseConRspEvent::operator = 
			(const CdSuNhTcpUiCloseConRspEvent& closeConRspEvent)
{
	connKey = closeConRspEvent.connKey;
	applActorId = closeConRspEvent.applActorId;
	opcode = closeConRspEvent.opcode;
	rspCode = closeConRspEvent.rspCode;
	failCode = closeConRspEvent.failCode;

} // CdSuNhTcpUiCloseConRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiDataRspEvent::init ()
// Description	: To be filled for Data Response Event.
//============================================================================
void CdSuNhTcpUiDataRspEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpRspCode rspc,
				CdSuNhTcpFailCode failc)
{
	((CdSuNhTcpUiConnEvent*) this)->init (ckey, aid, opc);
	rspCode = rspc;
	failCode = failc;

} // CdSuNhTcpUiDataRspEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiDataRspEvent::= ()
// Description	: Overload Equal To Operator for Close Con Response Event.
//============================================================================

void CdSuNhTcpUiDataRspEvent::operator = 
			(const CdSuNhTcpUiDataRspEvent& dataRspEvent)
{
	connKey = dataRspEvent.connKey;
	applActorId = dataRspEvent.applActorId;
	opcode = dataRspEvent.opcode;
	rspCode = dataRspEvent.rspCode;
	failCode = dataRspEvent.failCode;

} // CdSuNhTcpUiDataRspEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiDataIndEvent::CdSuNhTcpUiDataIndEvent ()
// Description	: Constructor
//============================================================================

CdSuNhTcpUiDataIndEvent::CdSuNhTcpUiDataIndEvent ()
{
	dataPtr = NULL;
	dataLen = 0;
	tpkt = false;

} // CdSuNhTcpUiDataIndEvent::CdSuNhTcpUiDataIndEvent ()

//============================================================================
// Function	: CdSuNhTcpUiDataIndEvent::~CdSuNhTcpUiDataIndEvent ()
// Description	: Destructor.
//============================================================================

CdSuNhTcpUiDataIndEvent::~CdSuNhTcpUiDataIndEvent ()
{
	if (dataPtr != NULL)
	{
		delete [] dataPtr;
		dataPtr = NULL;
	}

} // CdSuNhTcpUiDataIndEvent::~CdSuNhTcpUiDataIndEvent ()


//============================================================================
// Function	: CdSuNhTcpUiDataIndEvent::init ()
// Description	: To be filled for Data Indication Event.
//===========================================================================
void CdSuNhTcpUiDataIndEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				char* rxBuf,
				Uint16 rxBufLen,
				bool tp)
{
	((CdSuNhTcpUiConnEvent*) this)->init (ckey, aid, opc);
	dataLen = rxBufLen;
	dataPtr = new char [dataLen];
	memcpy (dataPtr, rxBuf, dataLen);
	tpkt = tp;

} // CdSuNhTcpUiDataIndEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiDataIndEvent::= ()
// Description	: Overload Equal To Operator for Data Indiacation Response Event.
//============================================================================

void CdSuNhTcpUiDataIndEvent::operator = 
			(const CdSuNhTcpUiDataIndEvent& dataIndEvent)
{
	connKey = dataIndEvent.connKey;
	applActorId = dataIndEvent.applActorId;
	opcode = dataIndEvent.opcode;
	dataLen = dataIndEvent.dataLen;
	dataPtr = new char [dataLen];
	memcpy (dataPtr, dataIndEvent.dataPtr, dataLen);
	tpkt = dataIndEvent.tpkt;

} // CdSuNhTcpUiDataIndEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiConIndEvent::init ()
// Description	: To be filled for Connection Indication Event.
//============================================================================
void CdSuNhTcpUiConIndEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc,
				CdSuNhTcpLstnKey lkey,
				bool tp,
				CdSuIpv4Addr dstIpAddr,
				CdSuIpv4TcpPortNo dstPort)
{
	((CdSuNhTcpUiLstnEvent*) this)->init (lkey, aid, opc);
	connKey = ckey;
	tpkt = tp;
	dstIpv4Addr = dstIpAddr;
	dstPortNo = dstPort;

} // CdSuNhTcpUiConIndEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiConIndEvent::= ()
// Description	: Overload Equal To Operator for Connection Indication Event.
//============================================================================

void CdSuNhTcpUiConIndEvent::operator = 
			(const CdSuNhTcpUiConIndEvent& conIndEvent)
{
	connKey = conIndEvent.connKey;
	applActorId = conIndEvent.applActorId;
	opcode = conIndEvent.opcode;
	lstnKey = conIndEvent.lstnKey;
	dstIpv4Addr = conIndEvent.dstIpv4Addr;
	dstPortNo = conIndEvent.dstPortNo;

} // CdSuNhTcpUiConIndEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpUiCloseConIndEvent::init ()
// Description	: To be filled for Close Connection Indication Event.
//============================================================================
void CdSuNhTcpUiCloseConIndEvent::init (CdSuNhTcpConnKey ckey, 
				CdSuActorId aid,
				CdSuNhTcpOpcode opc)
{
	((CdSuNhTcpUiConnEvent*) this)->init (ckey, aid, opc);

} // CdSuNhTcpUiCloseConIndEvent::init ()

//============================================================================
// Function	: CdSuNhTcpUiCloseConIndEvent::= ()
// Description	: Overload Equal To Operator for Close Connection Indication Event.
//============================================================================

void CdSuNhTcpUiCloseConIndEvent::operator = 
			(const CdSuNhTcpUiCloseConIndEvent& closeConIndEvent)
{
	connKey = closeConIndEvent.connKey;
	applActorId = closeConIndEvent.applActorId;
	opcode = closeConIndEvent.opcode;

} // CdSuNhTcpUiCloseConIndEvent::operator = ()

//============================================================================
// Function	: CdSuNhTcpConnKey::init ()
// Description	: To be filled for CdSuNhTcpConnKey.
//============================================================================
void CdSuNhTcpConnKey::init (CdSuNhTcpConnId cid, CdSuNhTcpInstId nhid)
{
	connId = cid;
	nhTcpInstId = nhid;

} // CdSuNhTcpConnKey::init ()

//============================================================================
// Function	: CdSuNhTcpConnKey::= ()
// Description	: Overload Equal To Operator for Connection key.
//============================================================================

void CdSuNhTcpConnKey::operator = 
			(const CdSuNhTcpConnKey& ck)
{
	nhTcpInstId = ck.nhTcpInstId;
	connId = ck.connId;

} // CdSuNhTcpConnKey::operator = ()


//============================================================================
// Function	: CdSuNhTcpLstnKey::init ()
// Description	: To be filled for CdSuNhTcpLstnKey. 
//============================================================================
void CdSuNhTcpLstnKey::init (CdSuNhTcpLstnId cid, CdSuNhTcpInstId nhid)
{
	lstnId = cid;
	nhTcpInstId = nhid;

} // CdSuNhTcpLstnKey::init ()

//============================================================================
// Function	: CdSuNhTcpLstnKey::= ()
// Description	: Overload Equal To Operator for Listen key.
//============================================================================

void CdSuNhTcpLstnKey::operator = 
			(const CdSuNhTcpLstnKey& ck)
{
	lstnId = ck.lstnId;
	nhTcpInstId = ck.nhTcpInstId;

} // CdSuNhTcpLstnKey::operator = ()

//----------------------------------------------------------------------------
//	<End of file>
//----------------------------------------------------------------------------

