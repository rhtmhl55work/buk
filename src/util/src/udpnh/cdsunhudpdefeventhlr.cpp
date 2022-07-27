//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpdefeventhlr.cpp
// Description	: Function declaration of .
// Author       : Vijaya Kumar
// Created On   :  
//============================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include "cdsuutiltypes.h"
#include "cdsunhudptypes.h"
#include "cdsunhudpapiparam.h"
#include "cdsutrace.h"
#include "cdsunhudpinstmgr.h"
#include "cdsunhudpdb.h"
#include "cdsunhudpmsg.h"
#include "cdsumdapi.h"

//---------------------------------------------------------------------------
// Extern Declaration od Insntance Manager.
//---------------------------------------------------------------------------
extern  CdSuNhUdpInstMgr* cdSuNhUdpInstMgr;

//---------------------------------------------------------------------------
// Function	: cdSuNhUdpDefSmEventHlr ()
// Description  : Default Event Hlr for SM Interface.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void cdSuNhUdpDefSmEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiEvent* eparam = (CdSuNhUdpMiEvent*) arg;
	
	// If Instance Manager is NULL then some error.	
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return; 
	}

	// Get DB.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (eparam->nhInstId);
	CdMessage* rspMsg = NULL;
	
	// Depending upon type of param form message.
	switch (eparam->opcode)
	{

	//--------------------------------------------------------------------
	// NH Init Resposne	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_INIT_RSP:
		{
		// Build Message	
		rspMsg =  CDSU_GETMSGBUF (CdSuNhUdpMiInitRspMsg);
		( (CdSuNhUdpMiInitRspMsg*)rspMsg)->initMsg (eparam->smActorId, db->getNhActorId (),
				*( (CdSuNhUdpMiInitRspEvent*) (eparam)));		
		// Free  Param.	
		delete 	(CdSuNhUdpMiInitRspEvent*) (eparam);
		}
		break;
	
	//--------------------------------------------------------------------
	// NH Delete Resposne	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_DELNH_RSP:
		{	
		// Build Message	
		rspMsg = CDSU_GETMSGBUF (CdSuNhUdpMiDelNhRspMsg);
		( (CdSuNhUdpMiDelNhRspMsg*)rspMsg)->initMsg (eparam->smActorId, db->getNhActorId (),
				*( (CdSuNhUdpMiDelNhRspEvent*) (eparam)));	
		// Free Param
		delete 	(CdSuNhUdpMiDelNhRspEvent*) (eparam);
		}
		break;

	//--------------------------------------------------------------------
	// Control Resposne	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_CTRL_RSP:
		{	
		// Build Message
		rspMsg = CDSU_GETMSGBUF (CdSuNhUdpMiCtrlRspMsg);
		( (CdSuNhUdpMiCtrlRspMsg*)rspMsg)->initMsg (eparam->smActorId, db->getNhActorId (),
				*( (CdSuNhUdpMiCtrlRspEvent*) (eparam)));
		// Free Param
		delete 	(CdSuNhUdpMiCtrlRspEvent*) (eparam);
		}
		break;
	
	//--------------------------------------------------------------------
	// Status Response	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_STATUS_RSP:
		{
		// Build Message
		rspMsg = CDSU_GETMSGBUF (CdSuNhUdpMiStatusRspMsg);
		( (CdSuNhUdpMiStatusRspMsg*)rspMsg)->initMsg (eparam->smActorId, db->getNhActorId (),
				*( (CdSuNhUdpMiStatusRspEvent*) (eparam)));	
		// Free Param.
		delete 	(CdSuNhUdpMiStatusRspEvent*) (eparam);
		}
		break;

	//--------------------------------------------------------------------
	// Statistics Response	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_STATISTICS_RSP:
		{
		// Build Message
		rspMsg = CDSU_GETMSGBUF (CdSuNhUdpMiStsRspMsg);
		( (CdSuNhUdpMiStsRspMsg*)rspMsg)->initMsg (eparam->smActorId, db->getNhActorId (),
				*( (CdSuNhUdpMiStsRspEvent*) (eparam)));	
		// Free Param
		delete 	(CdSuNhUdpMiStsRspEvent*) (eparam);
		}
		break;
	
	//TODO: case for unsolicated rsp to be added.
	default:
		{
		
		}	
		break;
	} // switch 
       	
	// Send the message. 
	if (rspMsg != NULL)
	{
		if (cdSuMdSendMsg (rspMsg) == true)
		{
			CDSU_TRACE (1, " Default Event Hlr (): cdSuMdSendMsg success!\n");
			return;
		} else 
		{
			// Error
			CDSU_TRACE (1, "Default Event Hlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (rspMsg);
			return;
		}
	}
	
} // cdSuNhUdpDefSmEventHlr 

//---------------------------------------------------------------------------
// Function	: cdSuNhUdpDefApplEventHlr ()
// Description  : Default Event Hlr for Application Interface.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void cdSuNhUdpDefApplEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	CdSuNhUdpUiEvent* eparam =  (CdSuNhUdpUiEvent*) arg;
	
	// If Instance Manager is NULL then error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return; 
	}

	CdSuNhUdpDb* db = NULL;	
	CdMessage* rspMsg = NULL;
	
	// Get DB.
	db = cdSuNhUdpInstMgr->getNhUdpDb (eparam->nhUdpPortKey.nhUdpInstId);
	
	// Depending upon type of param form message.
	switch (eparam->opcode)
	{
	//--------------------------------------------------------------------
	// Add Udp Port Response	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_ADD_UDPPORT_RSP:
		{
		// Build Message
		rspMsg =  CDSU_GETMSGBUF (CdSuNhUdpUiAddUdpPortRspMsg);
		( (CdSuNhUdpUiAddUdpPortRspMsg*)rspMsg)->initMsg (eparam->applActorId,
				db->getNhActorId (), *( (CdSuNhUdpUiAddUdpPortRspEvent*) (eparam)));
		//Delete Param.
		delete (CdSuNhUdpUiAddUdpPortRspEvent*) eparam;
		}
		break;
	
	//--------------------------------------------------------------------
	// Remove Udp Port Response	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_REM_UDPPORT_RSP:
		{
		// Build Message
		rspMsg =  CDSU_GETMSGBUF (CdSuNhUdpUiRemUdpPortRspMsg);
		( (CdSuNhUdpUiRemUdpPortRspMsg*)rspMsg)->initMsg (eparam->applActorId,
				db->getNhActorId (), *( (CdSuNhUdpUiRemUdpPortRspEvent*) (eparam)));
		//Delete param.
		delete (CdSuNhUdpUiRemUdpPortRspEvent*) eparam;
		}
		break;
	
	//--------------------------------------------------------------------
	// Remove Udp Port Response	
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_DATA_RSP:
		{
		// Build Message.
		rspMsg =  CDSU_GETMSGBUF (CdSuNhUdpUiDataRspMsg);
		( (CdSuNhUdpUiDataRspMsg*)rspMsg)->initMsg (eparam->applActorId,
				db->getNhActorId (), *( (CdSuNhUdpUiDataRspEvent*) (eparam)));
		//Delete param.
		delete (CdSuNhUdpUiDataRspEvent*) eparam;
		}
		break;
	
	//--------------------------------------------------------------------
	// Send Data  Response	(TO BE REMOVED)
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_SEND_RSP:
		{
		/*db = cdSuNhUdpInstMgr->getNhUdpDb (eparam->nhUdpInstId);
		rspMsg =  CDSU_GETMSGBUF (CdSuNhUdpUiSendRspMsg);
		( (CdSuNhUdpUiSendRspMsg*)rspMsg)->initMsg (eparam->applActorId,
			    				    db->getNhActorId (),
							    *( (CdSuNhUdpUiSendRspEvent*) (eparam)));
		//Delete event.
		delete (CdSuNhUdpUiSendRspEvent*) eparam;*/
		}
		break;

	//--------------------------------------------------------------------
	// Data Indication
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_DATA_IND:
		{
		// Build Message.		
		rspMsg =  CDSU_GETMSGBUF (CdSuNhUdpUiDataIndMsg);
		( (CdSuNhUdpUiDataIndMsg*)rspMsg)->initMsg (eparam->applActorId,
				db->getNhActorId (), *( (CdSuNhUdpUiDataIndEvent*) (eparam)));
		//Delete param.
		delete (CdSuNhUdpUiDataIndEvent*) eparam;
		}
		break;
	
	default:
		CDSU_TRACE (1, "Default Event Hlr (): Unknown Msg \n");
		break;
	} // switch ()
	
	// Send the message. 
	if (rspMsg != NULL)
	{
		if (cdSuMdSendMsg (rspMsg) == true)
		{
			CDSU_TRACE (5, "Default Event Hlr(): cdSuMdSendMsg success\n");
		return;
		} else 
		{
			CDSU_TRACE (1, "Default Event Hlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (rspMsg);
			return;
		}
	}

} // void cdSuNhUdpDefApplEventHlr ()

//============================================================================
// <End Of File cdsunhudpdefeventhlr.cpp>
//============================================================================

