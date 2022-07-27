//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpapi.cpp
// Description	: It contains the description of the API provided by the
//   		  TCP Network Handler class.
// Author       : Seema Jain
// Created On   : Tue Feb 10 18:07:50 IST 2004
// Last Modified: By Sumit Bijwe  on Wed Feb 25 10:21:38 IST 2004
//               + Added cdSuNhTcpMiTerminate () API.
//============================================================================

//============================================================================
// Include files.
//============================================================================
#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsumdapi.h"
#include "cdsunhtcptypes.h"
#include "cdsunhtcpapiparam.h"
#include "cdsunhtcpdb.h"
#include "cdsunhtcp.h"
#include "cdsunhtcpinstmgr.h"
#include "cdsunhtcpmsg.h"
#include "cdsunhtcpapiparamchk.h"


//============================================================================
// Forward Declaration
//============================================================================


CdSuNhTcpInstMgr* cdSuNhTcpInstMgr;

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpMiInit ()
// Description  : Initializes the Network Handler Instance. It creates the
// 		  Network Handler Instance Manager if it is not already
// 		  created. Otherwise it gets the pointer to the Network
// 		  Handler Manager Class. It returns the Network Handler
// 		  Instance Id. 
// Inputs       : InitParam: Configurable parameters.
// Return Value : CdSuNhTcpInstId,  The Instance Id of the Network Handler
// 		  instance. This is a logical Id of the Network Handler
// 		  instance.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiInit (const CdSuNhTcpMiInitParam& initParam, 
					    CdSuNhTcpInstId& nhInstId)
{
	// Create the singleton and call the add NH method.
	cdSuNhTcpInstMgr = CdSuNhTcpInstMgr::createNhTcpInstMgr ();

	// First validate nh init parameter. 
	CdSuNhTcpApiRetType retType = validateInitNhParam (initParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	if (cdSuNhTcpInstMgr != NULL)
	{
		nhInstId = cdSuNhTcpInstMgr->addNhTcpInst (initParam);
		return (retType);
	} else
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpInstId cdSuNhTcpMiInit ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpDelNhReq ()
// Description  : Deletes the Network Handler Instance.
// Inputs       : DelNhParam: 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiDelNhReq (const CdSuNhTcpMiDelNhParam& delNhParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// First validate the delete Nh param.
	CdSuNhTcpApiRetType retType = validateDelNhParam (delNhParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (delNhParam.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpMiDelNhReq(): Invalid nhTcpInstId: %d\n", delNhParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	// Create the appropriate message to be sent.
	CdSuNhTcpMiDelNhReqMsg* delNhReqMsg =  CDSU_GETMSGBUF (CdSuNhTcpMiDelNhReqMsg);

	delNhReqMsg->initMsg (db->getNhActorId (), 
				db->getSmActorId (),
				delNhParam);		

	// Send the message. 
	if (cdSuMdSendMsg (delNhReqMsg) == true)
	{
		return (retType);
	} else 
	{
		CDSU_TRACE (1, "cdSuNhTcpMiDelNhReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (delNhReqMsg);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpApiRetType cdSuNhTcpMiDelNhReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpMiTerminate ()
// Description  : Terminates the Network Handler Instance.
// Inputs       : TerminateNhParam 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiTerminate (const CdSuNhTcpMiTerminateParam& termParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// First validate the Terminate Nh param.
	CdSuNhTcpApiRetType retType = validateTerminateNhParam (termParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (termParam.nhTcpInstId);
	// NHTCP should be in deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		if (db != NULL)
		{
			bool retval = cdSuNhTcpInstMgr->removeNhTcpInst (termParam.nhTcpInstId);
			if (cdSuNhTcpInstMgr->getNhInstCounter () <= 0)
			{
				delete cdSuNhTcpInstMgr; 
				cdSuNhTcpInstMgr = NULL;
			}
			
			if (retval != false)	
			{
				return (retType);
			} else
			{
				CDSU_TRACE (1, "cdSuNhTcpMiTerminate (): failed!\n");
				return (CDSU_NHTCP_API_RT_FAIL);
			}	
		} // if (db)
	
	} else
	{
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	} 

	return (retType);

} //  cdSuNhTcpMiTerminate ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpMiCtrlReq ()
// Description  : Control Request for a Network Handler instance.
// Inputs       : CtrlReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiCtrlReq (const CdSuNhTcpMiCtrlReqParam& ctrlReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}
	
	// First validate the Ctrl req param.
	CdSuNhTcpApiRetType retType = validateCtrlReqParam (ctrlReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (ctrlReqParam.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpMiCtrlReq(): Invalid nhTcpInstId: %d\n", ctrlReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}


	CdSuNhTcpMiCtrlReqMsg* ctrlMsg = CDSU_GETMSGBUF (CdSuNhTcpMiCtrlReqMsg); 
	ctrlMsg->initMsg (db->getNhActorId (),
			  db->getSmActorId (), ctrlReqParam);		

	if (cdSuMdSendMsg (ctrlMsg) == true)
	{
		return (retType);
	} else 
	{
		CDSU_FREEMSGBUF (ctrlMsg);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpInstId cdSuNhTcpMiCtrlReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpMiCfgReq ()
// Description  : Config Request for a Network Handler instance.
// Inputs       : CfgReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiCfgReq (const CdSuNhTcpMiCfgReqParam& cfgReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
		return (CDSU_NHTCP_API_RT_FAIL);

	// First validate the config Nh param.
	CdSuNhTcpApiRetType retType = validateCfgReqParam (cfgReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	// Extract the Destination ModuleId.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (cfgReqParam.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpMiCfgReq(): Invalid nhTcpInstId: %d\n", cfgReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	// Get a message buffer and fill it up.
	CdSuNhTcpMiCfgReqMsg* cfgMsg = CDSU_GETMSGBUF (CdSuNhTcpMiCfgReqMsg); 
	cfgMsg->initMsg (db->getNhActorId (), 
			db->getSmActorId (),
			cfgReqParam);		

	if (cdSuMdSendMsg (cfgMsg) == true)
	{
		return (retType);
	} else 
	{
		CDSU_FREEMSGBUF (cfgMsg);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpInstId cdSuNhTcpMiCfgReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpMiStatus ()
// Description  : Status Request for a Network Handler instance.
// Inputs       : StatusReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiStatusReq (const CdSuNhTcpMiStatusReqParam& statusReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}
	// First validate the Status Req Nh param.
	CdSuNhTcpApiRetType retType = validateStatusReqParam (statusReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (statusReqParam.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpMiStatusReqParam(): Invalid nhTcpInstId: %d\n", statusReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	CdSuNhTcpMiStatusReqMsg* statusMsg = CDSU_GETMSGBUF (CdSuNhTcpMiStatusReqMsg); 
	statusMsg->initMsg (db->getNhActorId (),
				db->getSmActorId (),
				statusReqParam);		

	if (cdSuMdSendMsg (statusMsg) == true)
	{
		return (retType);
	}
	else 
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	
	}

} // CdSuNhTcpInstId cdSuNhTcpMiStatusReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpMiStsReq ()
// Description  : Statistics Request for a Network Handler instance.
// Inputs       : StatusReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiStsReq (const CdSuNhTcpMiStsReqParam& stsReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// First validate the Statstics Req Nh param.
	CdSuNhTcpApiRetType retType = validateStsReqParam(stsReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (stsReqParam.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpMiStatusReqParam(): Invalid nhTcpInstId: %d\n", stsReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	CdSuNhTcpMiStsReqMsg* stsMsg = CDSU_GETMSGBUF (CdSuNhTcpMiStsReqMsg); 
	stsMsg->initMsg (db->getNhActorId (),
			db->getSmActorId (),
			stsReqParam);		
	if (cdSuMdSendMsg (stsMsg) == true)
	{
		return (retType);
	}
	else 
	{
		
		return (CDSU_NHTCP_API_RT_FAIL); 
	} 

} // CdSuNhTcpInstId cdSuNhTcpMiStsReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhTcpUiAddLstnReq ()
// Description  : Request to add a listener.
// Inputs       : .  
// Return Value : 
// Side Effects : 
// Algorithm    :
//			*) Allocate a listener Id.
//			*) Update the listener param.
//			*) Create the Add Listener Msg.
//			*) Send the Add Listener Msg.
//			*) Return the listener Id. 
//
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpUiAddLstnReq (const CdSuNhTcpUiAddLstnReqParam& arp, 
						CdSuNhTcpLstnKey& lkey)
{
	CdSuNhTcpUiAddLstnReqParam addLstnReqParam = arp;

	if (cdSuNhTcpInstMgr == NULL)
	{
		lkey.init (CDSU_NHTCP_INV_LSTNID, addLstnReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_FAIL);

	}

	// First validate the Add Listener param.
	CdSuNhTcpApiRetType retType = validateAddLstnReqParam (arp);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (addLstnReqParam.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpUiAddLstnReq (): Invalid nhTcpInstId: %d\n", 
			     addLstnReqParam.nhTcpInstId);
		lkey.init (CDSU_NHTCP_INV_LSTNID, addLstnReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);

	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}


	// Allocate a listener Id.
	CdSuNhTcpLstnId lstnId = db->allocLstnId ();
	if (lstnId == CDSU_NHTCP_INV_LSTNID)
	{
		lkey.init (CDSU_NHTCP_INV_LSTNID, addLstnReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// Fill a message and mail it to the appropriate NH.
	// Set return Listen Key.
	lkey.init (lstnId, addLstnReqParam.nhTcpInstId);
	// Set lstnId in addLstnReqParam .
	CdSuNhTcpUiAddLstnReqMsg* addLstnReqMsg = CDSU_GETMSGBUF (CdSuNhTcpUiAddLstnReqMsg);	
	addLstnReqMsg->initMsg (db->getNhActorId (), 
				addLstnReqParam.applActorId, 
				lstnId, 
				addLstnReqParam);		

	if (cdSuMdSendMsg (addLstnReqMsg) == true)
	{
		return (retType);
	} else 
	{
		// Failure to mail. Deallocate msgBuf and LstnId.
		CDSU_TRACE (1, "cdSuNhTcpUiAddLstnReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (addLstnReqMsg);

		// Deallocate Listener Id.
		if (db->freeLstnId (lstnId) == false)
		{
			CDSU_TRACE (1, "free Listener Id failed!\n");
		}

		lkey.init (CDSU_NHTCP_INV_LSTNID, addLstnReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpInstId cdSuNhTcpUiAddLstnReq ()

CdSuNhTcpApiRetType cdSuNhTcpUiRemLstnReq (const CdSuNhTcpUiRemLstnReqParam& remLstnReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// First validate the Remove Listener param.
	CdSuNhTcpApiRetType retType = validateRemLstnReqParam (remLstnReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}
	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (remLstnReqParam.lstnKey.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpUiRemLstnReq (): Invalid nhTcpInstId: %d\n", 
			     remLstnReqParam.lstnKey.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}


	// Send a Request to the TCP NH to remove the listener.
	CdSuNhTcpUiRemLstnReqMsg* remLstnReqMsg = CDSU_GETMSGBUF (CdSuNhTcpUiRemLstnReqMsg);	
	remLstnReqMsg->initMsg (db->getNhActorId (), 
				remLstnReqParam.applActorId, 
				remLstnReqParam);		

	if (cdSuMdSendMsg (remLstnReqMsg) == true)
	{
		return (retType);
	} else 
	{
		// Failure to mail. Deallocate msg.
		CDSU_TRACE (1, "cdSuNhTcpUiRemLstnReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (remLstnReqMsg);
		return (CDSU_NHTCP_API_RT_FAIL);

	}

} // CdSuNhTcpApiRetType CdSuNhTcpUiRemLstnReq ()

CdSuNhTcpApiRetType cdSuNhTcpUiConReq (const CdSuNhTcpUiConReqParam& crp,CdSuNhTcpConnKey& ckey)
{
	CdSuNhTcpUiConReqParam conReqParam = crp;
	
	if (cdSuNhTcpInstMgr == NULL)
	{
		CdSuNhTcpConnKey cKey;
		cKey.init (CDSU_NHTCP_INV_CONNID, conReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// First validate the Connection Request param.
	CdSuNhTcpApiRetType retType = validateConReqParam (crp);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (conReqParam.nhTcpInstId);

	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpUiConReqParam (): Invalid nhTcpInstId: %d\n", 
			     conReqParam.nhTcpInstId);
		ckey.init (CDSU_NHTCP_INV_CONNID,
			conReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	// Allocate a Connection Id.
	CdSuNhTcpConnId connId = db->allocConnId ();
	if (connId == CDSU_NHTCP_INV_CONNID)
	{
		ckey.init (connId, conReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_FAIL);
	}


	// Fill a message and mail it to the appropriate NH.
	// Set return Conn key. 
	ckey.init (connId, conReqParam.nhTcpInstId);
	CdSuNhTcpUiConReqMsg* conReqMsg = CDSU_GETMSGBUF (CdSuNhTcpUiConReqMsg);	
	conReqMsg->initMsg (db->getNhActorId (), 
			    	conReqParam.applActorId, 
				connId,
				conReqParam);		

	if (cdSuMdSendMsg (conReqMsg) == true)
	{
		return (retType);
	} else 
	{
		// Failure to mail. Deallocate msg and Connection Id.
		CDSU_TRACE (1, "cdSuNhTcpUiConnReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (conReqMsg);

		// Deallocate Connection Id.
		if (db->freeConnId (connId) == false)
		{
			CDSU_TRACE (1, "free Connection Id failed!\n");
		}

		ckey.init (CDSU_NHTCP_INV_CONNID,
			conReqParam.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpConnId cdSuNhTcpUiConReq () 

CdSuNhTcpApiRetType cdSuNhTcpUiCloseConReq (const CdSuNhTcpUiCloseConReqParam& closeConReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{
		return (CDSU_NHTCP_API_RT_FAIL);
	}

	// First validate the Close Connection param.
	CdSuNhTcpApiRetType retType = validateCloseConReqParam (closeConReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (closeConReqParam.connKey.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpUiConReqParam (): Invalid nhTcpInstId: %d\n", 
			     closeConReqParam.connKey.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	// Fill a message and mail it to the appropriate NH.
	CdSuNhTcpUiCloseConReqMsg* closeConReqMsg = CDSU_GETMSGBUF (CdSuNhTcpUiCloseConReqMsg);	
	closeConReqMsg->initMsg (db->getNhActorId (), 
			    	closeConReqParam.applActorId, 
				closeConReqParam);		

	if (cdSuMdSendMsg (closeConReqMsg) == true)
	{
		return (retType);
	} else 
	{
		// Failure to mail. Deallocate msgBuf.
		CDSU_TRACE (1, "cdSuNhTcpUiCloseConReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (closeConReqMsg);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpApiRetType cdSuNhTcpUiCloseConReq ()

CdSuNhTcpApiRetType cdSuNhTcpUiDataReq (const CdSuNhTcpUiDataReqParam& dataReqParam)
{
	if (cdSuNhTcpInstMgr == NULL)
	{	
		return (CDSU_NHTCP_API_RT_FAIL);
	}
	
	// First validate the Data param.
	CdSuNhTcpApiRetType retType = validateDataReqParam (dataReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		return (retType);
	}

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (dataReqParam.connKey.nhTcpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpUiConReqParam (): Invalid nhTcpInstId: %d\n", 
			     dataReqParam.connKey.nhTcpInstId);
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}
	// Return Operation invalid in NHTCP deleted state.	
	if ((db->getNhTcpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "TCP NH is in deleted state.\n");
		return (CDSU_NHTCP_API_RT_OPERATION_INV);
	}

	// Fill a message and mail it to the appropriate NH.
	CdSuNhTcpUiDataReqMsg* dataReqMsg = CDSU_GETMSGBUF (CdSuNhTcpUiDataReqMsg);	
	dataReqMsg->initMsg (db->getNhActorId (), 
			    	dataReqParam.applActorId, 
				dataReqParam);		

	if (cdSuMdSendMsg (dataReqMsg) == true)
	{
		return (retType);
	} else 
	{
		// Failure to mail. Deallocate msgBuf.
		CDSU_TRACE (1, "cdSuNhTcpUiCloseConReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (dataReqMsg);
		return (CDSU_NHTCP_API_RT_FAIL);
	}

} // CdSuNhTcpApiRetType cdSuNhTcpUiDataReq () 


bool cdSuNhTcpDefLstnEventHlr (const CdSuNhTcpUiLstnEvent& lstnEvent)
{
	if (cdSuNhTcpInstMgr == NULL)
		return false;

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (lstnEvent.lstnKey.nhTcpInstId);
	bool retVal = false;
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpDefLstnEventHlr (): Invalid nhTcpInstId: %d\n", 
			     lstnEvent.lstnKey.nhTcpInstId);
		return (false);
	}

	// Check the type of event.
	switch (lstnEvent.opcode)
	{
	case CDSU_NHTCP_UI_ADD_LISTENER_RSP:
		{
		//------------------------------------------------------------
		// If Add Listener Response Event.
		//------------------------------------------------------------

		// Get the proper Event.
		CdSuNhTcpUiAddLstnRspEvent* addLstnRspEvent = 
				(CdSuNhTcpUiAddLstnRspEvent*) &lstnEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiAddLstnRspMsg* addLstnRspMsg = CDSU_GETMSGBUF (CdSuNhTcpUiAddLstnRspMsg);	

		addLstnRspMsg->initMsg (addLstnRspEvent->applActorId,
					db->getNhActorId (),
					*addLstnRspEvent);
		if (cdSuMdSendMsg (addLstnRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefLstnEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (addLstnRspMsg);
		}
		}
		break;

	case CDSU_NHTCP_UI_REM_LISTENER_RSP:
		{
		//------------------------------------------------------------
		// If Remove Listener Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpUiRemLstnRspEvent* remLstnRspEvent =
				(CdSuNhTcpUiRemLstnRspEvent*) &lstnEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiRemLstnRspMsg* remLstnRspMsg = CDSU_GETMSGBUF (CdSuNhTcpUiRemLstnRspMsg);	

		remLstnRspMsg->initMsg (remLstnRspEvent->applActorId,
					db->getNhActorId (),
					*remLstnRspEvent);
		if (cdSuMdSendMsg (remLstnRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefLstnEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (remLstnRspMsg);
		}
		} 
		break;
	
	default:
		//------------------------------------------------------------
		// If Invalid Listener Response Event
		//------------------------------------------------------------
		{
		CDSU_TRACE (1, "Invalid Response Opcode\n");
		}
		break;
	}

	return (retVal);
	
} // bool cdSuNhTcpDefLstnEventHlr ()

bool cdSuNhTcpDefConEventHlr (const CdSuNhTcpUiConnEvent& conEvent)
{
	if (cdSuNhTcpInstMgr == NULL)
		return false;

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (conEvent.connKey.nhTcpInstId);
	bool retVal = false;
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpDefConEventHlr (): Invalid nhTcpInstId: %d\n", 
			     conEvent.connKey.nhTcpInstId);
		return (false);
	}

	// Check the type of event.
	switch (conEvent.opcode)
	{
	case CDSU_NHTCP_UI_CONNECT_RSP:
		{
		//------------------------------------------------------------
		// If Connect Response Event.
		//------------------------------------------------------------

		// Get the proper Event.
		CdSuNhTcpUiConRspEvent* conRspEvent =
				(CdSuNhTcpUiConRspEvent*) &conEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiConRspMsg* conRspMsg = CDSU_GETMSGBUF (CdSuNhTcpUiConRspMsg);	

		conRspMsg->initMsg (conRspEvent->applActorId,
					db->getNhActorId (),
					*conRspEvent);
		if (cdSuMdSendMsg (conRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefConEventHlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (conRspMsg);
		}

		}
		break;

	case CDSU_NHTCP_UI_DATA_RSP:
		{
		//------------------------------------------------------------
		// If Data Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpUiDataRspEvent* dataRspEvent =
				(CdSuNhTcpUiDataRspEvent*) &conEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiDataRspMsg* dataRspMsg = CDSU_GETMSGBUF (CdSuNhTcpUiDataRspMsg);	

		dataRspMsg->initMsg (dataRspEvent->applActorId,
					db->getNhActorId (),
					*dataRspEvent);
		if (cdSuMdSendMsg (dataRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefConEventHlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (dataRspMsg);
		}
		} 
		break;
	
	case CDSU_NHTCP_UI_CLOSE_CON_RSP:
		{
		//------------------------------------------------------------
		// If Close Connection Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpUiCloseConRspEvent* closeConRspEvent =
				(CdSuNhTcpUiCloseConRspEvent*) &conEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiCloseConRspMsg* closeConMsg = CDSU_GETMSGBUF (CdSuNhTcpUiCloseConRspMsg);	

		closeConMsg->initMsg (closeConRspEvent->applActorId,
					db->getNhActorId (),
					*closeConRspEvent);
		if (cdSuMdSendMsg (closeConMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefConEventHlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (closeConMsg);
		}

		}
		break;

	case CDSU_NHTCP_UI_DATA_IND:
		{
		//------------------------------------------------------------
		// If Close Connection Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpUiDataIndEvent* dataIndEvent =
				(CdSuNhTcpUiDataIndEvent*) &conEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiDataIndMsg* dataIndMsg  = CDSU_GETMSGBUF (CdSuNhTcpUiDataIndMsg);	

		dataIndMsg->initMsg (dataIndEvent->applActorId,
					db->getNhActorId (),
					*dataIndEvent);
		if (cdSuMdSendMsg (dataIndMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefConEventHlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (dataIndMsg);
		}
		}
		break;

	case CDSU_NHTCP_UI_CON_IND:
		{
		//------------------------------------------------------------
		// If Connection Indication Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpUiConIndEvent* conIndEvent =
				(CdSuNhTcpUiConIndEvent*) &conEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiConIndMsg* conIndMsg  = CDSU_GETMSGBUF (CdSuNhTcpUiConIndMsg);	

		conIndMsg->initMsg (conIndEvent->applActorId,
					db->getNhActorId (),
					*conIndEvent);
		if (cdSuMdSendMsg (conIndMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefConEventHlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (conIndMsg);
		}
		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_IND:
		{
		//------------------------------------------------------------
		// If Close Connection Indication Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpUiCloseConIndEvent* closeConIndEvent =
				(CdSuNhTcpUiCloseConIndEvent*) &conEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpUiCloseConIndMsg* closeConIndMsg  = CDSU_GETMSGBUF (CdSuNhTcpUiCloseConIndMsg);	

		closeConIndMsg->initMsg (closeConIndEvent->applActorId,
					db->getNhActorId (),
					*closeConIndEvent);
		if (cdSuMdSendMsg (closeConIndMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpDefConEventHlr (): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (closeConIndMsg);
		}
		}
		break;
	
	default:
		//------------------------------------------------------------
		// If Invalid Connection Response Event
		//------------------------------------------------------------
		{
		CDSU_TRACE (1, "Invalid Response Opcode\n");
		}
		break;
	}

	return (retVal);

} // bool cdSuNhTcpDefConEventHlr ()


bool cdSuNhTcpDefSmEventHlr (const CdSuNhTcpMiEvent& smEvent)
{
	if (cdSuNhTcpInstMgr == NULL)
		return false;

	// Get the DB of the appropriate NH.
	CdSuNhTcpDb* db = cdSuNhTcpInstMgr->getNhTcpDb (smEvent.nhTcpInstId);
	bool retVal = false;
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhTcpSmDefEventHlr (): Invalid nhTcpInstId: %d\n", 
			     smEvent.nhTcpInstId);
		return (false);
	}

	// Check the type of event.
	switch (smEvent.opcode)
	{
	case CDSU_NHTCP_MI_INIT_RSP:
		{
		//------------------------------------------------------------
		// If Init Response Event
		//------------------------------------------------------------

		// Get the proper Event.
		CdSuNhTcpMiInitRspEvent* initRspEvent =
				(CdSuNhTcpMiInitRspEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiInitRspMsg* initRspMsg = CDSU_GETMSGBUF (CdSuNhTcpMiInitRspMsg);	

		initRspMsg->initMsg (initRspEvent->smActorId,
					db->getNhActorId (),
					*initRspEvent);
		if (cdSuMdSendMsg (initRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (initRspMsg);
		}
		}
		break;

	case CDSU_NHTCP_MI_DELNH_RSP:
		{
		//------------------------------------------------------------
		// If Delete Network Handler Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpMiDelNhRspEvent* delNhRspEvent =
				(CdSuNhTcpMiDelNhRspEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiDelNhRspMsg* delNhRspMsg = CDSU_GETMSGBUF (CdSuNhTcpMiDelNhRspMsg);	

		delNhRspMsg->initMsg (delNhRspEvent->smActorId,
					db->getNhActorId (),
					*delNhRspEvent);
		if (cdSuMdSendMsg (delNhRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (delNhRspMsg);
		}
		} 
		break;
	
	case CDSU_NHTCP_MI_CTRL_RSP:
		{
		//------------------------------------------------------------
		// If Control Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpMiCtrlRspEvent* ctrlRspEvent =
				(CdSuNhTcpMiCtrlRspEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiCtrlRspMsg* ctrlRspMsg = CDSU_GETMSGBUF (CdSuNhTcpMiCtrlRspMsg);	

		ctrlRspMsg->initMsg (ctrlRspEvent->smActorId,
					db->getNhActorId (),
					*ctrlRspEvent);
		if (cdSuMdSendMsg (ctrlRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (ctrlRspMsg);
		}
		} 
		break;
	
	case CDSU_NHTCP_MI_CFG_RSP:
		{
		//------------------------------------------------------------
		// If Config Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpMiCfgRspEvent* cfgRspEvent =
				(CdSuNhTcpMiCfgRspEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiCfgRspMsg* cfgRspMsg = CDSU_GETMSGBUF (CdSuNhTcpMiCfgRspMsg);	

		cfgRspMsg->initMsg (cfgRspEvent->smActorId,
					db->getNhActorId (),
					*cfgRspEvent);
		if (cdSuMdSendMsg (cfgRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (cfgRspMsg);
		}
		} 
		break;

	case CDSU_NHTCP_MI_STATUS_RSP:
		{
		//------------------------------------------------------------
		// If Status Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpMiStatusRspEvent* statusRspEvent =
				(CdSuNhTcpMiStatusRspEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiStatusRspMsg* statusRspMsg = CDSU_GETMSGBUF (CdSuNhTcpMiStatusRspMsg);	

		statusRspMsg->initMsg (statusRspEvent->smActorId,
					db->getNhActorId (),
					*statusRspEvent);
		if (cdSuMdSendMsg (statusRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (statusRspMsg);
		}
		}
		break;

	case CDSU_NHTCP_MI_STATISTICS_RSP:
		{
		//------------------------------------------------------------
		// If Statistics Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpMiStsRspEvent* stsRspEvent =
				(CdSuNhTcpMiStsRspEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiStsRspMsg* stsRspMsg = CDSU_GETMSGBUF (CdSuNhTcpMiStsRspMsg);	

		stsRspMsg->initMsg (stsRspEvent->smActorId,
					db->getNhActorId (),
					*stsRspEvent);
		if (cdSuMdSendMsg (stsRspMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (stsRspMsg);
		}
		} 
		break;
	
	case CDSU_NHTCP_MI_USTATUS_IND:
		{
		//------------------------------------------------------------
		// If Statistics Response Event.
		//------------------------------------------------------------
		// Get the proper Event.
		CdSuNhTcpMiUStaIndEvent* ustaIndEvent =
				(CdSuNhTcpMiUStaIndEvent*) &smEvent;
		// Send the Message to the destination specified in the Rsp Event.
		CdSuNhTcpMiUStaIndMsg* ustaIndMsg = CDSU_GETMSGBUF (CdSuNhTcpMiUStaIndMsg);	

		ustaIndMsg->initMsg (ustaIndEvent->smActorId,
					db->getNhActorId (),
					*ustaIndEvent);
		if (cdSuMdSendMsg (ustaIndMsg) == true)
		{
			retVal = true;
		} else 
		{
			// Failure to mail. Deallocate msgBuf.
			CDSU_TRACE (1, " cdSuNhTcpSmDefEventHlr(): cdSuMdSendMsg failed!\n");
			CDSU_FREEMSGBUF (ustaIndMsg);
		}
		}
		break;

	default:
		//------------------------------------------------------------
		// If Invalid Listener Response Event
		//------------------------------------------------------------
		{
		CDSU_TRACE (1, "Invalid Response Opcode\n");
		}
		break;
	}

	return (retVal);
	
} // bool cdSuNhTcpSmDefEventHlr ()

//============================================================================
// <End of cdsunhtcpapi.cpp >
//============================================================================
