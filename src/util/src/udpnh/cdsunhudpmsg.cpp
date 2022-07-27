//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtudmsg.cpp
// Description  : Definition of cdsunhtcpmsg.cpp
// Author       : Vijaya Kumar 
// Created On   : Tue Mar  9 20:24:17 IST 2004
// Last Modified:
// Notes	: 
//			*) Message structure definitions. 
//============================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include "cdsuutiltypes.h"
#include "cdsunhudpmsg.h"

//---------------------------------------------------------------------------
// 	SM->NH Interface Messages. These messages are created and sent by
// 	the functions defined in the API.
// 	------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiDelNhReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiDelNhReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiDelNhParam& delNhP)
					
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_DELNH_REQ,
				      sizeof (CdSuNhUdpMiDelNhReqMsg));
	delNhParam = delNhP;
	
}; // CdSuNhUdpMiDelNhReqMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiCtrlReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiCtrlReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiCtrlReqParam& ctrlReqP)
					
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_CTRL_REQ,
				sizeof (CdSuNhUdpMiCtrlReqMsg));
	ctrlReqParam = ctrlReqP;

}; // CdSuNhUdpMiDelNhReqMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiCfgReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiCfgReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiCfgReqParam& cfgReqP)
					
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_CFG_REQ,
				sizeof (CdSuNhUdpMiCfgReqMsg));
	cfgReqParam = cfgReqP;
	
}; // CdSuNhUdpMiCfgReqMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiStatusReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiStatusReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiStatusReqParam& statusReqP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_STATUS_REQ,
				sizeof (CdSuNhUdpMiStatusReqMsg));
	statusReqParam = statusReqP;
	
}; // CdSuNhUdpMiCfgReqMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiStsReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiStsReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
                               	const CdSuNhUdpMiStsReqParam& stsReqP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_STATISTICS_REQ,
				sizeof (CdSuNhUdpMiStsReqMsg));
	stsReqParam = stsReqP;

} // CdSuNhUdpMiStsReqMsg::initMsg ()

//---------------------------------------------------------------------------
// 	NH->SM Interface Messages. These messages are sent by the Default
// 	SM Event Handler only if not SM Event Handler is declared by the SM.
// 	-------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiStsReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiInitRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiInitRspEvent& initRspEvP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_INIT_RSP,
				sizeof (CdSuNhUdpMiInitRspMsg));
	initRspEvent = initRspEvP;

} // CdSuNhUdpMiInitRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiDelNhRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiDelNhRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiDelNhRspEvent& delNhRspEvP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_DELNH_RSP,
				sizeof (CdSuNhUdpMiDelNhRspMsg));
	delNhRspEvent = delNhRspEvP;
	
} // CdSuNhUdpMiDelNhRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiCtrlRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiCtrlRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpMiCtrlRspEvent& ctrlRspEvP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_CTRL_RSP,
				sizeof (CdSuNhUdpMiCtrlRspMsg));
	ctrlRspEvent = ctrlRspEvP;
	
} // CdSuNhUdpMiCtrlRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiCfgRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiCfgRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId, 
				const CdSuNhUdpMiCfgRspEvent& cfgRspEvP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_CFG_RSP,
				sizeof (CdSuNhUdpMiCfgRspMsg));
	cfgRspEvent = cfgRspEvP;
	
} // CdSuNhUdpMiCfgRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpMiStatusRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpMiStatusRspMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId, 
				const CdSuNhUdpMiStatusRspEvent& statusRspEvP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_STATUS_RSP,
				sizeof (CdSuNhUdpMiStatusRspMsg));
	statusRspEvent = statusRspEvP;
} // CdSuNhUdpMiStatusRspMsg::initMsg ()

void CdSuNhUdpMiStsRspMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId, 
				const CdSuNhUdpMiStsRspEvent& stsRspEvP)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_MI_STATISTICS_RSP,
				sizeof (CdSuNhUdpMiStsRspMsg));
	stsRspEvent = stsRspEvP;

} // CdSuNhUdpMiStatusRspMsg::initMsg ()


//---------------------------------------------------------------------------
// 	UI->NH Interface Messages. These messages are created and sent by
// 	the functions defined in the API. 
// 	-----------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiAddUdpPortReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiAddUdpPortReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiAddUdpPortReqParam& addUdpPortReqParam,
				const CdSuNhUdpPortId& udpPortId)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHUDP_UI_ADD_UDPPORT_REQ,
				sizeof (CdSuNhUdpUiAddUdpPortReqMsg));
	this->addUdpPortReqParam = addUdpPortReqParam;
	this->udpPortId = udpPortId;

} // CdSuNhUdpUiAddUdpPortReqMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiRemUdpPortReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiRemUdpPortReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiRemUdpPortReqParam& remUdpPortReqParam)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_REM_UDPPORT_REQ,
				sizeof (CdSuNhUdpUiRemUdpPortReqMsg));
	this->remUdpPortReqParam = remUdpPortReqParam;

} // CdSuNhUdpUiAddUdpPortReqMsg::initMsg ()


//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiDataReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiDataReqParam& dataReqParam)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHUDP_UI_DATA_REQ,
				sizeof (CdSuNhUdpUiDataReqMsg));
	this->dataReqParam = dataReqParam;

}; // CdSuNhUdpUiDataReqMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiSendDataReqMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiSendReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiSendReqParam& sendReqParam)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_SEND_REQ,
				sizeof (CdSuNhUdpUiSendReqMsg));
	this->sendReqParam = sendReqParam;

}; // CdSuNhUdpUiSendReqMsg::initMsg ()

//---------------------------------------------------------------------------
//	NH->UI Interface Messages. These messages are created and sent by
// 	the Default Event Handler only if
// 	the corresponding Event Handlers are not declared by the Application.
// 	-------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiAddUdpPortRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiAddUdpPortRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiAddUdpPortRspEvent& addUdpPortRspEvent)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_ADD_UDPPORT_RSP,
				sizeof (CdSuNhUdpUiAddUdpPortRspMsg));
	this->addUdpPortRspEvent = addUdpPortRspEvent;

}; // CdSuNhUdpUiAddUdpPortRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiRemUdpPortRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiRemUdpPortRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiRemUdpPortRspEvent& remUdpPortRspEvent)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_REM_UDPPORT_RSP,
				sizeof (CdSuNhUdpUiRemUdpPortRspMsg));
	this->remUdpPortRspEvent = remUdpPortRspEvent;

}; // CdSuNhUdpUiAddUdpPortRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiDataRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiDataRspEvent& dataRspEvent)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_DATA_RSP,
				sizeof (CdSuNhUdpUiDataRspMsg));
	this->dataRspEvent = dataRspEvent;

}; // CdSuNhUdpUiDataRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiSendRspMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiSendRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiSendRspEvent& sendRspEvent)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_SEND_RSP,
				sizeof (CdSuNhUdpUiSendRspMsg));
	this->sendRspEvent = sendRspEvent;

}; // CdSuNhUdpUiSendRspMsg::initMsg ()

//---------------------------------------------------------------------------
// Method	: initMsg () 
// struct Name	: CdSuNhUdpUiDataIndMsg
// Description  :  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdpUiDataIndMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhUdpUiDataIndEvent& dataIndEvent)
{
	((CdMessage*) this)->initMsg (dstAId, srcAId, CDSU_NHUDP_UI_DATA_IND,
				sizeof (CdSuNhUdpUiDataIndMsg));
	this->dataIndEvent = dataIndEvent;

}; // CdSuNhUdpUiDataIndMsg::initMsg ()

//============================================================================
// < End of cdsunhudpmsg.cpp>
//============================================================================
