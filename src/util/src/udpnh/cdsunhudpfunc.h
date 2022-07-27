#ifndef __CDSU_NHUDPFUNC_H__
#define __CDSU_NHUDPFUNC_H__

//============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhudpapi.h
// Description  : API for the Generic UDP Network Handler.
// Author       : Vijaya Kumar 
// Created On   : Mon Mar  8 21:08:23 IST 2004
// Last Modified: 
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//============================================================================

//----------------------------------------------------------------------------
// Includes 
//----------------------------------------------------------------------------
#include "cdsunhudptypes.h"
#include "cdsunhudpapiparam.h"
//----------------------------------------------------------------------------
// Network Handler System Management Interface:: NHSMI 
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//			 SM -> NH Interface Function
//			 ---------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// API provided to the System Management to create, configure, control,
// delete and get status and statistics from the stack. 
//----------------------------------------------------------------------------

// This function is used to create NH instance. But first time when this
// function is called it creates NH Instance Manager.  
CdSuNhUdpApiRetType cdSuNhUdpMiInit (const CdSuNhUdpMiInitParam& initParam, CdSuNhUdpInstId& nhUdpInstId);

// This function is used to stop UDP Network Handler instance.
CdSuNhUdpApiRetType cdSuNhUdpMiDelNhReq (const CdSuNhUdpMiDelNhParam& delNhParam);

// This function is used to destroy UDP Network Handler instance.
// It destroys UDP network handler Manager when it deletes last UDP Network Handler
// instance
CdSuNhUdpApiRetType cdSuNhUdpMiTerminate (const CdSuNhUdpMiTerminateParam& termParam);

// This function is used to do control related operation on UDP NH. Control
// related operation includes creating UDP NH, starting UDP NH,
// stoping UDP NH, deleting UDP NH.
CdSuNhUdpApiRetType cdSuNhUdpMiCtrlReq (const CdSuNhUdpMiCtrlReqParam& ctrlParam);

// This function is used to configure UDP NH. 
CdSuNhUdpApiRetType cdSuNhUdpMiCfgReq (const CdSuNhUdpMiCfgReqParam& cfgParam);

// This function is used to request status of UDP NH.
CdSuNhUdpApiRetType cdSuNhUdpMiStatusReq (const CdSuNhUdpMiStatusReqParam& statusParam);

// This function is used to request statistic information.
CdSuNhUdpApiRetType cdSuNhUdpMiStsReq (const CdSuNhUdpMiStsReqParam& stsParam);


//----------------------------------------------------------------------------
// 				Appl -> NH Interface Function
// 				-----------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// API provided to the Application to add,  remove udp port, send data,
// receiving data indication.
//----------------------------------------------------------------------------

// Add Udp Port.
// This function is used to open udp port.
CdSuNhUdpApiRetType cdSuNhUdpUiAddUdpPortReq (const CdSuNhUdpUiAddUdpPortReqParam&, CdSuNhUdpPortKey& nhUdpPortKey);

// This function is used to close udp port opened.
CdSuNhUdpApiRetType cdSuNhUdpUiRemUdpPortReq (const CdSuNhUdpUiRemUdpPortReqParam&);

// This function is used to send data over udp port. While sending data
// application can send opaque data. NH doesn't care about opaque data,
// it is up to application make use of it. 
CdSuNhUdpApiRetType cdSuNhUdpUiDataReq (const CdSuNhUdpUiDataReqParam&);

//============================================================================
// <End of cdsunhtcpfunc.h>
//============================================================================
#endif
