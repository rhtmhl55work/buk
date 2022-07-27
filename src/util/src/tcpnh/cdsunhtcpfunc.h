#ifndef __CDSU_NHTCP_FUNC_H
#define __CDSU_NHTCP_FUNC_H
//============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhtcpfunc.h
// Description  : Function declaration.
// Author       : Seema Jain
// Created On   : 4th Feb, 2004
// Last Modified: By Sumit Bijwe  on Wed Feb 25 10:21:38 IST 2004
//               + Added cdSuNhTcpMiTerminate () API.
//		  By Sumit Bijwe  on Wed Feb 25 10:21:38 IST 2004
//               + Added CdSuNhTcpApiRetType for all API.
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//============================================================================

//----------------------------------------------------------------------------
// Network Handler System Management Interface:: NHSMI 
//----------------------------------------------------------------------------

// SM -> NH.

//----------------------------------------------------------------------------
// API provided to the System Management to create, configure, control,
// delete and get status and statistics from the stack. 
//----------------------------------------------------------------------------

CdSuNhTcpApiRetType cdSuNhTcpMiInit (const CdSuNhTcpMiInitParam& initParam, CdSuNhTcpInstId& nhid);
CdSuNhTcpApiRetType cdSuNhTcpMiDelNhReq (const CdSuNhTcpMiDelNhParam& delNhParam);
CdSuNhTcpApiRetType cdSuNhTcpMiTerminate (const CdSuNhTcpMiTerminateParam& termParam);
CdSuNhTcpApiRetType cdSuNhTcpMiCtrlReq (const CdSuNhTcpMiCtrlReqParam& p);
CdSuNhTcpApiRetType cdSuNhTcpMiCfgReq (const CdSuNhTcpMiCfgReqParam& p);
CdSuNhTcpApiRetType cdSuNhTcpMiStatusReq (const CdSuNhTcpMiStatusReqParam& p);
CdSuNhTcpApiRetType cdSuNhTcpMiStsReq (const CdSuNhTcpMiStsReqParam& p);
bool cdSuNhTcpDefSmEventHlr (const CdSuNhTcpMiEvent& p);

//----------------------------------------------------------------------------
// Network Handler Data interface.
//----------------------------------------------------------------------------

// Appl -> NH

CdSuNhTcpApiRetType cdSuNhTcpUiAddLstnReq (const CdSuNhTcpUiAddLstnReqParam& p, CdSuNhTcpLstnKey& lkey);
CdSuNhTcpApiRetType cdSuNhTcpUiRemLstnReq (const CdSuNhTcpUiRemLstnReqParam& p);
CdSuNhTcpApiRetType cdSuNhTcpUiConReq (const CdSuNhTcpUiConReqParam& p, CdSuNhTcpConnKey& ckey);
CdSuNhTcpApiRetType cdSuNhTcpUiCloseConReq (const CdSuNhTcpUiCloseConReqParam& p);
CdSuNhTcpApiRetType cdSuNhTcpUiDataReq (const CdSuNhTcpUiDataReqParam& p);

// NH -> Appl (Default Event Handlers)
bool cdSuNhTcpDefLstnEventHlr (const CdSuNhTcpUiLstnEvent& p);
bool cdSuNhTcpDefConEventHlr (const CdSuNhTcpUiConnEvent& p);

//============================================================================
// <End of cdsunhtcpfunc.h>
//============================================================================
#endif
