//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpmsg.cpp
// Description  : Definition of cdsunhtcpmsg.cpp
// Author       : Seema Jain 
// Created On   : Thu Feb 12 17:11:10 IST 2004
// Last Modified:
// Notes	: 
//			*) Message structure definitions. 
//============================================================================

//============================================================================
// Include Files
//============================================================================
#include "cdsuutiltypes.h"
#include "cdsunhtcpmsg.h"

//============================================================================
// SM->NH Interface Messages. These messages are created and sent by
// the functions defined in the API.
//============================================================================

void CdSuNhTcpMiInitReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiInitParam& initP)
					
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_INIT_REQ,
				sizeof (CdSuNhTcpMiInitReqMsg));
	initParam = initP;

}; // CdSuNhTcpMiInitReqMsg::initMsg ()

void CdSuNhTcpMiDelNhReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiDelNhParam& delNhP)
					
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_DELNH_REQ,
				sizeof (CdSuNhTcpMiDelNhReqMsg));
	delNhParam = delNhP;

}; // CdSuNhTcpMiDelNhReqMsg::initMsg ()

void CdSuNhTcpMiCtrlReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiCtrlReqParam& ctrlReqP)
					
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_CTRL_REQ,
				sizeof (CdSuNhTcpMiCtrlReqMsg));
	ctrlReqParam = ctrlReqP;

}; // CdSuNhTcpMiDelNhReqMsg::initMsg ()

void CdSuNhTcpMiCfgReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiCfgReqParam& cfgReqP)
					
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_CFG_REQ,
				sizeof (CdSuNhTcpMiCfgReqMsg));
	cfgReqParam = cfgReqP;

}; // CdSuNhTcpMiCfgReqMsg::initMsg ()

void CdSuNhTcpMiStatusReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiStatusReqParam& statusReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_STATUS_REQ,
				sizeof (CdSuNhTcpMiStatusReqMsg));
	statusReqParam = statusReqP;

}; // CdSuNhTcpMiCfgReqMsg::initMsg ()

void CdSuNhTcpMiStsReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
                               	const CdSuNhTcpMiStsReqParam& stsReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_STATISTICS_REQ,
				sizeof (CdSuNhTcpMiStsReqMsg));
	stsReqParam = stsReqP;

} // CdSuNhTcpMiStsReqMsg::initMsg ()

//============================================================================
// NH->SM Interface Messages. These messages are sent by the Default
// SM Event Handler only if not SM Event Handler is declared by the SM.
//============================================================================

void CdSuNhTcpMiInitRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiInitRspEvent& initRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_INIT_RSP,
				sizeof (CdSuNhTcpMiInitRspMsg));

	initRspEvent = initRspEv;

} // CdSuNhTcpMiInitRspMsg::initMsg ()

void CdSuNhTcpMiDelNhRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiDelNhRspEvent& delNhRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_DELNH_RSP,
				sizeof (CdSuNhTcpMiDelNhRspMsg));
	delNhRspEvent = delNhRspEv;

} // CdSuNhTcpMiDelNhRspMsg::initMsg ()

void CdSuNhTcpMiCtrlRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiCtrlRspEvent& ctrlRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_CTRL_RSP,
				sizeof (CdSuNhTcpMiCtrlRspMsg));

	ctrlRspEvent = ctrlRspEv;

} // CdSuNhTcpMiCtrlRspMsg::initMsg ()

void CdSuNhTcpMiCfgRspMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId, 
				const CdSuNhTcpMiCfgRspEvent& cfgRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_CFG_RSP,
				sizeof (CdSuNhTcpMiCfgRspMsg));
	cfgRspEvent = cfgRspEv;

} // CdSuNhTcpMiCfgRspMsg::initMsg ()

void CdSuNhTcpMiStatusRspMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId, 
				const CdSuNhTcpMiStatusRspEvent& statusRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_STATUS_RSP,
				sizeof (CdSuNhTcpMiStatusRspMsg));
	statusRspEvent = statusRspEv;

} // CdSuNhTcpMiStatusRspMsg::initMsg ()

void CdSuNhTcpMiStsRspMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId, 
				const CdSuNhTcpMiStsRspEvent& stsRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_STATISTICS_RSP,
				sizeof (CdSuNhTcpMiStsRspMsg));
	stsRspEvent = stsRspEv;

} // CdSuNhTcpMiStatusRspMsg::initMsg ()

void CdSuNhTcpMiUStaIndMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId,
				const CdSuNhTcpMiUStaIndEvent& ustaIndEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_MI_USTATUS_IND,
				sizeof (CdSuNhTcpMiUStaIndMsg));
	ustaIndEvent = ustaIndEv;

} // CdSuNhTcpMiUStaIndMsg::initMsg ()

//============================================================================
// UI->NH Interface Messages. These messages are created and sent by
// the functions defined in the API. 
//============================================================================

void CdSuNhTcpUiAddLstnReqMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId,
				const CdSuNhTcpLstnId lid, 
				const CdSuNhTcpUiAddLstnReqParam& addLstnReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId, 
				CDSU_NHTCP_UI_ADD_LISTENER_REQ,
				sizeof (CdSuNhTcpUiAddLstnReqMsg));
	addLstnReqParam = addLstnReqP;
	lstnId = lid;

} // CdSuNhTcpUiAddLstnReqMsg::initMsg ()

void CdSuNhTcpUiRemLstnReqMsg::initMsg (const CdSuActorId& dstAId, 
				const CdSuActorId& srcAId, 
				const CdSuNhTcpUiRemLstnReqParam& remLstnReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_REM_LISTENER_REQ,
				sizeof (CdSuNhTcpUiRemLstnReqMsg));
	remLstnReqParam = remLstnReqP;

} // CdSuNhTcpUiRemLstnReqMsg::initMsg ()

void CdSuNhTcpUiConReqMsg::initMsg (const CdSuActorId& dstAId,
                                const CdSuActorId& srcAId,
				const CdSuNhTcpConnId cid,
				const CdSuNhTcpUiConReqParam& conReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_CONNECT_REQ,
				sizeof (CdSuNhTcpUiConReqMsg));
	conReqParam = conReqP;
	connId = cid;

} // CdSuNhTcpUiConReqMsg::initMsg ()

void CdSuNhTcpUiDataReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpUiDataReqParam& dataReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_DATA_REQ,
				sizeof (CdSuNhTcpUiDataReqMsg));

	dataReqParam = dataReqP;

} // CdSuNhTcpUiDataReqMsg::initMsg ()

void CdSuNhTcpUiCloseConReqMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
				const CdSuNhTcpUiCloseConReqParam& closeConReqP)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_CLOSE_CON_REQ,
				sizeof (CdSuNhTcpUiCloseConReqMsg));

	closeConReqParam = closeConReqP;

} // CdSuNhTcpUiCloseConReqMsg::initMsg ()

//============================================================================
// NH->UI Interface Messages. These messages are created and sent by
// the Default Listener and Default Connection Event Handler only if
// the corresponding Event Handlers are not declared by the Application.
//============================================================================

void CdSuNhTcpUiAddLstnRspMsg::initMsg (const CdSuActorId& dstAId,
                                    const CdSuActorId& srcAId,
                                    const CdSuNhTcpUiAddLstnRspEvent& addLstnRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_ADD_LISTENER_RSP,
				sizeof (CdSuNhTcpUiAddLstnRspMsg));
	addLstnRspEvent = addLstnRspEv;

} // CdSuNhTcpUiAddLstnReqMsg::initMsg ()

void CdSuNhTcpUiRemLstnRspMsg::initMsg (const CdSuActorId& dstAId,
                                    const CdSuActorId& srcAId,
                                    const CdSuNhTcpUiRemLstnRspEvent& remLstnRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_REM_LISTENER_RSP,
				sizeof (CdSuNhTcpUiRemLstnRspMsg));
	remLstnRspEvent = remLstnRspEv;

} // CdSuNhTcpUiRemLstnRspMsg::initMsg ()

void CdSuNhTcpUiConRspMsg::initMsg (const CdSuActorId& dstAId,
                                    const CdSuActorId& srcAId,
                                    const CdSuNhTcpUiConRspEvent& conRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_CONNECT_RSP,
				sizeof (CdSuNhTcpUiConRspMsg));
	conRspEvent = conRspEv;

} // CdSuNhTcpUiConRspMsg::initMsg ()

void CdSuNhTcpUiDataRspMsg::initMsg (const CdSuActorId& dstAId,
                                    const CdSuActorId& srcAId,
                                    const CdSuNhTcpUiDataRspEvent& dataRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_DATA_RSP,
				sizeof (CdSuNhTcpUiDataRspMsg));
	dataRspEvent = dataRspEv;

} // CdSuNhTcpUiDataRspMsg::initMsg ()

void CdSuNhTcpUiCloseConRspMsg::initMsg (const CdSuActorId& dstAId,
                                    const CdSuActorId& srcAId,
                                    const CdSuNhTcpUiCloseConRspEvent& closeConRspEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_CLOSE_CON_RSP,
				sizeof (CdSuNhTcpUiCloseConRspMsg));
	closeConRspEvent = closeConRspEv;

} // CdSuNhTcpUiCloseConRspMsg::initMsg ()

void CdSuNhTcpUiDataIndMsg::initMsg (const CdSuActorId& dstAId,
                               const CdSuActorId& srcAId,
                               const CdSuNhTcpUiDataIndEvent& dataIndEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_DATA_IND,
				sizeof (CdSuNhTcpUiDataIndMsg));
	dataIndEvent = dataIndEv;

} // CdSuNhTcpUiDataIndMsg::initMsg ()

void CdSuNhTcpUiConIndMsg::initMsg (const CdSuActorId& dstAId,
				const CdSuActorId& srcAId,
                                const CdSuNhTcpUiConIndEvent& conIndEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_CON_IND,
				sizeof (CdSuNhTcpUiConIndMsg));
	conIndEvent = conIndEv;

} // CdSuNhTcpUiConIndMsg::initMsg ()

void CdSuNhTcpUiCloseConIndMsg::initMsg (const CdSuActorId& dstAId,
                                    const CdSuActorId& srcAId,
                                    const CdSuNhTcpUiCloseConIndEvent& closeConIndEv)
{
	((CdMessage*) this)->initMsg (dstAId, 
				srcAId,
				CDSU_NHTCP_UI_CLOSE_CON_IND,
				sizeof (CdSuNhTcpUiCloseConIndMsg));
	closeConIndEvent = closeConIndEv;

} // CdSuNhTcpUiCloseConIndMsg::initMsg ()

//----------------------------------------------------------------------------
// < End of cdsunhtcpmsg.cpp>
//----------------------------------------------------------------------------
