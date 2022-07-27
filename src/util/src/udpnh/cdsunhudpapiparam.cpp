//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpparam.cpp
// Description	: This contains the definitions of the functions of the
// 		  various parameters and events used to fill the messages.
// Author       : Vijaya Kumar
// Created On   : Mon Mar  8 21:42:24 IST 2004
// Last Modified: 1. Mon Aug 16 10:49:38 IST 2004
// 			Bugs bs/25 is fixed
// 		  2. Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

//---------------------------------------------------------------------------
// Include files.
//---------------------------------------------------------------------------

#include "string.h"
#include "cdsunhudpapiparam.h"

//---------------------------------------------------------------------------
// 	Definitions for Method of Parameters of   SM->NH Interface.
// 	-------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: Constructor of CdSuNhUdpMiInitParam 
// Struct	: CdSuNhUdpMiInitParam
// Description  : Constructor 
// Inputs       : Nothing
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpMiInitParam:: CdSuNhUdpMiInitParam ()
{
	maxNoOfUdpPort = CDSU_NHUDP_MAX_UDPPORT; 
	maxBufferLen = CDSU_NHUDP_DEF_BUFLEN;
	dataSelectTimer = CDSU_NHUDP_DEF_DATA_SELTMR;
	noHandleTimer = CDSU_NHUDP_DEF_NOHDL_TMR;
	smEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR;
} // CdSuNhUdpMiInitParam:: CdSuNhUdpMiInitParam ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiInitParam
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiInitParam:: init (CdSuActorId nhActorId,
			CdSuActorId smActorId,
			Uint32 maxUdpPort,
		       	Uint16 maxBufferLen,
		       	Uint32 dataSelectTimer,
		       	Uint32 noHandleTimer,
			CdSuNhUdpEventHlr smEventHlr)
{
	this->nhActorId = nhActorId;
	this->smActorId = smActorId;
	this->maxNoOfUdpPort = maxUdpPort; 
	this->maxBufferLen = maxBufferLen;
	this->dataSelectTimer = dataSelectTimer;
	this->noHandleTimer = noHandleTimer;
	this->smEventHlr = smEventHlr;
	
} // CdSuNhUdpMiInitParam:: init ()

//---------------------------------------------------------------------------
// 	Definitions for Method of Parameters of  NH->SM Interface.
// 	-------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiEvent::init  (CdSuNhUdpInstId nhInstId, 
				CdSuActorId smActorId,
				CdSuNhUdpOpcode opcode)
{
	this->nhInstId = nhInstId;
	this->smActorId = smActorId;
	this->opcode = opcode;
	
} // void CdSuNhUdpMiEvent::init ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiInitRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiInitRspEvent::init (CdSuNhUdpInstId nhInstId,
					CdSuActorId smActorId,
					CdSuNhUdpOpcode opcode,
					CdSuNhUdpRspCode rspCode)
{
	( (CdSuNhUdpMiEvent*) this)->init (nhInstId, smActorId,	opcode);
	this->rspCode = rspCode;
	
} // void CdSuNhUdpMiInitRspEvent:: init ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiDelNhRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiDelNhRspEvent::init (CdSuNhUdpInstId nhInstId,
						CdSuActorId smActorId)
{
	( (CdSuNhUdpMiEvent*) this)->init (nhInstId, smActorId,	CDSU_NHUDP_MI_DELNH_RSP);
	this->rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;
	
} // void CdSuNhUdpMiDelNhRspEvent:: init ()
 
//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiDelNhRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiDelNhRspEvent::init (CdSuNhUdpInstId nhInstId,
					CdSuActorId smActorId,
					CdSuNhUdpRspFailCode failCode)
{
	( (CdSuNhUdpMiEvent*) this)->init (nhInstId, smActorId, CDSU_NHUDP_MI_DELNH_RSP);
	this->rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	this->failCode = failCode;
	
} // void CdSuNhUdpMiDelNhRspEvent:: init ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiCtrlRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiCtrlRspEvent::init (CdSuNhUdpInstId nhid, 
				CdSuActorId smid,
				CdSuNhUdpRspCode rspc,
				CdSuNhUdpSubOpcode subopc)
{
	((CdSuNhUdpMiEvent*) this)->init (nhid, smid, CDSU_NHUDP_MI_CTRL_RSP);
	rspCode = rspc;
	subOpcode = subopc;

} // void CdSuNhUdpMiCtrlRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiStatusRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiStatusRspEvent::init (CdSuNhUdpInstId nhid, 
				CdSuActorId smid,
				CdSuNhUdpRspCode rspc,
				CdSuNhUdpState state)
{
	((CdSuNhUdpMiEvent*) this)->init (nhid, smid, CDSU_NHUDP_MI_STATUS_RSP);
	rspCode = rspc;
	nhState = state;
	
} // void CdSuNhUdpMiStatusRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiStsRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiStsRspEvent::init (CdSuNhUdpInstId nhid, 
				CdSuActorId smid,
				const Ulong* ctrs)
				
{
	((CdSuNhUdpMiEvent*) this)->init (nhid, smid, CDSU_NHUDP_MI_STATISTICS_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;
	for (int i = 0 ; i < CDSU_NHUDP_MAX_CTR; ++i)
	{
		counters [i] = ctrs [i];
	}
	
} // void CdSuNhUdpMiStsRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpMiStsRspEvent
// Description  : initializing parameters. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiStsRspEvent::init (CdSuNhUdpInstId nhid, 
				CdSuActorId smid,
				CdSuNhUdpRspFailCode failCode)
				
{
	((CdSuNhUdpMiEvent*) this)->init (nhid, smid, CDSU_NHUDP_MI_STATISTICS_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	this->failCode = failCode;
	
} // void CdSuNhUdpMiStsRspEvent::init ()

/****************************************************************************/

//---------------------------------------------------------------------------
// 	Definitions for Method of Parameters of User->NH Interface.
// 	------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: Constructor 
// Struct 	: CdSuNhUdpUiAddUdpPortReqParam
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpUiAddUdpPortReqParam::CdSuNhUdpUiAddUdpPortReqParam ()
{
	localPortNo = CDSU_NHUDP_INV_UDPPORT;	
	destPortNo = CDSU_NHUDP_INV_UDPPORT;
	applEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR;
	
} // CdSuNhUdpUiAddUdpPortReqParam::CdSuNhUdpUiAddUdpPortReqParam ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpUiAddUdpPortReqParam
// Description  : initializing
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiAddUdpPortReqParam::init (CdSuNhUdpInstId nhUdpInstId,
						CdSuActorId applActorId,
						CdSuIpv4UdpPortNo localPortNo,
						CdSuIpv4Addr localAddr, 
						CdSuNhUdpEventHlr applEventHlr,
						CdSuIpv4UdpPortNo destPortNo,
						CdSuIpv4Addr destAddr)
{
	this->nhUdpInstId = nhUdpInstId;
	this->applActorId = applActorId;
	this->localPortNo = localPortNo;	
	this->localAddr = localAddr;
	this->destPortNo = destPortNo;
	this->destAddr = destAddr;
	this->applEventHlr = applEventHlr;
	
} // CdSuNhUdpUiAddUdpPortReqParam::CdSuNhUdpUiAddUdpPortReqParam ()

//---------------------------------------------------------------------------
// Method	: Constructor 
// Struct 	: CdSuNhUdpUiRemUdpPortReqParam
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpUiRemUdpPortReqParam::CdSuNhUdpUiRemUdpPortReqParam ()
{
	udpPortKey.nhUdpInstId = CDSU_NHUDP_INV_NHINSTID;
	udpPortKey.nhUdpPortId = CDSU_NHUDP_INV_UDPPORTID;
	applEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR;
	
} // CdSuNhUdpUiRemUdpPortReqParam::CdSuNhUdpUiRemUdpPortReqParam ()

//---------------------------------------------------------------------------
// Method	: Constructor 
// Struct 	: CdSuNhUdpUiRemUdpPortReqParam
// Description  : initializing
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiRemUdpPortReqParam::init (CdSuNhUdpPortKey udpPortKey,
					CdSuActorId applActorId,
					CdSuNhUdpEventHlr applEventHlr)
{
	this->udpPortKey = udpPortKey;
	this->applActorId = applActorId;
	this->applEventHlr = applEventHlr;
	
} // CdSuNhUdpUiRemUdpPortReqParam::init ()

//---------------------------------------------------------------------------
// Method	: Constructor 
// Struct 	: CdSuNhUdpUiDataReqParam
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpUiDataReqParam::CdSuNhUdpUiDataReqParam ()
{
	dataPtr = NULL;
	destPortNo = CDSU_NHUDP_INV_UDPPORT;
	isAckReq = true;
	applEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR;
	
} // CdSuNhUdpUiDataReqParam::CdSuNhUdpUiDataReqParam ()

//---------------------------------------------------------------------------
// Method	: init () 
// Struct 	: CdSuNhUdpUiDataReqParam
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataReqParam::init (CdSuNhUdpPortKey udpPortKey,
			CdSuActorId applActorId,
			char* dataPtr,
			Uint16 dataLen,
			Uint32 opaqueData,
			bool isAckReq,
			CdSuNhUdpEventHlr applEventHlr,
			CdSuIpv4UdpPortNo destPortNo,
			CdSuIpv4Addr destAddr)
{
	this->udpPortKey = udpPortKey;
	this->applActorId = applActorId;
	this->dataPtr = dataPtr;
	this->dataLen = dataLen;
	this->opaqueData = opaqueData;
	this->isAckReq = isAckReq;
	this->destPortNo = destPortNo;
	this->destAddr = destAddr;
	this->applEventHlr = applEventHlr;
	
} // CdSuNhUdpUiDataReqParam::CdSuNhUdpUiDataReqParam ()

//---------------------------------------------------------------------------
// Method	: Overloading operator = 
// Struct 	: CdSuNhUdpUiDataReqParam
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataReqParam::operator = (const CdSuNhUdpUiDataReqParam& p)
{
	udpPortKey = p.udpPortKey;
	applActorId = p.applActorId;
	dataPtr = new char [p.dataLen];
	memcpy (dataPtr, p.dataPtr, p.dataLen);
	dataLen = p.dataLen;
	opaqueData = p.opaqueData;
	destPortNo = p.destPortNo;	
	destAddr = p.destAddr;		
	isAckReq = p.isAckReq;
	applEventHlr = p.applEventHlr; 
	
} // void CdSuNhUdpUiDataReqParam::operator = ( )

//---------------------------------------------------------------------------
// Method	: Constructor.
// Struct 	: CdSuNhUdpUiSendReqParam
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpUiSendReqParam::CdSuNhUdpUiSendReqParam ()
{
	dataPtr = NULL;
	destPortNo = CDSU_NHUDP_INV_UDPPORT;
	applEventHlr = (void (*)(void *))CDSU_NHUDP_INV_UDPEVENTHLR;
	
} // CdSuNhUdpUiSendReqParam::CdSuNhUdpUiSendReqParam ()

//---------------------------------------------------------------------------
// Method	: Overloading operator = 
// Struct 	: CdSuNhUdpUiSendReqParam
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiSendReqParam::operator = (const CdSuNhUdpUiSendReqParam& p)
{
	nhUdpInstId = p.nhUdpInstId;
	applActorId = p.applActorId;
	dataPtr = new char [p.dataLen];
	memcpy (dataPtr, p.dataPtr, p.dataLen);
	dataLen = p.dataLen;
	opaqueData = p.opaqueData;
	destPortNo = p.destPortNo;	
	destAddr = p.destAddr;		
	applEventHlr = p.applEventHlr; 

} // void CdSuNhUdpUiSendReqParam::operator = ( )

//---------------------------------------------------------------------------
// 	Definitions for Method of Parameters of   NH->SM Interface.
// 	-------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				CdSuNhUdpOpcode opcode)
{
	this->nhUdpPortKey = nhUdpPortKey;
	this->applActorId = applActorId;
	this->opcode = opcode;

} // CdSuNhUdpUiEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiEvent::init (CdSuNhUdpInstId nhUdpInstId, 
				CdSuActorId applActorId,
				CdSuNhUdpOpcode opcode)
{
	// this->nhUdpInstId = nhUdpInstId;
	this->applActorId = applActorId;
	this->opcode = opcode;

} // CdSuNhUdpUiEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiAddUdpPortRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiAddUdpPortRspEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				CdSuIpv4UdpPortNo nhUdpPort)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_ADD_UDPPORT_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;
	localPortNo =  nhUdpPort;

} // CdSuNhUdpUiAddUdpPortRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiAddUdpPortRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiAddUdpPortRspEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				CdSuIpv4UdpPortNo nhUdpPort,
				CdSuNhUdpRspFailCode failCode)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_ADD_UDPPORT_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	localPortNo =  nhUdpPort;
	this->failCode = failCode;
	
} // CdSuNhUdpUiAddUdpPortRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiRemUdpPortRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiRemUdpPortRspEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_REM_UDPPORT_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;

} // CdSuNhUdpUiRemUdpPortRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiRemUdpPortRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiRemUdpPortRspEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				CdSuNhUdpRspFailCode failCode)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_REM_UDPPORT_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	this->failCode = failCode;
	
} // CdSuNhUdpUiRemUdpPortRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiSendRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiSendRspEvent::init (CdSuNhUdpInstId nhUdpInstId, 
				CdSuActorId applActorId,
				Uint32 opaqueData)
{
	//((CdSuNhUdpUiEvent*) this)->init (nhUdpInstId, applActorId, CDSU_NHUDP_UI_SEND_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;
	this->opaqueData = opaqueData;
	
} // CdSuNhUdpUiDataRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiSendRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiSendRspEvent::init (CdSuNhUdpInstId nhUdpInstId, 
				CdSuActorId applActorId,
				Uint32 opaqueData,
				CdSuNhUdpRspFailCode failCode)
{
	//((CdSuNhUdpUiEvent*) this)->init (nhUdpInstId, applActorId, CDSU_NHUDP_UI_SEND_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	this->failCode = failCode;
	this->opaqueData = opaqueData;
	
} // CdSuNhUdpUiSendRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiDataRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataRspEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				Uint32 opaqueData)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_DATA_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;
	this->opaqueData = opaqueData;
	
} // CdSuNhUdpUiDataRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiDataRspEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataRspEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				Uint32 opaqueData,
				CdSuNhUdpRspFailCode failCode)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_DATA_RSP);
	rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	this->failCode = failCode;
	this->opaqueData = opaqueData;
	
} // CdSuNhUdpUiDataRspEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiDataIndEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataIndEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				char* dataPtr,
				Uint16 dataLen,
				CdSuIpv4UdpPortNo localPortNo,
				CdSuIpv4Addr localAddr,
				CdSuIpv4UdpPortNo destPortNo,  
				CdSuIpv4Addr destAddr)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_DATA_IND);
	
	if (dataLen > 0)
	{
		this->dataPtr = new char [dataLen];
	} else
	{
		this->dataPtr = NULL;
	}
	if (this->dataPtr != NULL)
	{
		memcpy (this->dataPtr, dataPtr, dataLen);
	}
	
	this->dataLen = dataLen;
	this->localAddr = localAddr;
	this->localPortNo = localPortNo;
	this->destPortNo = destPortNo;
	this->destAddr = destAddr;
	rspCode = CDSU_NHUDP_RSPCODE_SUCCESS;
	
} // CdSuNhUdpUiDataIndEvent::init ()

//---------------------------------------------------------------------------
// Method	: init ()  
// Struct 	: CdSuNhUdpUiDataIndEvent
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataIndEvent::init (CdSuNhUdpPortKey nhUdpPortKey, 
				CdSuActorId applActorId,
				char* dataPtr,
				Uint16 dataLen,
				CdSuIpv4UdpPortNo localPortNo,
				CdSuIpv4Addr localAddr,
				CdSuIpv4UdpPortNo destPortNo,
				CdSuIpv4Addr destAddr,
				CdSuNhUdpRspFailCode failCode)
{
	((CdSuNhUdpUiEvent*) this)->init (nhUdpPortKey, applActorId, CDSU_NHUDP_UI_DATA_IND);
	
	if (dataLen > 0)
	{
		this->dataPtr = new char [dataLen];
	} else
	{
		this->dataPtr = NULL;
	}
	
	if (this->dataPtr != NULL)
	{
		memcpy (this->dataPtr, dataPtr, dataLen);
	}

	this->dataLen = dataLen;
	this->localAddr = localAddr;
	this->localPortNo = localPortNo;
	this->destPortNo = destPortNo;
	this->destAddr = destAddr;	
	rspCode = CDSU_NHUDP_RSPCODE_FAIL;
	this->failCode = failCode;

} // CdSuNhUdpUiDataIndEvent::init ()

//============================================================================
//	<End of file cdsunhudpapiparam.cpp>
//============================================================================

