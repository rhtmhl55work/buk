//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpapi.cpp
// Description	: It implements UDP NH Api.
// Author       : Vijaya Kumar
// Created On   : Tue Mar  9 13:50:12 IST 2004
// Last Modified: 
//============================================================================

//---------------------------------------------------------------------------
// Include files.
//---------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsumdapi.h"
#include "cdsunhudptypes.h"
#include "cdsunhudpapiparam.h"
#include "cdsunhudpdb.h"
#include "cdsunhudp.h"
#include "cdsunhudpinstmgr.h"
#include "cdsunhudpmsg.h"
#include "cdsunhudpapiparamchk.h"

// Singleton object pointer.
CdSuNhUdpInstMgr* cdSuNhUdpInstMgr = NULL;

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpMiInit ()
// Description  : Initializes the Network Handler Instance. It creates the
// 		  Network Handler Instance Manager if it is not already
// 		  created. Otherwise it gets the pointer to the Network
// 		  Handler Manager Class. It returns the Network Handler
// 		  Instance Id. 
// Inputs       : InitParam: Configurable parameters.
// Return Value : CdSuNhUdpInstId,  The Instance Id of the Network Handler
// 		  instance. This is a logical Id of the Network Handler
// 		  instance.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiInit (const CdSuNhUdpMiInitParam& initParam, 
		CdSuNhUdpInstId& nhInstId)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retType;
	nhInstId = CDSU_NHUDP_INV_NHINSTID;
	
	// Validate Parameter.
	retType = validateInitNhParam (initParam);
	if ( retType !=  CDSU_NHUDP_API_RT_SUCCESS)
	{
		// If Parameter are not proper return error.
		return retType;
	}
	
	// Create the singleton and call the add NH method.
	cdSuNhUdpInstMgr = CdSuNhUdpInstMgr::createNhUdpInstMgr ();
	// If creation success full then add udp NH instance.
	if (cdSuNhUdpInstMgr != NULL)
	{
		nhInstId = cdSuNhUdpInstMgr->addNhUdpInst (initParam);
		// If Nh Instance Id Invalid then overflow might have take place.
		if (nhInstId  == CDSU_NHUDP_INV_NHINSTID)
		{
			return CDSU_NHUDP_API_RT_INST_OVERFLOW;
		} // if (nhInstId  == CDSU_NHUDP_INV_INSTID)
		return CDSU_NHUDP_API_RT_SUCCESS;		
	} else
	{
		// Instance manager not created properly so return error.
		return (CDSU_NHUDP_API_RT_FAIL);
	}
	
} // CdSuNhUdpApiRetType cdSuNhUdpMiInit ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpMiTerminate ()
// Description  : Terminates the Network Handler Instance.
// Inputs       : TerminateNhParam 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiTerminate (const CdSuNhUdpMiTerminateParam& termParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retVal;
	// Check Instance Manager Exist.
	if (cdSuNhUdpInstMgr == NULL)
	{	
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}
	
	// Validate Parameters.	
	retVal = validateTerminateNhParam (termParam);
	if (retVal != CDSU_NHUDP_API_RT_SUCCESS)
	{
		// If Parameters are not proper then return error type
		return retVal;
	}
	
	// Get DB.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (termParam.nhUdpInstId);
	if (db != NULL)
	{
		// remove Instance.
		retVal = cdSuNhUdpInstMgr->removeNhUdpInst (termParam.nhUdpInstId);
		// delete instance Manager.
		if (retVal == CDSU_NHUDP_API_RT_SUCCESS)
		{
			// If it is the last NH instance then it returns NULL.	
			cdSuNhUdpInstMgr = CdSuNhUdpInstMgr::deleteNhUdpInstMgr ();
		}
		return (retVal);
	} else
	{
		// DB is null. Instance Id May be Invalid. 
		CDSU_TRACE (1, "cdSuNhUdpMiTerminate (): failed!\n");
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}
	
} // CdSuNhUdpApiRetType cdSuNhUdpMiTerminate ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpDelNhReq ()
// Description  : Deletes the Network Handler Instance.
// Inputs       : DelNhParam: 
// Return Value : 
// 		  instance.
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiDelNhReq (const CdSuNhUdpMiDelNhParam& delNhParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpApiRetType retType;
	
	// Check If Instance Manager Created 
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}
	
	// validate Parameters.
	retType = validateDelNhParam (delNhParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		// If parameter are not proper then return error type.
		return retType;
	}
	
	// Get DB.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (delNhParam.nhUdpInstId);
	if (db == NULL)
	{
		CDSU_TRACE1 (1, "cdSuNhUdpMiDelNhReq(): Invalid nhUdpInstId: %d\n", delNhParam.nhUdpInstId);
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}

	// Check if NH is in deleted state.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		// If NH is In Deleted state then invalid operation.
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}
	
	// Create the appropriate message to be sent.
	CdSuNhUdpMiDelNhReqMsg* delNhReqMsg =  CDSU_GETMSGBUF (CdSuNhUdpMiDelNhReqMsg);
	delNhReqMsg->initMsg (db->getNhActorId (), 
				db->getSmActorId (), 
				delNhParam);		
	// Send the message. 
	if (cdSuMdSendMsg (delNhReqMsg) == true)
	{
		return (CDSU_NHUDP_API_RT_SUCCESS);
	} else 
	{
		// Error.
		CDSU_TRACE (1, "cdSuNhUdpMiDelNhReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (delNhReqMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // CdSuNhUdpApiRetType cdSuNhUdpMiDelNhReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpMiCtrlReq ()
// Description  : Control Request for a Network Handler instance.
// Inputs       : CtrlReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiCtrlReq (const CdSuNhUdpMiCtrlReqParam& ctrlReqParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpApiRetType retType;
	
	// If Instance Manager Then Return Error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}

	// Validate Parameters.
	retType = validateCtrlReqParam (ctrlReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		// Parameters are invalid, return error.
		return retType;
	}
	
	// Get DB
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (ctrlReqParam.nhUdpInstId);
	if (db == NULL)
	{
		// DB Is Null. Instance Id May be invalid.
		CDSU_TRACE1 (1, "cdSuNhUdpMiCtrlReq(): Invalid nhUdpInstId: %d\n", ctrlReqParam.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}
	
	// If NH Is In Deleted state Then Requested operation Is Invalid.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		// Return Error.
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Build Message to Send to NH
	CdSuNhUdpMiCtrlReqMsg* ctrlMsg = CDSU_GETMSGBUF (CdSuNhUdpMiCtrlReqMsg); 
	ctrlMsg->initMsg (db->getNhActorId (), db->getSmActorId (), ctrlReqParam);		

	// Send Message TO NH
	if (cdSuMdSendMsg (ctrlMsg) == true)
	{
		return (CDSU_NHUDP_API_RT_SUCCESS);
	} else 
	{
		// Error in sending Message.
		CDSU_FREEMSGBUF (ctrlMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // CdSuNhUdpApiRetType cdSuNhUdpMiCtrlReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpMiCfgReq ()
// Description  : Config Request for a Network Handler instance.
// Inputs       : CfgReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiCfgReq (const CdSuNhUdpMiCfgReqParam& cfgReqParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpApiRetType retType;
	
	// If Instance Manager is NULL then not initialized.
	if (cdSuNhUdpInstMgr == NULL)
	{
		// return Error Type.
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL;
	}

	// Validate Parameters.
	retType = validateCfgReqParam (cfgReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		// Parameter are not valid so return error.
		return retType;
	}
	
	// Get Db.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (cfgReqParam.nhUdpInstId);
	if (db == NULL)
	{
		// Db Is NULL so return error.
		CDSU_TRACE1 (1, "cdSuNhUdpMiCfgReq(): Invalid nhUdpInstId: %d\n", cfgReqParam.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}

	// If NH is in deleted state then rquested service is invalid.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		// Return Error.
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Get a message buffer and fill it up.
	CdSuNhUdpMiCfgReqMsg* cfgMsg = CDSU_GETMSGBUF (CdSuNhUdpMiCfgReqMsg); 
	CdSuActorId dstActorId = db->getNhActorId ();
	cfgMsg->initMsg (dstActorId, db->getSmActorId (), cfgReqParam);		
	// Send Message.
	if (cdSuMdSendMsg (cfgMsg) == true)
	{
		return (CDSU_NHUDP_API_RT_SUCCESS);
	} else 
	{
		// Error.
		CDSU_FREEMSGBUF (cfgMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}
	
} // CdSuNhUdpApiRetType cdSuNhUdpMiCfgReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpMiStatus ()
// Description  : Status Request for a Network Handler instance.
// Inputs       : StatusReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiStatusReq (const CdSuNhUdpMiStatusReqParam& statusReqParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retType;
	
	// If Instance Manager is NULL return error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}

	// Validate Parameters.
	retType = validateStatusReqParam (statusReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		//Parameters are not valid, So Return Error.
		return retType;
	}

	// Get DB.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (statusReqParam.nhUdpInstId);
	if (db == NULL)
	{
		// DB Is NULL return Error.
		CDSU_TRACE1 (1, "cdSuNhUdpMiStatusReqParam(): Invalid nhUdpInstId: %d\n", statusReqParam.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}
	
	// If NH is IN Deleted State requested srevice is invalid return error.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Extract the Destination ModuleId. And Build Message.
	CdSuActorId dstActorId = db->getNhActorId ();
	CdSuNhUdpMiStatusReqMsg* statusMsg = CDSU_GETMSGBUF (CdSuNhUdpMiStatusReqMsg); 
	statusMsg->initMsg (dstActorId, db->getSmActorId (), statusReqParam);		
	// Send Message.
	if (cdSuMdSendMsg (statusMsg) == true)
	{
		return (CDSU_NHUDP_API_RT_SUCCESS);
	}
	else 
	{
		// Error.
		CDSU_FREEMSGBUF (statusMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // CdSuNhUdpApiRetType cdSuNhUdpMiStatusReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpMiStsReq ()
// Description  : Statistics Request for a Network Handler instance.
// Inputs       : StatusReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpMiStsReq (const CdSuNhUdpMiStsReqParam& stsReqParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retType;

	// If Instance Manager Is NULL then fail. return error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}

	// Valiadate Parameters.
	retType = validateStsReqParam (stsReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		return retType;
	}

	// Get DB.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (stsReqParam.nhUdpInstId);
	if (db == NULL)
	{
		// If DB is NULL then return error.
		CDSU_TRACE1 (1, "cdSuNhUdpMiStatusReqParam(): Invalid nhUdpInstId: %d\n", stsReqParam.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}
	
	// If NH is in Deleted state requestes service is invalid, if so return error.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Extract the Destination ModuleId. And Build Message.
	CdSuActorId dstActorId = db->getNhActorId ();
	CdSuNhUdpMiStsReqMsg* stsMsg = CDSU_GETMSGBUF (CdSuNhUdpMiStsReqMsg); 
	stsMsg->initMsg (dstActorId, db->getSmActorId (), stsReqParam);		
	// Send Message.
	if (cdSuMdSendMsg (stsMsg) == true)
	{
		return (CDSU_NHUDP_API_RT_SUCCESS);
	} else 
	{
		// Error.
		CDSU_FREEMSGBUF (stsMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // CdSuNhUdpApiRetType cdSuNhUdpMiStsReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpUiAddUdpPortReq ()
// Description  : 
// Inputs       : StatusReqParam.  
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpUiAddUdpPortReq (const CdSuNhUdpUiAddUdpPortReqParam& aupReqParam, CdSuNhUdpPortKey& nhUdpPortKey)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retType;
	nhUdpPortKey.nhUdpInstId = aupReqParam.nhUdpInstId;
	nhUdpPortKey.nhUdpPortId  = CDSU_NHUDP_INV_UDPPORTID;
	
	// If Instance Manager is NULL then fail, if so return error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL;
	}
	
	// Validate Parameter, if not valid then return error.
	retType = validateAddUdpPortReqParam (aupReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		return retType;
	}

	// Get the DB of the appropriate NH.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (aupReqParam.nhUdpInstId);
	if (db == NULL)
	{
		// IF Db is NULL instance id may be invalid.
		CDSU_TRACE1 (1, "cdSuNhUdpUiAddUdpPortReq (): Invalid nhUdpInstId: %d\n", 
		     aupReqParam.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}

	// If NH is In Deleted State then requested operation is invalid.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Allocate a UdpPort ID .
	nhUdpPortKey.nhUdpPortId = db->allocUdpPortId ();
	if (nhUdpPortKey.nhUdpPortId == CDSU_NHUDP_INV_UDPPORTID)
	{
		return (CDSU_NHUDP_API_RT_FAIL);
	}

	// Fill a message and mail it to the appropriate NH.
	CdSuNhUdpUiAddUdpPortReqMsg* addUdpPortReqMsg = CDSU_GETMSGBUF (CdSuNhUdpUiAddUdpPortReqMsg);	
	addUdpPortReqMsg->initMsg (db->getNhActorId (), 
				aupReqParam.applActorId, 
				aupReqParam,
				nhUdpPortKey.nhUdpPortId);		
	// Send Message
	if (cdSuMdSendMsg (addUdpPortReqMsg) == true)
	{
		return (CDSU_NHUDP_API_RT_SUCCESS);
	} else 
	{
		// Failure to mail. Deallocate msgBuf and LstnId.
		CDSU_TRACE (1, "cdSuNhUdpUiAddUdpPortReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (addUdpPortReqMsg);
		
		// Deallocate Listener Id.
		if (db->freeUdpPortId (	nhUdpPortKey.nhUdpPortId) == false)
		{
			CDSU_TRACE (1, "free UdpPort Id Id failed!\n");
		}
		nhUdpPortKey.nhUdpPortId = CDSU_NHUDP_INV_UDPPORTID;
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // CdSuNhUdpApiRetType cdSuNhUdpMiStsReq ()

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpUiRemUdpPortReq ()
// Description  : Removing Udp POrt.
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpUiRemUdpPortReq (const CdSuNhUdpUiRemUdpPortReqParam& rupReqParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retType;
	
	// If Instance Manager then return error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}

	// Validate Parameter. If parameter are not valid then return error.
	retType = validateRemUdpPortReqParam (rupReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		return retType;
	}
	
	// Get the DB of the appropriate NH.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (rupReqParam.udpPortKey.nhUdpInstId);
	if (db == NULL)
	{
		// If db is null then return error.
		CDSU_TRACE1 (1, "cdSuNhUdpUiRemUdpPortReq (): Invalid nhUdpInstId: %d\n", rupReqParam.udpPortKey.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}
	
	// If NH deleted then requested operation is invalid.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Build Message and Send a Request .
	CdSuNhUdpUiRemUdpPortReqMsg* remUdpPortReqMsg = CDSU_GETMSGBUF (CdSuNhUdpUiRemUdpPortReqMsg);	
	remUdpPortReqMsg->initMsg (db->getNhActorId (), 
				rupReqParam.applActorId, 
				rupReqParam);		
	// Send Message.
	if (cdSuMdSendMsg (remUdpPortReqMsg) == true)
	{
		return CDSU_NHUDP_API_RT_SUCCESS;	
	} else 
	{
		// Failure to mail. Deallocate msg.
		CDSU_TRACE (1, "cdSuNhUdpUiRemLstnReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (remUdpPortReqMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // CdSuNhUdpApiRetType cdSuNhUdpUiRemUdpPortReq

//---------------------------------------------------------------------------
// Name		: cdSuNhUdpUiDataReq ()
// Description  : Send Data Request.
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpApiRetType cdSuNhUdpUiDataReq (const CdSuNhUdpUiDataReqParam& dataReqParam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpApiRetType retType;

	// If Instance Manager is NULL return error.
	if (cdSuNhUdpInstMgr == NULL)
	{
		CDSU_TRACE (1, "Not initialized properly\n");
		return CDSU_NHUDP_API_RT_FAIL; 
	}

	// Validate Parameter. If parameter is not proper then return error.
	retType = validateDataReqParam (dataReqParam);
	if (retType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		return retType;
	}

	// Get the DB of the appropriate NH.
	CdSuNhUdpDb* db = cdSuNhUdpInstMgr->getNhUdpDb (dataReqParam.udpPortKey.nhUdpInstId);
	if (db == NULL)
	{
		// Db is NULL return Error.
		CDSU_TRACE1 (1, "cdSuNhUdpUiDataReq (): Invalid nhUdpInstId: %d\n", dataReqParam.udpPortKey.nhUdpInstId);
		return (CDSU_NHUDP_API_RT_INSTID_INV);
	}
	
	// If NH is in Deleted then requested service is not valid.
	if ((db->getNhUdpPtr())->isDeleted ())
	{
		CDSU_TRACE (1, "NH Is In Deleted state.\n");
		return CDSU_NHUDP_API_RT_OP_INV;
	}

	// Build Message and Send a Request 
	CdSuNhUdpUiDataReqMsg* dataReqMsg = CDSU_GETMSGBUF (CdSuNhUdpUiDataReqMsg);	
	dataReqMsg->initMsg (db->getNhActorId (), 
				dataReqParam.applActorId, 
				dataReqParam);		
	// Send Message.
	if (cdSuMdSendMsg (dataReqMsg) == true)
	{
		return CDSU_NHUDP_API_RT_SUCCESS;	
	} else 
	{
		// Failure to mail. Deallocate msg.
		CDSU_TRACE (1, "cdSuNhUdpUiDataReq (): cdSuMdSendMsg failed!\n");
		CDSU_FREEMSGBUF (dataReqMsg);
		return (CDSU_NHUDP_API_RT_FAIL);
	}

} // bool cdSuNhUdpUiRemUdpPortReq

//============================================================================
// <End of cdsunhudpapi.cpp >
//============================================================================
