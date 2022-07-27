#ifndef __CDSU_NHTCP_APIPARAM_CHK
#define __CDSU_NHTCP_APIPARAM_CHK
//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcpapiparamchk.h
// Description	: It contains the API parameter validation functions.
// Author       : Sumit Bijwe
// Created On   : Thu Mar 25 14:35:17 IST 2004
// Last Modified: 
//============================================================================

CdSuNhTcpApiRetType validateInitNhParam (const CdSuNhTcpMiInitParam& p);
CdSuNhTcpApiRetType validateDelNhParam (const CdSuNhTcpMiDelNhParam& p);
CdSuNhTcpApiRetType validateTerminateNhParam (const CdSuNhTcpMiTerminateParam& p);
CdSuNhTcpApiRetType validateCtrlReqParam (const CdSuNhTcpMiCtrlReqParam& p);
CdSuNhTcpApiRetType validateCfgReqParam (const CdSuNhTcpMiCfgReqParam& p);
CdSuNhTcpApiRetType validateStatusReqParam (const CdSuNhTcpMiStatusReqParam& p);
CdSuNhTcpApiRetType validateStsReqParam (const CdSuNhTcpMiStsReqParam& p);

CdSuNhTcpApiRetType validateAddLstnReqParam (const CdSuNhTcpUiAddLstnReqParam& p);
CdSuNhTcpApiRetType validateRemLstnReqParam (const CdSuNhTcpUiRemLstnReqParam& p);
CdSuNhTcpApiRetType validateConReqParam (const CdSuNhTcpUiConReqParam& p);
CdSuNhTcpApiRetType validateCloseConReqParam (const CdSuNhTcpUiCloseConReqParam& p);
CdSuNhTcpApiRetType validateDataReqParam (const CdSuNhTcpUiDataReqParam& p);

CdSuNhTcpApiRetType chkNhInstIdRng (CdSuNhTcpInstId nid);

//----------------------------------------------------------------------------
// <End of cdsunhtcpapiparamchk.h>
//----------------------------------------------------------------------------
#endif
