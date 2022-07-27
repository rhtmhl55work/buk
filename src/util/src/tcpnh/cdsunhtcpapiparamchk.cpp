//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpapiparamchk.cpp
// Description	: It contains the API parameter validation functions.
// Author       : Sumit Bijwe
// Created On   : Thu Mar 25 14:35:17 IST 2004
// Last Modified: 
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
#include "cdsunhtcpinstmgr.h"
#include "cdsunhtcpmsg.h"
#include "cdsunhtcpfunc.h"
#include "cdsunhtcpapiparamchk.h"

//---------------------------------------------------------------------------
// Name		: validateInitNhParam ()
// Description  : 
// Inputs       : NhTcp Init Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateInitNhParam (const CdSuNhTcpMiInitParam& initparam)
{
	//if (initparam.maxNoOfConnections)		
	//if (initparam.maxNoOfLstns)
	//if (initparam.maxBufferLen)			
	//if (initparam.listenSelectTimer)
	//if (initparam.dataSelectTimer)
	//if (initparam.noHandleTimer)			
	//if (initparam.listenQSize)			
	return (CDSU_NHTCP_API_RT_SUCCESS);
}

//---------------------------------------------------------------------------
// Name		: validateDelNhParam ()
// Description  : 
// Inputs       : Delete NH Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateDelNhParam (const CdSuNhTcpMiDelNhParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateTerminateNhParam ()
// Description  : 
// Inputs       : Terminate NH Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateTerminateNhParam (const CdSuNhTcpMiTerminateParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateCtrlReqParam()
// Description  : 
// Inputs       : Ctrl Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateCtrlReqParam (const CdSuNhTcpMiCtrlReqParam& p)
{
	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateCfgReqParam ()
// Description  : 
// Inputs       : Config Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateCfgReqParam (const CdSuNhTcpMiCfgReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateStatusReqParam ()
// Description  : 
// Inputs       : Status Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateStatusReqParam (const CdSuNhTcpMiStatusReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateStsReqParam ()
// Description  : 
// Inputs       : Statistcs Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateStsReqParam (const CdSuNhTcpMiStsReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}
	
//---------------------------------------------------------------------------
// Name		: validateAddLstnReqParam ()
// Description  : 
// Inputs       : Add Listener Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateAddLstnReqParam (const CdSuNhTcpUiAddLstnReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateRemLstnReqParam ()
// Description  : 
// Inputs       : Remove Listener Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateRemLstnReqParam (const CdSuNhTcpUiRemLstnReqParam& p)
{
	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.lstnKey.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateConReqParam ()
// Description  : 
// Inputs       : Connection Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateConReqParam (const CdSuNhTcpUiConReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateCloseConReqParam ()
// Description  : 
// Inputs       : Close Con Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateCloseConReqParam (const CdSuNhTcpUiCloseConReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.connKey.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: validateDataReqParam()
// Description  : 
// Inputs       : Data Req Param.
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpApiRetType validateDataReqParam (const CdSuNhTcpUiDataReqParam& p)
{

	CdSuNhTcpApiRetType retType = chkNhInstIdRng (p.connKey.nhTcpInstId);
	return (retType);
}

//---------------------------------------------------------------------------
// Name		: chkNhInstIdRng ()
// Description  : It checks NH Inst Id within range (0-128). 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------


CdSuNhTcpApiRetType chkNhInstIdRng (CdSuNhTcpInstId nid)
{
	if (nid < CDSU_NHTCP_MAX_INST)
	{
		return (CDSU_NHTCP_API_RT_SUCCESS);
	} else
	{
		CDSU_TRACE (1, "NH Instance Id is out of range \n");
		return (CDSU_NHTCP_API_RT_INVINSTID);
	}

}
//----------------------------------------------------------------------------
// <End of cdsunhtcpapiparamchk.cpp>
//----------------------------------------------------------------------------
