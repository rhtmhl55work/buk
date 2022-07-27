#ifndef PARAMETER_H_
#define PARAMETER_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : parameter.h
// Description  : Contains parameter like server portnumber, client portnumber
// 		: etc. 
// Author       : Vijaya Kumar
// Created On   : Thu Feb 12 17:07:38 IST 2004
// Last Modified:
//============================================================================

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "cdsunhudpapi.h"

// Following are configurable values.
// You can change the value 

#define MAXPORT	25
#define REMOTEMAXPORT 25 // Now set MAXPORT = REMOTEMAXPORT = X,
#define MAXINST 1
#define SENDATTEMPT 16
#define MSGDISTUDPPORT 8005


// Change Following Address acording to setup
char localIpAddr [4] = { 192, 168, 16, 72 };
char remoteIpAddr [4] = {192, 168, 51, 215 };
bool doReply = false;
int delay = 20000000; //20 microsecond



// Dont Touch Following

bool isMiMsgIntf = false;
bool isUiMsgIntf = false;
bool isLoadSharedMode = false;
bool isRemote = false;
bool isSelf = false;
bool useDelay = false;

int maxPortPerInst = (MAXPORT / MAXINST); 
int startingLocalPort = 6000;
int startingDestPort = startingLocalPort + MAXPORT;
int setOfLocalPort [ MAXPORT ];
int setOfRemotePort [ REMOTEMAXPORT ];
int portId2Index [ 50000 ];
int instId2Index [ 256 ];
int noOfOpenedPort;

CdSuActorId nhModId [MAXINST];		// Network handler Actor ID.
CdSuActorId smModId;		// Sys Mgmt Actor ID.
CdSuActorId applnModId;		// Application Actor ID.
CdSuNhUdpInstId nhInstID [MAXINST];	// UDP NH Instance ID.

bool isValidPortKey [ MAXPORT ] ;
bool isValidInstId [ 256 ];

CdSuNhUdpPortKey nhUdpPortKey [ MAXPORT ];	// Udp Port Key.

char data [100] = "DATA...................................";
char tempData [100];

// Statistics
int noOfInit = 0;
int noOfStart = 0;
int noOfInitRsp = 0;
int noOfStartRsp = 0;
int noOfStop = 0;
int noOfStopRsp = 0;
int noOfDel =0;
int noOfDelRsp = 0;
int noOfTerm = 0;
int noOfAddUdpPort = 0;
int noOfAddUdpPortRsp = 0;
int noOfDataSent = 0;
int noOfDataSentSuccess = 0;
int noOfDataRsp = 0;
int noOfDataFailRsp = 0;
int noOfDataRecvd = 0;
int noOfUdpPortRem = 0;
int noOfUdpPortRemRsp = 0;

CdSuNhUdpSubOpcode subOpCode;

char counterString [][100] = {
	"CDSU_NHUDP_ADD_UDPPORT_CTR",
	"CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_CTR",
	"CDSU_NHUDP_ADD_UDPPORT_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_ADD_UDPPORT_FIXED_DST_CTR",
	"CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_EPH_CTR",
	
	"CDSU_NHUDP_DATAREQ_CTR",
	"CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR",
	"CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_DATAREQ_FIXED_DST_CTR",
	"CDSU_NHUDP_DATAREQ_NOTFIXED_CTR",
	"CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR",
	
	"CDSU_NHUDP_REM_UDPPORT_CTR",
	"CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_CTR",
	"CDSU_NHUDP_REM_UDPPORT_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_REM_UDPPORT_FIXED_DST_CTR",
	"CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_EPH_CTR",

	"CDSU_NHUDP_DATAIND_CTR",
	"CDSU_NHUDP_DATAIND_FIXED_LCL_CTR",
	"CDSU_NHUDP_DATAIND_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_DATAIND_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_DATAIND_FIXED_LCL_EPH_CTR",
	"CDSU_NHUDP_DATAIND_ERROR_CTR",

	"CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR",
	"CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_CTR",
	"CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_DST_CTR",
	"CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_EPH_CTR",
	
	"CDSU_NHUDP_SUCCESS_DATAREQ_CTR",
	"CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_CTR",
	"CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_DST_CTR",
	"CDSU_NHUDP_SUCCESS_DATAREQ_NOTFIXED_CTR",
	"CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_EPH_CTR",
	
	"CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR",
	"CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_CTR",
	"CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCLDST_CTR",
	"CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_DST_CTR",
	"CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_ST_CTR",
	"CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_EPH_CTR",

	"CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR",
	"CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR",
	"CDSU_NHUDP_MAX_CTR"
};

char failCodeString  [] [100] =
{
	"",
	"CDSU_NHUDP_RSPFCODE_INVPARAM",
	"CDSU_NHUDP_RSPFCODE_INV_OP",
	"CDSU_NHUDP_RSPFCODE_UDPPORT_OVERFLOW",
	"CDSU_NHUDP_RSPFCODE_BINDERROR",
	"CDSU_NHUDP_RSPFCODE_CONNECTERROR",
	"CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST",
	"CDSU_NHUDP_RSPFCODE_UNKNOWNERROR",
	"CDSU_NHUDP_RSPFCODE_SENDERROR",
	"CDSU_NHUDP_RSPFCODE_OPENERROR",
	"CDSU_NHUDP_RSPFCODE_READERROR",
	"CDSU_NHUDP_RSPFCODE_INVALID"
} ;

char apiRtTypeString  [] [100] =
{
	"CDSU_NHUDP_API_RT_SUCCESS",
	"CDSU_NHUDP_API_RT_INSTID_INV",
	"CDSU_NHUDP_API_RT_INST_OVERFLOW",
	"CDSU_NHUDP_API_RT_OP_INV",
	"CDSU_NHUDP_API_RT_FAIL",
	"CDSU_NHUDP_API_RT_MAXPORT_OUTOFRNG",
	"CDSU_NHUDP_API_RT_MAXBUFLEN_OUTOFRNG",
	"CDSU_NHUDP_API_RT_SELTMRVAL_OUTOFRNG",
	"CDSU_NHUDP_API_RT_NOHLRTMRVAL_OUTOFRNG",
       	"CDSU_NHUDP_API_RT_OPCODE_INV",
	"CDSU_NHUDP_API_RT_SUBOPCODE_INV",
	"CDSU_NHUDP_API_RT_PORTID_INV",
	"CDSU_NHUDP_API_RT_DATAPTR_INV",
	"CDSU_NHUDP_API_RT_DESTPORT_INV",
	"CDSU_NHUDP_API_RT_DATALEN_INV",
	"CDSU_NHUDP_API_RT_DATALEN_OUTOFRNG",
} ;

// Parameter 
CdSuNhUdpMiInitParam initParam;
CdSuNhUdpMiTerminateParam terminateParam;
CdSuNhUdpMiDelNhParam delnhparam;
CdSuNhUdpMiCtrlReqParam ctrlReqParam;
CdSuNhUdpMiCfgReqParam cfgParam;
CdSuNhUdpMiStatusReqParam statusReqParam;
CdSuNhUdpMiStsReqParam statsReqParam;

CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;
CdSuNhUdpUiRemUdpPortReqParam remUdpPortReqParam;
CdSuNhUdpUiDataReqParam sendDataReqParam;

CdSuNhUdpApiRetType rtType;

bool useZeroSelTimer = false;

long int timegap;
time_t recordedTime;

//============================================================================
// <End of parameter.h>
//============================================================================
#endif
