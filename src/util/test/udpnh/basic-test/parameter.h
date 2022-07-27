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
#include "cdsunhudpapi.h"


#define ISMIMSGINTF true
#define ISUIMSGINTF true
#define DOREPLY	    false

#define INVALMODID 	200
#define INVALNHINSTID 	100
#define INVALUDPPORTID 100
#define INVALBUFFERLEN 	0
#define INVALDATAPTR 	0
#define LOCALPORT	6790
#define EXTRALOCALPORT  5000 // 8000
#define DESTPORT	7060
#define EXTRADESTPORT	8000 //5000	// For testing connected udp port. To make sure
				// that udp port doesn't receive data from 
				// Different port .

char localIpAddr [4] = { 192, 168, 16, 72 };
char diffLocalIpAddr [4] = { 192, 168, 16, 72 };

char remoteIpAddr [4] = {192, 168, 16, 72};
char diffRemoteIpAddr [4] = {192, 168, 16,72 };


CdSuActorId nhModId;		// Network handler Actor ID.

CdSuActorId smModId;		// Sys Mgmt Actor ID.

CdSuActorId applnModId;		// Application Actor ID.

CdSuNhUdpInstId nhInstID;	// UDP NH Instance ID.

CdSuNhUdpPortKey nhUdpPortKey;	// Udp Port Key.

CdSuNhUdpSubOpcode subOpCode;

bool useInvNhInstId = false; 	// flag.
bool useMoreThanMaxNhInstId = false;
bool useInvApplModId = false;	// flag.
bool useInvSmModId = false;		// flag.
bool useInvNhModId = false;	// Flag.
bool useZeroSelTimer 	= false;
bool useMoreThanMaxSelTimer = false;
bool useZeroNoHlrTimer = false;
bool useMoreThanMaxNoHlrTimer = false;
bool useZeroBufferLen 	= false; // flag.
bool useMoreThanMaxBufferLen = false;
bool useMoreThanMaxUdpPort = false;
bool useInvPortId = false; 	// flag.
bool useInvDataPtr = false;
bool useZeroDataLen  = false;
bool useDiffLocalAddr = false;
bool useDiffDestAddr = false;
bool useLocalAddrAsDestAddr = false;
bool useAllOpenedUdpPort = false;
bool noAckReq = false;
enum LocalPortType
{
	EPHERMAL = 0,
	STANDARD,
	DIF_LCL_PORT,
	LOCAL_DEFAULT,
} ;

enum DestPortType
{
	ZERO = 0,
	VALID,
	DIF_DEST_PORT,
	SELF_PORT,
	DEST_DEFAULT
} ;
LocalPortType localPortType = STANDARD;
DestPortType destPortType = DEST_DEFAULT;

CdSuIpv4UdpPortNo localUdpPort = 6790;
CdSuIpv4UdpPortNo remoteUdpPort = 7060;
CdSuIpv4UdpPortNo dataRcvdFromPortNo;
CdSuIpv4Addr dataRcvdFromAddr;
char* rcvdData = NULL;

int counter = -1;
const int maxCounter = 5;
char data [maxCounter] [100] = 
			{
				"Vijaya Kumar",
				"UDP NH built by CDOT Bangalore",
				"Internet Telephony Group Bangalore",
				"Udupi is beautiful place",
				"CDOT Delhi is in Delhi"
			};

	
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
	"CDSU_NHUDP_DATAREQ_ALWOP_CTR",

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
	"CDSU_NHUDP_SUCCESS_DATAREQ_ALWOP_CTR",

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

// Counter for statistics
int statCounter [CDSU_NHUDP_MAX_CTR];
int statCounterOfNh [CDSU_NHUDP_MAX_CTR];

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
CdSuNhUdpUiSendReqParam sendReqParam;

//============================================================================
// <End of parameter.h>
//============================================================================
#endif

