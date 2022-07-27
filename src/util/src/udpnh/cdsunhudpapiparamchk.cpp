//============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhudpapiparamchk.h
// Description  : Contains function for validating the api parameter.
// Author       : Vijaya Kumar
// Created On   : Fri Mar 26 12:33:58 IST 2004
// Last Modified: 
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//		  Unix Network Programming by W. Rechard Steavens	
//============================================================================

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "cdsunhudpapiparamchk.h"
#include "cdsutrace.h"
#include <stdlib.h>
//----------------------------------------------------------------------------
// Function	: validateInstId ()
// Description	: Validates Nh Instance ID.
// Inputs	: NH Instance ID
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateInstId (CdSuNhUdpInstId instId)
{
	if (instId >= CDSU_NHUDP_MAX_INST)
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}	
	
	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateInstId ()

//----------------------------------------------------------------------------
// Function	: validateInitNhParam ()
// Description	: Validates the Init NH Param.
// Inputs	: CdSuNhUdpMiInitParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateInitNhParam (const CdSuNhUdpMiInitParam& p)
{
	if (p.maxBufferLen == 0)
	{
		return CDSU_NHUDP_API_RT_MAXBUFLEN_OUTOFRNG;
	}
	
     	if (p.dataSelectTimer == 0)	
	{
		return CDSU_NHUDP_API_RT_SELTMRVAL_OUTOFRNG;
	}
       
	if  (p.noHandleTimer == 0)
	{
		return 	CDSU_NHUDP_API_RT_NOHLRTMRVAL_OUTOFRNG;
	}	
	if (p.maxNoOfUdpPort > CDSU_NHUDP_MAX_UDPPORT)
	{
		return CDSU_NHUDP_API_RT_MAXPORT_OUTOFRNG;
	}
	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateInitNhParam ()

//----------------------------------------------------------------------------
// Function	: validateDelNhParam ()
// Description	: Validates the Delete NH Param.
// Inputs	: CdSuNhUdpMiDelNhParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateDelNhParam (const CdSuNhUdpMiDelNhParam& p)
{
	return validateInstId (p.nhUdpInstId);

} // CdSuNhUdpApiRetType validateDelNhParam ()

//----------------------------------------------------------------------------
// Function	: validateTerminateNhParam ()
// Description	: Validates the Terminate NH Param.
// Inputs	: CdSuNhUdpMiTerminateParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateTerminateNhParam (const CdSuNhUdpMiTerminateParam& p)
{
	return validateInstId (p.nhUdpInstId);

} // CdSuNhUdpApiRetType validateTerminateNhParam ()

//----------------------------------------------------------------------------
// Function	: validateCtrlReqParam ()
// Description	: Validates the Ctrls NH Param.
// Inputs	: CdSuNhUdpMiTerminateParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateCtrlReqParam (const CdSuNhUdpMiCtrlReqParam& p)
{
	if (validateInstId (p.nhUdpInstId) == CDSU_NHUDP_API_RT_INSTID_INV)
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}	
	
	if (! (p.subOpcode ==CDSU_NHUDP_SO_CRE_REQ ||	
		p.subOpcode ==	CDSU_NHUDP_SO_DEL_REQ ||
		p.subOpcode == CDSU_NHUDP_SO_START_REQ ||
		p.subOpcode == CDSU_NHUDP_SO_STOP_REQ ))
	{
		return CDSU_NHUDP_API_RT_SUBOPCODE_INV;
	}

	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateCtrlReqParam ()

//----------------------------------------------------------------------------
// Function	: validateCfgReqParam ()
// Description	: Validates the Config NH Param.
// Inputs	: CdSuNhUdpMiTerminateParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateCfgReqParam (const CdSuNhUdpMiCfgReqParam& p)
{
	if (validateInstId (p.nhUdpInstId) == CDSU_NHUDP_API_RT_INSTID_INV)
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}	
	
	if (! (p.subOpcode == CDSU_NHUDP_SO_INITIAL_CFG_REQ ||	
		p.subOpcode ==	CDSU_NHUDP_SO_GENERAL_CFG_REQ ))
	{
		return CDSU_NHUDP_API_RT_SUBOPCODE_INV;
	}

	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateCfgReqParam ()

//----------------------------------------------------------------------------
// Function	: validateStatusReqParam ()
// Description	: Validates the Status Request Param.
// Inputs	: CdSuNhUdpMiStatusReqParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateStatusReqParam (const CdSuNhUdpMiStatusReqParam& p)
{
	return validateInstId (p.nhUdpInstId);	

} // CdSuNhUdpApiRetType validateStatusReqParam ()

//----------------------------------------------------------------------------
// Function	: validateStatusReqParam ()
// Description	: Validates the Status Request Param.
// Inputs	: CdSuNhUdpMiStatusReqParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateStsReqParam (const CdSuNhUdpMiStsReqParam& p)
{
	return validateInstId (p.nhUdpInstId);	

} // CdSuNhUdpApiRetType validateStsReqParam ()

//----------------------------------------------------------------------------
// Function	: validateAddUdpPortReqParam ()
// Description	: Validates the Add Udp Port Req Param.
// Inputs	: CdSuNhUdpUiAddUdpPortReqParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateAddUdpPortReqParam (const CdSuNhUdpUiAddUdpPortReqParam& p)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	if (validateInstId (p.nhUdpInstId) == CDSU_NHUDP_API_RT_INSTID_INV)
	{
		CDSU_TRACE (1, "CDSU_NHUDP_API_RT_INSTID_INV \n");
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}
	
	if (p.destPortNo == 0)
	{
		CDSU_TRACE (1, "CDSU_NHUDP_API_RT_DESTPORT_INV \n");
		return CDSU_NHUDP_API_RT_DESTPORT_INV;
	}
	
	return CDSU_NHUDP_API_RT_SUCCESS;
	
} // CdSuNhUdpApiRetType validateAddUdpPortReqParam ()

//----------------------------------------------------------------------------
// Function	: validateRemUdpPortReqParam ()
// Description	: Validates the Rem Udp Port Req Param.
// Inputs	: CdSuNhUdpUiRemUdpPortReqParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateRemUdpPortReqParam (const CdSuNhUdpUiRemUdpPortReqParam& p)
{
	if (validateInstId (p.udpPortKey.nhUdpInstId) == CDSU_NHUDP_API_RT_INSTID_INV)
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}
	
	if (p.udpPortKey.nhUdpPortId >= CDSU_NHUDP_INV_UDPPORTID)
	{
		return CDSU_NHUDP_API_RT_PORTID_INV;
	}
	
	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateRemUdpPortReqParam ()

//----------------------------------------------------------------------------
// Function	: validateDataReqParam ()
// Description	: Validates the Data Req Param.
// Inputs	: CdSuNhUdpUiDataReqParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateDataReqParam (const CdSuNhUdpUiDataReqParam& p)
{

	if (validateInstId (p.udpPortKey.nhUdpInstId) == CDSU_NHUDP_API_RT_INSTID_INV)
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}
	
	if (p.udpPortKey.nhUdpPortId >= CDSU_NHUDP_INV_UDPPORTID)
	{
		return CDSU_NHUDP_API_RT_PORTID_INV;
	}

	if (p.dataPtr == NULL)
	{
		return CDSU_NHUDP_API_RT_DATAPTR_INV;
	}
	
	if (p.dataLen == 0)
	{
		return CDSU_NHUDP_API_RT_DATALEN_INV;
	}
	
	if (p.destPortNo == 0)
	{
	
		return CDSU_NHUDP_API_RT_DESTPORT_INV;
	} 
	
	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateDataReqParam ()

//----------------------------------------------------------------------------
// Function	: validateSendReqParam ()
// Description	: Validates the Send Data Req Param.
// Inputs	: CdSuNhUdpUiSendReqParam
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

CdSuNhUdpApiRetType validateSendReqParam (const CdSuNhUdpUiSendReqParam& p)
{

	if (validateInstId (p.nhUdpInstId) == CDSU_NHUDP_API_RT_INSTID_INV)
	{
		return CDSU_NHUDP_API_RT_INSTID_INV;
	}
	
	if (p.dataPtr == NULL)
	{
		return CDSU_NHUDP_API_RT_DATAPTR_INV;
	}
	
	if (p.dataLen == 0)
	{
		return CDSU_NHUDP_API_RT_DATALEN_INV;
	}
	
	if (p.destPortNo == 0 || p.destPortNo == CDSU_NHUDP_INV_UDPPORT)
	{
	
		return CDSU_NHUDP_API_RT_DESTPORT_INV;
	} 
	
	return CDSU_NHUDP_API_RT_SUCCESS;

} // CdSuNhUdpApiRetType validateDataReqParam ()

//============================================================================
// <End of cdsunhudpparamchk.cpp>
//============================================================================


