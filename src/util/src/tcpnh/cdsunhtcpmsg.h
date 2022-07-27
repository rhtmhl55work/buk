#ifndef __CDSU_NHTCP_MESSAGE_H_
#define __CDSU_NHTCP_MESSAGE_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpmsg.h
// Description  : 
// Author       : Seema Jain 
// Created On   : Mon Feb  9 11:02:29 IST 2004
// Last Modified:
// Notes	: 
//			*) Message structure definitions. 
//============================================================================

//----------------------------------------------------------------------------
// Includes.
//----------------------------------------------------------------------------

#include "cdsuutiltypes.h"
#include "cdsunhtcpapiparam.h"

//----------------------------------------------------------------------------
// 			Management Interface (MI) Messages
//			----------------------------------
// 
//	* SM -> NH
//		+ InitReq
//		+ 
//
//	* NH -> SM
//		+ 
//
//			Upper Interface (UI) Messages
//			-----------------------------
//		
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Message	: MiInitReq
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhTcpMiInitReqMsg : public CdMessage
{
	CdSuNhTcpMiInitParam initParam;	

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid,
			const CdSuNhTcpMiInitParam& initParam);
};

//----------------------------------------------------------------------------
// Message	: DelNhReq
// Description	: 
//----------------------------------------------------------------------------


struct CdSuNhTcpMiDelNhReqMsg : public CdMessage
{
	CdSuNhTcpMiDelNhParam delNhParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid,
			const CdSuNhTcpMiDelNhParam& delNhParam);
};

struct CdSuNhTcpMiCtrlReqMsg : public CdMessage
{
	CdSuNhTcpMiCtrlReqParam ctrlReqParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiCtrlReqParam& ctrlReqParam);

};

struct CdSuNhTcpMiCfgReqMsg : public CdMessage
{
	CdSuNhTcpMiCfgReqParam cfgReqParam;
	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiCfgReqParam& cfgReqParam);

};

struct CdSuNhTcpMiStatusReqMsg : public CdMessage
{

	CdSuNhTcpMiStatusReqParam statusReqParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiStatusReqParam& statusReqParam);
};

struct CdSuNhTcpMiStsReqMsg : public CdMessage
{
	CdSuNhTcpMiStsReqParam stsReqParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiStsReqParam& stsReqParam);

};

struct CdSuNhTcpMiInitRspMsg : public CdMessage
{
	CdSuNhTcpMiInitRspEvent initRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiInitRspEvent& initRspEvent);
};

struct CdSuNhTcpMiDelNhRspMsg : public CdMessage
{
	CdSuNhTcpMiDelNhRspEvent delNhRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiDelNhRspEvent& delNhRspEvent);
};

struct CdSuNhTcpMiCtrlRspMsg : public CdMessage
{

	CdSuNhTcpMiCtrlRspEvent ctrlRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiCtrlRspEvent& ctrlRspEvent);

};

struct CdSuNhTcpMiCfgRspMsg : public CdMessage
{
	CdSuNhTcpMiCfgRspEvent cfgRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiCfgRspEvent& cfgRspEvent);
};

struct CdSuNhTcpMiStatusRspMsg : public CdMessage
{
	CdSuNhTcpMiStatusRspEvent statusRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiStatusRspEvent& statusRspEvent);
};

struct CdSuNhTcpMiStsRspMsg : public CdMessage
{
	CdSuNhTcpMiStsRspEvent stsRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiStsRspEvent& stsReqEvent);

};

struct CdSuNhTcpMiUStaIndMsg : public CdMessage
{
	CdSuNhTcpMiUStaIndEvent ustaIndEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpMiUStaIndEvent& ustaIndEvent);
};

struct CdSuNhTcpUiAddLstnReqMsg : public CdMessage
{
	CdSuNhTcpUiAddLstnReqParam addLstnReqParam;
	CdSuNhTcpLstnId lstnId;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpLstnId lid,
			const CdSuNhTcpUiAddLstnReqParam& addLstnReqParam);

};

struct CdSuNhTcpUiRemLstnReqMsg : public CdMessage
{
	CdSuNhTcpUiRemLstnReqParam remLstnReqParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiRemLstnReqParam& remLstnReqParam);
};

struct CdSuNhTcpUiConReqMsg : public CdMessage
{
	CdSuNhTcpUiConReqParam conReqParam;
	CdSuNhTcpConnId connId;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpConnId cid,
			const CdSuNhTcpUiConReqParam& conReqParam);
};

struct CdSuNhTcpUiDataReqMsg : public CdMessage
{
	CdSuNhTcpUiDataReqParam dataReqParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiDataReqParam& dataReqParam);
};

struct CdSuNhTcpUiCloseConReqMsg : public CdMessage
{
	CdSuNhTcpUiCloseConReqParam closeConReqParam;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiCloseConReqParam& closeConReqParam);
};


struct CdSuNhTcpUiAddLstnRspMsg : public CdMessage
{
	CdSuNhTcpUiAddLstnRspEvent addLstnRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiAddLstnRspEvent& addLstnRspEvent);
};

struct CdSuNhTcpUiRemLstnRspMsg : public CdMessage
{
	CdSuNhTcpUiRemLstnRspEvent remLstnRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiRemLstnRspEvent& remLstnRspEvent);
};

struct CdSuNhTcpUiConRspMsg : public CdMessage
{
	CdSuNhTcpUiConRspEvent conRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiConRspEvent& conRspEvent);
};

struct CdSuNhTcpUiDataRspMsg : public CdMessage
{
	CdSuNhTcpUiDataRspEvent dataRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiDataRspEvent& dataRspEvent);
};

struct CdSuNhTcpUiCloseConRspMsg : public CdMessage
{
	CdSuNhTcpUiCloseConRspEvent closeConRspEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiCloseConRspEvent& closeConRspEvent);

};

struct CdSuNhTcpUiDataIndMsg : public CdMessage
{
	CdSuNhTcpUiDataIndEvent dataIndEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiDataIndEvent& dataIndEvent);
};

struct CdSuNhTcpUiConIndMsg : public CdMessage
{
	CdSuNhTcpUiConIndEvent conIndEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiConIndEvent& conIndEvent);
};

struct CdSuNhTcpUiCloseConIndMsg : public CdMessage
{
	CdSuNhTcpUiCloseConIndEvent closeConIndEvent;

	void initMsg (const CdSuActorId& dstAid, 
			const CdSuActorId& srcAid, 
			const CdSuNhTcpUiCloseConIndEvent& closeConIndEvent);

};

//============================================================================
// <End of file>
//============================================================================
#endif
