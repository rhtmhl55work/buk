#ifndef __CDSU_NHUDPTYPES_H__
#define __CDSU_NHUDPTYPES_H__

//============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhudtypes.h
// Description  : Type Definitions the Generic UDP Network Handler.
// Author       : Vijaya Kumar
// Created On   : Mon Mar  8 16:28:57 IST 2004
// Last Modified: 
// Reference	: C++ Network Programming by Douglas C. Schmidt and 
//		  Stephen D. Huston.
//		  Unix Network Programming by W. Rechard Steavens	
// Modified	:
//============================================================================

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------

#include "cdsuutiltypes.h" 	

//----------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------

typedef CdModuleId CdSuActorId; 	// Each Module Has Actor ID.

// Each NH instance is idintified by unique ID.
typedef Uint32 CdSuNhUdpInstId;

// Each Udp Port within NH Instance identified by unique ID.
typedef Uint32 CdSuNhUdpPortId;	

typedef Uint16 CdSuIpv4UdpPortNo;	// Udp Port number;
typedef Uint32 CdSuIpv4Addr;		// Ipv4 Address 

// Callback Function (Event Handler function) prototype
typedef void (*CdSuNhUdpEventHlr) (void*);

//----------------------------------------------------------------------------
// Struct	: CdSuNhUdpPortKey
// Description 	: Each Udp Port added by user is identified by unique
// 		: Udp Port Key. Udp Port Key composes of Instance Id of NH 
// 		: over which NH instance Udp Port opened and Udp Port Id which
// 		: is a unique identifier of a Udp Port within NH Instance.
//----------------------------------------------------------------------------

struct CdSuNhUdpPortKey
{
	CdSuNhUdpInstId nhUdpInstId;	// NH instance Id.
	CdSuNhUdpPortId	nhUdpPortId;	// Udp port Id;
	
}; // struct CdSuNhUdpPortKey


//----------------------------------------------------------------------------
//				Enumerations
//				------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// contains some default value 
//----------------------------------------------------------------------------
enum
{
	CDSU_NHUDP_MAX_INST = 128,	// Possible maximum NH instance can be in a process
	CDSU_NHUDP_MAX_UDPPORT = 1024,	// Maximum no of udp port allowed to open per NH instance
	CDSU_NHUDP_DEF_BUFLEN = 4096,	// Default maximum receiver buffer len.
	CDSU_NHUDP_DEF_DATA_SELTMR = 250,	// Default value for timeout for select to read data over udp port(Microseconds).
	CDSU_NHUDP_DEF_NOHDL_TMR = 25000000,	// Sleeping time if there is no fd to read (Microseconds).
	
	CDSU_NHUDP_INV_UDPPORT = 0xffff,	// Invalid Udp Port
	CDSU_NHUDP_INV_UDPPORTID = 0xffffffff,	// Invalid Udp Port ID
	CDSU_NHUDP_ALWOP_UDPPORTID = 0, 	// Always opened udp port's port id.
	CDSU_NHUDP_INV_NHINSTID = 0xffffffff,	// Invalid Instancd ID of NH
	CDSU_NHUDP_INV_UDPEVENTHLR = 0
};

//----------------------------------------------------------------------------
// Enum 	: CdSuNhUdpApiRetType
// Description 	: API Return Type, Following are enum returned by api function.
// 		: This enum tells wether operation is success or failuare
// 		: reason if failed. 
//----------------------------------------------------------------------------
//
enum CdSuNhUdpApiRetType
{
	CDSU_NHUDP_API_RT_SUCCESS,		// Function operation is success.
	
	// Failure Retrun Types.	
	CDSU_NHUDP_API_RT_INSTID_INV,		// Invalid NH Instance Id.
	CDSU_NHUDP_API_RT_INST_OVERFLOW,	// Over Flow of Instance.
	CDSU_NHUDP_API_RT_OP_INV,		// Request Operation Is invalid (Out Of Sequence Operation)
	CDSU_NHUDP_API_RT_FAIL,			// Operation Failed.
	CDSU_NHUDP_API_RT_MAXPORT_OUTOFRNG,	// Max Connection configured is out of range.
	CDSU_NHUDP_API_RT_MAXBUFLEN_OUTOFRNG,	// Max Buffer configured is out of range.
	CDSU_NHUDP_API_RT_SELTMRVAL_OUTOFRNG,	// Select Timer value configured is out of range.
	CDSU_NHUDP_API_RT_NOHLRTMRVAL_OUTOFRNG,	// No Handler Timer configured is out of ranage.
       	CDSU_NHUDP_API_RT_OPCODE_INV,		// Invalid Opcode.
	CDSU_NHUDP_API_RT_SUBOPCODE_INV,	// Invalid Subopcode.
	CDSU_NHUDP_API_RT_PORTID_INV,		// Invalid Port Id.
	CDSU_NHUDP_API_RT_DATAPTR_INV,		// Data Pointer = NULL
	CDSU_NHUDP_API_RT_DESTPORT_INV,		// Dest Port = 0
	CDSU_NHUDP_API_RT_DATALEN_INV,		// Data Len = 0
	CDSU_NHUDP_API_RT_DATALEN_OUTOFRNG,	// Outof range of Data Length
	
}; // enum CdSuNhUdpApiRetType


//----------------------------------------------------------------------------
// Enum		: CdSuNhUdpOpcode
// Description	: SM <-> NH Opcodes
// 		: Appl <-> NH Opcodes
// 		: Opcode tells what type of request or response or indication. 
//----------------------------------------------------------------------------

enum CdSuNhUdpOpcode
{
	// SM -> NH
	CDSU_NHUDP_MI_INIT_REQ,			// NH Init Request.
	CDSU_NHUDP_MI_DELNH_REQ,		// NH Delete Request.
	CDSU_NHUDP_MI_CTRL_REQ,			// NH Start Request.
	CDSU_NHUDP_MI_CFG_REQ,			// NH Config Request.
	CDSU_NHUDP_MI_STATUS_REQ,		// NH Status Request.
	CDSU_NHUDP_MI_STATISTICS_REQ,		// NH Statistics Request.

	// NH -> SM
	CDSU_NHUDP_MI_INIT_RSP,			// NH Init Response.
	CDSU_NHUDP_MI_DELNH_RSP,		// NH Delete Response.
	CDSU_NHUDP_MI_CTRL_RSP,			// NH Control Response.
	CDSU_NHUDP_MI_CFG_RSP,			// NH Config Response.
	CDSU_NHUDP_MI_STATUS_RSP,		// NH Status Response.
	CDSU_NHUDP_MI_STATISTICS_RSP,		// NH Statistics Response.
	CDSU_NHUDP_MI_USTATUS_IND,		// NH Unsolicited Status Indication.

	// Appl -> NH
	CDSU_NHUDP_UI_ADD_UDPPORT_REQ,		// Add UDP Port Req.
	CDSU_NHUDP_UI_REM_UDPPORT_REQ,		// Remove UDP Port Req.
	CDSU_NHUDP_UI_DATA_REQ,			// Transmit Data Req.
	CDSU_NHUDP_UI_SEND_REQ,			// Send Data without Adding Udp Port.
	
	// NH -> Appl
	CDSU_NHUDP_UI_ADD_UDPPORT_RSP,		// Add UDP Port Rsp.
	CDSU_NHUDP_UI_REM_UDPPORT_RSP,		// Add UDP Port Rsp.
	CDSU_NHUDP_UI_DATA_RSP,			// Transmit Data Rsp.
	CDSU_NHUDP_UI_SEND_RSP,			// Send Response.
	CDSU_NHUDP_UI_DATA_IND,			// Data Indication.

	// Invalid 
	CDSU_NHUDP_INVALID
		
}; // enum CdSuNhUdpOpcode


//----------------------------------------------------------------------------
// Enum		: CdSuNhUdpSubOpcode
// Description 	: Some Message Contains Subopcode apart from opcode
// 	 	: Ex. Control , Configuration operation.
//----------------------------------------------------------------------------

enum CdSuNhUdpSubOpcode
{
	
	CDSU_NHUDP_SO_INITIAL_CFG_REQ = 1,	// Initial Configuration.
	CDSU_NHUDP_SO_GENERAL_CFG_REQ,		// General Configuration.
	CDSU_NHUDP_SO_CRE_REQ,			// Create NH. 
	CDSU_NHUDP_SO_DEL_REQ,			// Delete Req.
	CDSU_NHUDP_SO_START_REQ,		// Start Request.
	CDSU_NHUDP_SO_STOP_REQ,			// Stop Request.
	CDSU_NHUDP_SO_INITIAL_CFG_RSP,		// Initial Configuration Response.
	CDSU_NHUDP_SO_GENERAL_CFG_RSP,		// General Configuration Response.
	CDSU_NHUDP_SO_CRE_RSP,			// Create Response.
	CDSU_NHUDP_SO_DEL_RSP,			// Delete Response.
	CDSU_NHUDP_SO_START_RSP,		// Start Response.
	CDSU_NHUDP_SO_STOP_RSP,			// Stop  Response.
	CDSU_NHUDP_SO_INVALID			// Invalid.
		
}; // enum CdSuNhUdpSubOpcode

//----------------------------------------------------------------------------
// Enum 	: CdSuNhUdpRspCode
// Description 	: It tells whether the operation requested is fail or success.
//----------------------------------------------------------------------------

enum CdSuNhUdpRspCode
{
	
	CDSU_NHUDP_RSPCODE_SUCCESS = 1,		// Success Response
	CDSU_NHUDP_RSPCODE_FAIL,		// Fail Response
	
}; // enum CdSuNhUdpRspCode

//----------------------------------------------------------------------------
// Enum		: CdSuNhUdpRspFailCode
// Description	: Response Fail Code
// 		: If Response code in response is fail this 
// 		: this enum tells which type of error.	
//----------------------------------------------------------------------------

enum CdSuNhUdpRspFailCode
{

	CDSU_NHUDP_RSPFCODE_INVPARAM = 1,	// Parameter of request are invalid.
	CDSU_NHUDP_RSPFCODE_INV_OP,		// Invalid operation requested.
	CDSU_NHUDP_RSPFCODE_UDPPORT_OVERFLOW,	// Can't open udp port since
       						// no of udp port existing = Max Udp Portconfigured
	CDSU_NHUDP_RSPFCODE_BINDERROR,		// Bind error.
	CDSU_NHUDP_RSPFCODE_CONNECTERROR,	// Connect Error.
	CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST,	// Udp Port Not exist.
	CDSU_NHUDP_RSPFCODE_UNKNOWNERROR,	// Unknown Error.
	CDSU_NHUDP_RSPFCODE_SENDERROR,		// Send error.
	CDSU_NHUDP_RSPFCODE_OPENERROR,		// Socket Open error
	CDSU_NHUDP_RSPFCODE_READERROR,		// Socket Read Error.
	CDSU_NHUDP_RSPFCODE_INVALID

}; // enum CdSuNhUdpRspFailCode

//----------------------------------------------------------------------------
// Enum		: CdSuNhUdpState
// Description 	: It indicates state of NH 
// 		: *) Created
// 		: *) Running
//----------------------------------------------------------------------------

enum CdSuNhUdpState
{
	
	CDSU_NHUDP_ST_CREATED = 1,		// Created State.
	CDSU_NHUDP_ST_RUNNING,			// Running State
	CDSU_NHUDP_ST_INVALID

}; // enum CdSuNhUdpState

//----------------------------------------------------------------------------
// Enum		: CdSuNhUdpCtrType
// Description 	: enumeration of counter types.
// 		: Statistics package contains different type of counter.
// 		: this contains enum for all type of counter.
//----------------------------------------------------------------------------

enum CdSuNhUdpCtrType
{	
	// No Of Add Udp Port Request Made.
	CDSU_NHUDP_ADD_UDPPORT_CTR = 0,
	// No Of Request Made for Add Udp Port of type local only fixed.
	CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_CTR,
	// No Of Request Made for Add Udp Port of type local and destination both fixed.
	CDSU_NHUDP_ADD_UDPPORT_FIXED_LCLDST_CTR,
	// No of Request Made for Add Udp Port of type destination only fixed.
	CDSU_NHUDP_ADD_UDPPORT_FIXED_DST_CTR,
	// No of Request Made for Add Udp Port where local  port no is standard.
	CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_ST_CTR,
	// No of Request Made for Add Udp Port of where local port no is ephemeral. 
	CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_EPH_CTR,
	
	// No of Data Request Made.
	CDSU_NHUDP_DATAREQ_CTR,
	// No of Data Request Made over udp port of type local only fixed.
	CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR,
	// No of Data Request Made over udp port of type local and destiantion both fixed.
	CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR,
	// No of Data Request Made over Udp Port of type destination only fixed.
	CDSU_NHUDP_DATAREQ_FIXED_DST_CTR,
	// No of Data Request Made over Udp Port of type both dest and local not fixed.
	CDSU_NHUDP_DATAREQ_NOTFIXED_CTR,
	// No of Data Request Made over udp port of type local fixed and local standard port.
	CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR,
	// No of Data Request made over udp port of type local fixed and local port is ephemeral.
	CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR,
	// No of data request made over udp port of type always opened.
	CDSU_NHUDP_DATAREQ_ALWOP_CTR,
		
	// No of remove udp port of request.
	CDSU_NHUDP_REM_UDPPORT_CTR,
	// No of remove udp port request where udp port of type local only fixed.
	CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_CTR,
	// No of remove udp port request where udp port of type local and destination both fixed.
	CDSU_NHUDP_REM_UDPPORT_FIXED_LCLDST_CTR,
	// NO of remove udp port request where udp port of type destination only fixed.
	CDSU_NHUDP_REM_UDPPORT_FIXED_DST_CTR,
	// No of remove udp port request where udp port of type local fixed and port is standard.
	CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_ST_CTR,
	// No of remove udp port request where udp port of type local fixed and port is ephemeral.
	CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_EPH_CTR,

	// No of data indication.
	CDSU_NHUDP_DATAIND_CTR,
	// No of data indication over udp port of type local only fixed.
	CDSU_NHUDP_DATAIND_FIXED_LCL_CTR,
	// NO of data indication over udp port of type both destination and local fixed.
	CDSU_NHUDP_DATAIND_FIXED_LCLDST_CTR,
	// No of data indication over udp port of type local fixed and port is standard.
	CDSU_NHUDP_DATAIND_FIXED_LCL_ST_CTR,
	// No of data indication over udp port of type local fixed and port is ephemeral.
	CDSU_NHUDP_DATAIND_FIXED_LCL_EPH_CTR,
	// No of data indication error.
	CDSU_NHUDP_DATAIND_ERROR_CTR,
	
	// Following are success counter for all above type.
	CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR,
	CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_CTR,
	CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCLDST_CTR,
	CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_DST_CTR,
	CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_ST_CTR,
	CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_EPH_CTR,
	
	CDSU_NHUDP_SUCCESS_DATAREQ_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_DST_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_NOTFIXED_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_ST_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_EPH_CTR,
	CDSU_NHUDP_SUCCESS_DATAREQ_ALWOP_CTR,
		
	CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR,
	CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_CTR,
	CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCLDST_CTR,
	CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_DST_CTR,
	CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_ST_CTR,
	CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_EPH_CTR,

	// No of active udp port present in udp nh.
	CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR,
	// No of active udp port present in udp nh where local is fixed.
	CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR,

	CDSU_NHUDP_MAX_CTR
		
}; // enum CdSuNhUdpCtrType

//============================================================================
// <End of cdsunhudptypes.h>
//============================================================================
#endif
