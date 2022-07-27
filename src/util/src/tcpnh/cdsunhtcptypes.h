#ifndef __CDSU_NHTCPAPI_H
#define __CDSU_NHTCPAPI_H
//============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhtcpapi.h
// Description  : API for the Generic Network Handler.
// Author       : Seema Jain
// Created On   : 4th Feb, 2004
// Last Modified: By Sumit Bijwe  on Wed Feb 25 10:21:38 IST 2004
//               + Added cdSuNhTcpMiTerminate () API.
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//============================================================================

#include "cdsuutiltypes.h"

//----------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------

typedef CdModuleId CdSuActorId;
typedef Uint32 CdSuNhTcpInstId;		// TCP Network Handler Instance Id.
typedef Uint32 CdSuNhTcpLstnId;
typedef int CdSuNhTcpHandle;		
typedef Uint32 CdSuNhTcpConnId;
typedef Uint32 CdSuIpv4Addr;
typedef Uint16 CdSuIpv4TcpPortNo;
typedef void (*CdSuNhTcpEventHlr) (void*);

//----------------------------------------------------------------------------
// Enumerations
//----------------------------------------------------------------------------

enum 
{
	CDSU_NHTCP_TPKT_VERSION = 3,		// Tpkt version No.

	CDSU_NHTCP_MAX_INST = 128,
	
	CDSU_NHTCP_DEF_NOCONNS = 100,
	CDSU_NHTCP_DEF_NOLSTNS = 20,
	CDSU_NHTCP_DEF_TCPBUFLEN = 4096,
	CDSU_NHTCP_DEF_LSTN_SELTMR = 250,	// Microseconds.
	CDSU_NHTCP_DEF_DATA_SELTMR = 250,	// Microseconds.
	CDSU_NHTCP_DEF_NOHDL_TMR = 250,		// Microseconds.
	CDSU_NHTCP_DEF_LSTNQ_SIZE = 5,		// Microseconds.

	CDSU_NHTCP_INV_CONNID = 0xffffffff,
	CDSU_NHTCP_INV_LSTNID = 0xffffffff,
	CDSU_NHTCP_INV_NHINSTID = 0xffffffff,
	CDSU_NHTCP_INV_TCPHANDLE = 0xffffffff
};

//----------------------------------------------------------------------------
// API Return Type  
//----------------------------------------------------------------------------
enum CdSuNhTcpApiRetType
{
	CDSU_NHTCP_API_RT_SUCCESS,
	CDSU_NHTCP_API_RT_FAIL,

	CDSU_NHTCP_API_RT_INVINSTID,			// Invalid NH Instance Id.
	CDSU_NHTCP_API_RT_INST_OVERFLOW,		// Over Flow of Instance.
	CDSU_NHTCP_API_RT_OPERATION_INV,		// Request Operation Is invalid (Out Of Sequence Operation)
	CDSU_NHTCP_API_RT_MAXCON_OUTOFRNG,		// Max Connection out of range.
	CDSU_NHTCP_API_RT_MAXLSTN_OUTOFRNG,		// Max Listener out of range.
	CDSU_NHTCP_API_RT_MAXBUFLEN_OUTOFRNG,		// Max Buffer out of range.
	CDSU_NHTCP_API_RT_DATA_SELTMRVAL_OUTOFRNG,	// Data Select Timer value out of range.
	CDSU_NHTCP_API_RT_LSTN_SELTMRVAL_OUTOFRNG,	// Lstnr Select Timer value out of range.
	CDSU_NHTCP_API_RT_NOHLR_SELTMRVAL_OUTOFRNG,	// No Handle Select Timer value out of range.
	CDSU_NHTCP_API_RT_LSTNQSZ_OUTOFRNG,		// Listern Queue Size out of range.
	CDSU_NHTCP_API_RT_OPCODE_INV,			// Invalid Opcode.
	CDSU_NHTCP_API_RT_SUBOPCODE_INV,		// Invalid Subopcode.
	CDSU_NHTCP_API_RT_DATAPTR_INV,			// Data Pointer = NULL
	CDSU_NHTCP_API_RT_DATALEN_INV,			// Data Len = 0
	CDSU_NHTCP_API_RT_DATALEN_OUTOFRNG		// Outof range of Data Length

};

// Fail response. When Response is fail, then fail code will be set.
enum CdSuNhTcpFailCode
{
	CDSU_NHTCP_FCODE_INVPARAM,		// Parameter of request are invalid.
	CDSU_NHTCP_FCODE_INV_OPERATION,		// Invalid operation requested.
	CDSU_NHTCP_FCODE_TCPCONN_OVERFLOW,	// Exceeds Max No Of Conn. 
	CDSU_NHTCP_FCODE_TCPLSTN_OVERFLOW,	// Exceeds Max No Of Lstn. 
	CDSU_NHTCP_FCODE_BUFLEN_OVERFLOW,	// Exceeds Max Buffer Length. 

	// Socket Error.
	CDSU_NHTCP_FCODE_SOCK_NO_KERMEM_ERR,	// Not enough kernel memory.
	CDSU_NHTCP_FCODE_SOCK_NO_PFTOF_ERR,	// Process file table overflow.
	CDSU_NHTCP_FCODE_SOCK_INSUF_MEM_ERR,	// Insufficient Memory available.
	CDSU_NHTCP_FCODE_SOCK_CRE_ERR,		// Socket Creation error.
	CDSU_NHTCP_FCODE_SOCK_READ_ERR,		// Socket read error.
	CDSU_NHTCP_FCODE_SOCK_LSTN_ERR,		// Socket listen error.

	CDSU_NHTCP_FCODE_TPKT_READ_ERR,		// TPKT read error.
	CDSU_NHTCP_FCODE_TPKT_WRITE_ERR,	// TPKT write error.
	CDSU_NHTCP_FCODE_TPKT_VER_UNSUPPORT,	// Unsupported TPKT version.
	CDSU_NHTCP_FCODE_DATA_WRITE_ERR,	// Data write error.

	CDSU_NHTCP_FCODE_BINDERROR,		// Bind error.
	CDSU_NHTCP_FCODE_CONNECTERROR,		// Connect Error.
	CDSU_NHTCP_FCODE_SENDERROR,		// Send error.

	CDSU_NHTCP_FCODE_NO_CONN,		// Connection doesn't exists. 
	CDSU_NHTCP_FCODE_NO_LSTN,		// Connection doesn't exists. 
	CDSU_NHTCP_FCODE_UNKNOWNERROR,		// Unknown Error.
	CDSU_NHTCP_FCODE_NOT_SET,		// Fail code not set.
	CDSU_NHTCP_FCODE_INVALID

};


//----------------------------------------------------------------------------
// CdSuNhTcpOpcode
// SM <-> NH Opcodes
// Appl <-> NH Opcodes
//
// Description:
//----------------------------------------------------------------------------

enum CdSuNhTcpOpcode
{
	// SM -> NH
	CDSU_NHTCP_MI_INIT_REQ,			// NH Init Request.
	CDSU_NHTCP_MI_DELNH_REQ,		// NH Delete Request.
	CDSU_NHTCP_MI_CTRL_REQ,			// NH Start Request.
	CDSU_NHTCP_MI_CFG_REQ,			// NH Config Request.
	CDSU_NHTCP_MI_STATUS_REQ,		// NH Status Request.
	CDSU_NHTCP_MI_STATISTICS_REQ,		// NH Statistics Request.

	// NH -> SM
	CDSU_NHTCP_MI_INIT_RSP,			// NH Init Response.
	CDSU_NHTCP_MI_DELNH_RSP,		// NH Delete Response.
	CDSU_NHTCP_MI_CTRL_RSP,			// NH Control Response.
	CDSU_NHTCP_MI_CFG_RSP,			// NH Config Response.
	CDSU_NHTCP_MI_STATUS_RSP,		// NH Status Response.
	CDSU_NHTCP_MI_STATISTICS_RSP,		// NH Statistics Response.

	CDSU_NHTCP_MI_USTATUS_IND,		// NH Unsolicited Status Indication.

	// Appl -> NH
	CDSU_NHTCP_UI_ADD_LISTENER_REQ,		// Add Lstn Req.
	CDSU_NHTCP_UI_REM_LISTENER_REQ,		// Remove Lstn Req.
	CDSU_NHTCP_UI_CONNECT_REQ,		// Establish Connection.
	CDSU_NHTCP_UI_DATA_REQ,			// Transmit Data Req.
	CDSU_NHTCP_UI_CLOSE_CON_REQ,		// Close Connection Req.

	// NH -> Appl
	CDSU_NHTCP_UI_ADD_LISTENER_RSP,		// Add Lstn Rsp.
	CDSU_NHTCP_UI_REM_LISTENER_RSP,		// Add Lstn Rsp.
	CDSU_NHTCP_UI_CONNECT_RSP,		// Add Connection Rsp.
	CDSU_NHTCP_UI_DATA_RSP,			// Transmit Data Rsp.
	CDSU_NHTCP_UI_CLOSE_CON_RSP,		// Close Connection Rsp.
	CDSU_NHTCP_UI_DATA_IND,			// Data Indication.
	CDSU_NHTCP_UI_CON_IND,			// Connection Indication.
	CDSU_NHTCP_UI_CLOSE_CON_IND,		// Close Connection Indication.

	CDSU_NHTCP_INVALID
};

//----------------------------------------------------------------------------
// Other Enumerations.
//----------------------------------------------------------------------------

enum CdSuNhTcpSubOpcode
{
	CDSU_NHTCP_SO_INIIIAL_CFG_REQ,
	CDSU_NHTCP_SO_GENERAL_CFG_REQ,
	CDSU_NHTCP_SO_START_REQ,
	CDSU_NHTCP_SO_STOP_REQ,
	CDSU_NHTCP_SO_INITIAL_CFG_RSP,
	CDSU_NHTCP_SO_GENERAL_CFG_RSP,
	CDSU_NHTCP_SO_START_RSP,
	CDSU_NHTCP_SO_STOP_RSP,
	CDSU_NHTCP_SO_INVALID
};

enum CdSuNhTcpRspCode
{
	CDSU_NHTCP_RSPCODE_SUCCESS = 1,
	CDSU_NHTCP_RSPCODE_FAIL,
};

enum CdSuNhTcpCloseConReason
{
	CDSU_NHTCP_CCR_NORMAL,			// Normal connection close.
	CDSU_NHTCP_CCR_ERROR,			// Connection close due to some error.
	CDSU_NHTCP_CCR_INVALID
};


enum CdSuNhTcpLstnQSize
{
	CDSU_NHTCP_LSTNQ_LOW_LOAD 	= 10,
	CDSU_NHTCP_LSTNQ_MEDIUM_LOAD 	= 100,
	CDSU_NHTCP_LSTNQ_HIGH_LOAD 	= 2000,
};

enum CdSuNhTcpState
{
	CDSU_NHTCP_ST_CREATED,
	CDSU_NHTCP_ST_RUNNING,
	CDSU_NHTCP_ST_INVALID

};

enum CdSuNhTcpCtrType
{	
	CDSU_NHTCP_ADDLSTNREQ_CTR = 0,
	CDSU_NHTCP_CONREQ_CTR,
	CDSU_NHTCP_DATAREQ_CTR,
	CDSU_NHTCP_DISCONREQ_CTR,
	CDSU_NHTCP_REMLSTNREQ_CTR,

	CDSU_NHTCP_CONIND_CTR,
	CDSU_NHTCP_DATAIND_CTR,
	CDSU_NHTCP_CLOSECON_IND_CTR,

	CDSU_NHTCP_SUCCESS_CON_CTR,
	CDSU_NHTCP_SUCCESS_ADDLSTN_CTR,
	CDSU_NHTCP_SUCCESS_DATATX_CTR,
	CDSU_NHTCP_SUCCESS_CLOSE_CON_CTR,
	CDSU_NHTCP_SUCCESS_REMLSTN_CTR,

	CDSU_NHTCP_CUR_ACTIVE_CON_CTR,
	CDSU_NHTCP_CUR_ACTIVE_LSN_CTR,

	CDSU_NHTCP_MAX_CTR
};

//----------------------------------------------------------------------------
// <End of file>
//----------------------------------------------------------------------------
#endif
