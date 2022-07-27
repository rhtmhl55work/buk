#ifndef __CDSU_NHUDP_APIPARAM_CHK
#define __CDSU_NHUDP_APIPARAM_CHK
//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpapiparamchk.h
// Description	: It contains the API parameter validation functions.
// Author       : Vijaya Kumar
// Created On   : Fri Mar 26 11:31:04 IST 2004
// Last Modified: 
//============================================================================

//----------------------------------------------------------------------------
// Include files.
//----------------------------------------------------------------------------
#include <stdio.h>
#include "cdsunhudpapiparam.h"
#include "cdsunhudptypes.h"

//----------------------------------------------------------------------------
// Parameter Validating Functions. Each Function validates the parameter
// and returns error return type. If parameters are valid then it returns 
// succes.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Following Functions Validates parameter to MI interface Functions.
//----------------------------------------------------------------------------
CdSuNhUdpApiRetType validateInitNhParam (const CdSuNhUdpMiInitParam& p);
CdSuNhUdpApiRetType validateDelNhParam (const CdSuNhUdpMiDelNhParam& p);
CdSuNhUdpApiRetType validateTerminateNhParam (const CdSuNhUdpMiTerminateParam& p);
CdSuNhUdpApiRetType validateCtrlReqParam (const CdSuNhUdpMiCtrlReqParam& p);
CdSuNhUdpApiRetType validateCfgReqParam (const CdSuNhUdpMiCfgReqParam& p);
CdSuNhUdpApiRetType validateStatusReqParam (const CdSuNhUdpMiStatusReqParam& p);
CdSuNhUdpApiRetType validateStsReqParam (const CdSuNhUdpMiStsReqParam& p);

//----------------------------------------------------------------------------
// Following Functions Validates parameter to UI interface Functions.
//----------------------------------------------------------------------------
CdSuNhUdpApiRetType validateAddUdpPortReqParam (const CdSuNhUdpUiAddUdpPortReqParam& p);
CdSuNhUdpApiRetType validateRemUdpPortReqParam (const CdSuNhUdpUiRemUdpPortReqParam& p);
CdSuNhUdpApiRetType validateDataReqParam (const CdSuNhUdpUiDataReqParam& p);
CdSuNhUdpApiRetType validateSendReqParam (const CdSuNhUdpUiSendReqParam& p);

//============================================================================
// <End of cdsunhudpapiparamchk.h>
//============================================================================
#endif
