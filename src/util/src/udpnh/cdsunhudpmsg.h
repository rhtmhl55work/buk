#ifndef __CDSU_NHUDP_MESSAGE_H_
#define __CDSU_NHUDP_MESSAGE_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpmsg.h
// Description  : Contains Definition of messages.
// Author       : Vijaya Kumar  
// Created On   : Mon Mar  8 20:27:29 IST 2004
// Last Modified:
// Notes	: 
//			*) Message structure definitions. 
//============================================================================

//----------------------------------------------------------------------------
// Includes.
//----------------------------------------------------------------------------

#include "cdsuutiltypes.h"
#include "cdsunhudpapiparam.h"

//----------------------------------------------------------------------------
// 			Management Interface (MI) Messages
//			----------------------------------
//				Dir: MI -> NH			
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Message	: Delete NH Request Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiDelNhReqMsg : public CdMessage
{
	CdSuNhUdpMiDelNhParam delNhParam;	// Delete NH Request Parameter 

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId,
			const CdSuNhUdpMiDelNhParam& delNhParam);

}; // struct CdSuNhUdpMiDelNhReqMsg

//----------------------------------------------------------------------------
// Message	:  Control NH Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCtrlReqMsg : public CdMessage
{
	CdSuNhUdpMiCtrlReqParam ctrlReqParam; 	// Control Request Parameter.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiCtrlReqParam& ctrlReqParam);

}; // struct CdSuNhUdpMiCtrlReqMsg

//----------------------------------------------------------------------------
// Message	:  Configure NH Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCfgReqMsg : public CdMessage
{
	CdSuNhUdpMiCfgReqParam cfgReqParam;	// Config Request Param
	
	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiCfgReqParam& cfgReqParam);
	
}; // struct CdSuNhUdpMiCfgReqMsg

//----------------------------------------------------------------------------
// Message	:  NH Status Request Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStatusReqMsg : public CdMessage
{
	
	CdSuNhUdpMiStatusReqParam statusReqParam;	// Status Request Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiStatusReqParam& statusReqParam);

}; // struct CdSuNhUdpMiStatusReqMsg 

//----------------------------------------------------------------------------
// Message	:  NH Statistics request Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStsReqMsg : public CdMessage
{
	CdSuNhUdpMiStsReqParam stsReqParam;	// Statistics Request Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiStsReqParam& stsReqParam);

}; // struct CdSuNhUdpMiStsReqMsg

//----------------------------------------------------------------------------
// 			Management Interface (MI) Messages
//			----------------------------------
//			Dir: NH -> MI			
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Message	:  NH Init Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiInitRspMsg : public CdMessage
{
	
	CdSuNhUdpMiInitRspEvent initRspEvent;	// Init Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiInitRspEvent& initRspEvent);
	
}; // struct CdSuNhUdpMiInitRspMsg 

//----------------------------------------------------------------------------
// Message	:  NH Delete Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiDelNhRspMsg : public CdMessage
{
	CdSuNhUdpMiDelNhRspEvent delNhRspEvent;		// Delete Nh Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiDelNhRspEvent& delNhRspEvent);
	
}; // struct CdSuNhUdpMiDelNhRspMsg 

//----------------------------------------------------------------------------
// Message	:  NH Control Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCtrlRspMsg : public CdMessage
{

	CdSuNhUdpMiCtrlRspEvent ctrlRspEvent;	// Control Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiCtrlRspEvent& ctrlRspEvent);

}; // struct CdSuNhUdpMiCtrlRspMsg

//----------------------------------------------------------------------------
// Message	:  NH Configure response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiCfgRspMsg : public CdMessage
{

	CdSuNhUdpMiCfgRspEvent cfgRspEvent;	// Config Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiCfgRspEvent& cfgRspEvent);
	
}; // struct CdSuNhUdpMiCfgRspMsg

//----------------------------------------------------------------------------
// Message	:  NH Status Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStatusRspMsg : public CdMessage
{
	CdSuNhUdpMiStatusRspEvent statusRspEvent;	// Status Response Event Param

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiStatusRspEvent& statusRspEvent);
	
}; // struct CdSuNhUdpMiStatusRspMsg

//----------------------------------------------------------------------------
// Message	:  NH Statistics Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpMiStsRspMsg : public CdMessage
{
	CdSuNhUdpMiStsRspEvent stsRspEvent;	// Statistics Response Event param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpMiStsRspEvent& stsReqEvent);

}; // struct CdSuNhUdpMiStsRspMsg 

//----------------------------------------------------------------------------
// 			User Interface (UI) Messages
//			----------------------------------
//			Dir: UI -> NH			
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Message	:  Add Udp Port request Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiAddUdpPortReqMsg : public CdMessage
{
	CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;	// Add Udp Port Request Param
	CdSuNhUdpPortId udpPortId;				// Udp Port ID
	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiAddUdpPortReqParam& addUdpPortReqParam,
			const CdSuNhUdpPortId& udpPortId);
	
}; // struct CdSuNhUdpUiAddUdpPortReqMsg

//----------------------------------------------------------------------------
// Message	:  Remover Udp Port Request  Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiRemUdpPortReqMsg : public CdMessage
{
	CdSuNhUdpUiRemUdpPortReqParam remUdpPortReqParam;	// Remove Udp Port Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiRemUdpPortReqParam& remUdpPortReqParam);
	
}; // struct CdSuNhUdpUiRemUdpPortReqMsg 

//----------------------------------------------------------------------------
// Message	:  Send Data request Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiDataReqMsg : public CdMessage
{
	CdSuNhUdpUiDataReqParam dataReqParam; 	// Send Data Request param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiDataReqParam& dataReqParam);
};

//----------------------------------------------------------------------------
// Message	:  Send Data request Message Without Opening Udp Port
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiSendReqMsg : public CdMessage
{
	CdSuNhUdpUiSendReqParam sendReqParam;	// Send Req Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiSendReqParam& sendReqParam);
	
}; // struct CdSuNhUdpUiSendReqMsg


//----------------------------------------------------------------------------
// 			User Interface (UI) Messages
//			----------------------------------
//			Dir: NH -> UI			
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Message	:  Add Udp Port response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiAddUdpPortRspMsg : public CdMessage
{
	CdSuNhUdpUiAddUdpPortRspEvent addUdpPortRspEvent;	// Add Udp Port Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiAddUdpPortRspEvent& addUdpPortRspEvent);

}; // struct CdSuNhUdpUiAddUdpPortRspMsg 

//----------------------------------------------------------------------------
// Message	:  Remove Udp Port Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiRemUdpPortRspMsg : public CdMessage
{
	CdSuNhUdpUiRemUdpPortRspEvent remUdpPortRspEvent;	// Remove Udp Port Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiRemUdpPortRspEvent& remUdpPortRspEvent);
	
}; // struct CdSuNhUdpUiRemUdpPortRspMsg

//----------------------------------------------------------------------------
// Message	:  Send Data Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiDataRspMsg : public CdMessage
{
	CdSuNhUdpUiDataRspEvent dataRspEvent; 	// Data Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiDataRspEvent& dataRspEvent);
	
}; // struct CdSuNhUdpUiDataRspMsg 


//----------------------------------------------------------------------------
// Message	:  Send Response Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiSendRspMsg : public CdMessage
{
	CdSuNhUdpUiSendRspEvent sendRspEvent;	// Send Response Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiSendRspEvent& sendRspEvent);
	
}; // struct CdSuNhUdpUiSendRspMsg

//----------------------------------------------------------------------------
// Message	:  Data Indication Message
// Description	: 
//----------------------------------------------------------------------------

struct CdSuNhUdpUiDataIndMsg : public CdMessage
{
	CdSuNhUdpUiDataIndEvent dataIndEvent;	// Data Indication Event Param.

	void initMsg (const CdSuActorId& dstAId, 
			const CdSuActorId& srcAId, 
			const CdSuNhUdpUiDataIndEvent& dataIndEvent);
	
}; // struct CdSuNhUdpUiDataIndMsg

//============================================================================
// <End of file cdsunhudpmsg.h>
//============================================================================
#endif

