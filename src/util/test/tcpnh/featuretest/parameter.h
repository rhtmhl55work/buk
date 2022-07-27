#ifndef PARAMETER_H_
#define PARAMETER_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : parameter.h
// Description  : Contains parameter like server portnumber, client portnumber
// 		: etc. 
// Author       : Sumit Bijwe
// Created On   : Wed Mar 24 14:47:20 IST 2004
// Last Modified:
//============================================================================

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cdsunhudpapi.h"



#define MAX_NO_CONN 500
#define MAX_NO_LSTN 100

#define ISMIMSGINTF true
#define ISUIMSGINTF false
#define INVALMODID 	200
#define INVALNHINSTID 	200
#define INVALBUFFERLEN 	0
#define INVALLSTNQSZ 0
#define INVALDATAPTR 	0

char selfIpAddr[4] = { 192, 168, 16, 72 };
char remoteIpAddr [4] = {192, 168, 16, 25 };


CdSuNhTcpInstId nhInstId;		// TCP NH Instance ID.
CdSuActorId nhModId;			// Network handler Actor ID.
CdSuActorId smModId;			// Sys Mgmt Actor ID.
CdSuActorId applnModId;			// Application Actor ID.

CdSuNhTcpConnKey connKey [MAX_NO_CONN];	// Connection Key.
CdSuNhTcpLstnKey lstnKey;		// Listener Key.

bool tpkt = true;			// TPKT flag.

CdSuNhTcpSubOpcode subOpCode;

//----------------------------------------------------------------------------
// API Parameter Variable.
//----------------------------------------------------------------------------
CdSuNhTcpMiInitParam initParam;
CdSuNhTcpMiTerminateParam termparam;
CdSuNhTcpMiDelNhParam delnhparam;
CdSuNhTcpMiCtrlReqParam ctrlReqParam;
CdSuNhTcpMiCfgReqParam cfgReqParam;
CdSuNhTcpMiStatusReqParam statusReqParam;
CdSuNhTcpMiStsReqParam statsReqParam;

CdSuNhTcpUiAddLstnReqParam listenReqParam;
CdSuNhTcpUiConReqParam conReqParam;
CdSuNhTcpUiDataReqParam dataReqParam; 
CdSuNhTcpUiRemLstnReqParam rmlstnparam;
CdSuNhTcpUiCloseConReqParam clConReqParam;

int counter = -1;
const int maxCounter = 5;
char data [maxCounter] [100] = 
			{
				"TCP NH built by CDOT Bangalore",
				"Internet Telephony Group Bangalore",
			};

	
char counterString [][100] = 
{
	"CDSU_NHTCP_ADDLSTNREQ_CTR",
	"CDSU_NHTCP_CONREQ_CTR",
	"CDSU_NHTCP_DATAREQ_CTR",
	"CDSU_NHTCP_DISCONREQ_CTR",
	"CDSU_NHTCP_REMLSTNREQ_CTR",
	"CDSU_NHTCP_CONIND_CTR",
	"CDSU_NHTCP_DATAIND_CTR",
	"CDSU_NHTCP_CLOSECON_IND_CTR",
	"CDSU_NHTCP_SUCCESS_CON_CTR",
	"CDSU_NHTCP_SUCCESS_ADDLSTN_CTR",
	"CDSU_NHTCP_SUCCESS_DATATX_CTR",
	"CDSU_NHTCP_SUCCESS_CLOSE_CON_CTR",
	"CDSU_NHTCP_SUCCESS_REMLSTN_CTR",
	"CDSU_NHTCP_CUR_ACTIVE_CON_CTR",
	"CDSU_NHTCP_CUR_ACTIVE_LSN_CTR",
	"CDSU_NHTCP_MAX_CTR"

};

char failCodeString  [] [100] =
{
	"CDSU_NHTCP_FCODE_INVPARAM" ,		// Parameter of request are invalid.
	"CDSU_NHTCP_FCODE_INV_OPERATION",	// Invalid operation requested.
	"CDSU_NHTCP_FCODE_TCPCONN_OVERFLOW",	// Exceeds Max No Of Conn. 
	"CDSU_NHTCP_FCODE_TCPLSTN_OVERFLOW",	// Exceeds Max No Of Lstn. 
	"CDSU_NHTCP_FCODE_BUFLEN_OVERFLOW",	// Exceeds Max Buffer Length. 
	
	"CDSU_NHTCP_FCODE_SOCK_NO_KERMEM_ERR",	// Not enough kernel memory.
	"CDSU_NHTCP_FCODE_SOCK_NO_PFTOF_ERR",	// Process file table overflow.
	"CDSU_NHTCP_FCODE_SOCK_INSUF_MEM_ERR",	// Insufficient Memory available.
	"CDSU_NHTCP_FCODE_SOCK_CRE_ERR",		// Socket Creation error.
	"CDSU_NHTCP_FCODE_SOCK_READ_ERR",		// Socket read error.
	"CDSU_NHTCP_FCODE_SOCK_LSTN_ERR",		// Socket listen error.
	"CDSU_NHTCP_FCODE_TPKT_READ_ERR",		// TPKT read error.
	"CDSU_NHTCP_FCODE_TPKT_WRITE_ERR",	// TPKT write error.
	"CDSU_NHTCP_FCODE_TPKT_VER_UNSUPPORT",	// Unsupported TPKT version.
	"CDSU_NHTCP_FCODE_DATA_WRITE_ERR",	// Data write error.
	"CDSU_NHTCP_FCODE_BINDERROR",		// Bind error.
	"CDSU_NHTCP_FCODE_CONNECTERROR",		// Connect Error.
	"CDSU_NHTCP_FCODE_SENDERROR",		// Send error.
	"CDSU_NHTCP_FCODE_NO_CONN",		// Connection doesn't exists. 
	"CDSU_NHTCP_FCODE_UNKNOWNERROR",		// Unknown Error.
	"CDSU_NHTCP_FCODE_NOT_SET"		// Fail code not set.

} ;

char apiRtTypeString  [] [100] =
{
	"CDSU_NHTCP_API_RT_SUCCESS",
	"CDSU_NHTCP_API_RT_FAIL",

	"CDSU_NHTCP_API_RT_INVINSTID",			// Invalid NH Instance Id.
	"CDSU_NHTCP_API_RT_INST_OVERFLOW",		// Over Flow of Instance.
	"CDSU_NHTCP_API_RT_OPERATION_INV",		// Request Operation Is invalid (Out Of Sequence Operation)
	"CDSU_NHTCP_API_RT_MAXCON_OUTOFRNG",		// Max Connection out of range.
	"CDSU_NHTCP_API_RT_MAXLSTN_OUTOFRNG",		// Max Listener out of range.
	"CDSU_NHTCP_API_RT_MAXBUFLEN_OUTOFRNG",		// Max Buffer out of range.
	"CDSU_NHTCP_API_RT_DATA_SELTMRVAL_OUTOFRNG",	// Data Select Timer value out of range.
	"CDSU_NHTCP_API_RT_LSTN_SELTMRVAL_OUTOFRNG",	// Lstnr Select Timer value out of range.
	"CDSU_NHTCP_API_RT_NOHLR_SELTMRVAL_OUTOFRNG",	// No Handle Select Timer value out of range.
	"CDSU_NHTCP_API_RT_LSTNQSZ_OUTOFRNG",		// Listern Queue Size out of range.
	"CDSU_NHTCP_API_RT_OPCODE_INV",			// Invalid Opcode.
	"CDSU_NHTCP_API_RT_SUBOPCODE_INV",		// Invalid Subopcode.
	"CDSU_NHTCP_API_RT_DATAPTR_INV",		// Data Pointer = NULL
	"CDSU_NHTCP_API_RT_DATALEN_INV",		// Data Len = 0
	"CDSU_NHTCP_API_RT_DATALEN_OUTOFRNG"		// Outof range of Data Length

} ;

//============================================================================
// <End of parameter.h>
//============================================================================
#endif
