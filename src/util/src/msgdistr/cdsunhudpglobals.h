#ifndef __CDSU__NHUDP_GLOBALS_H__
#define __CDSU__NHUDP_GLOBALS_H__
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsunhudpglobals.h 
// Description  : 
// Author       : Sumit Bijwe
// Created On   : Tue Jul 22 16:47:45 IST 2003
// Last Modified:
// Reference	:
//============================================================================


#include "stdio.h"
#include "cdsuutiltypes.h"
#include "cdsutsqueue.h"

typedef unsigned short int Uint16;

//----------------------------------------------------------------------------
// Socket variables
//----------------------------------------------------------------------------
#define SA struct sockaddr
#define socklen_t unsigned int

enum
{
	CDSU_MSGDISTR_READ_BUF_LEN  = 30000
};

//----------------------------------------------------------------------------
// Enumerations
//----------------------------------------------------------------------------
enum CdSuMdUdpNhEntity
{
	CDSU_MD_ET_NW_APPL = 1,
	CDSU_MD_ET_UDP_NW_HLR,
	CDSU_MD_ET_INVALID
};

enum CdSuMdUdpNhOpcodes
{
	CDSU_MD_NH_TX_BUF_MSG = 1,
	CDSU_MD_DEL_UDPNH_MSG,
	CDSU_MD_AP_RXD_BUF_MSG,
	CDSU_MD_OP_INVALID
};

enum CdSuMdModuleId
{
	CDSU_MD_UDP_MID_NH = 1,
	CDSU_MD_UDP_MID_APPL,
	CDSU_MD_UDP_MID_INVALID
};

//----------------------------------------------------------------------------
// Struct Name	: CdSuMdUdpNhInitParam
// Description	: This message structure is the list of parameters to initalize
//		  the Udp Network Handler.
//---------------------------------------------------------------------------
struct CdSuMdUdpNhInitParam
{
	CdModuleId nhMid;
	Uint32 selfIpAddr;
	Uint16 portNo;
	CdSuTsQueue <CdMessage*>* udpNhQueue;

}; // struct CdSuMdUdpNhInit 

//----------------------------------------------------------------------------
// Struct Name	: CdSuMdUdpNhTxBufMsg 
// Description	: This message structure is for transmit the message buffer to 
//		  the remote. 
//---------------------------------------------------------------------------
struct CdSuMdUdpNhTxBufMsg: CdMessage 
{
	
	CdSuMdUdpNhTxBufMsg (const CdModuleId dst, const CdModuleId src, char* buf, Uint16 len, Uint32 ia, Uint16 pn)
	{
		msgHdr.dst = dst;
		msgHdr.src = src;
		msgHdr.opcode = CDSU_MD_NH_TX_BUF_MSG;
		msgHdr.msgLen = sizeof (CdSuMdUdpNhTxBufMsg);

		encodedMessage = buf;
		ipAddress = ia;
		portNo = pn;
		dataLen = len;
	}

	~CdSuMdUdpNhTxBufMsg ()
	{	
		// Currently nothing.
	}

	Uint32 dataLen;
	char* encodedMessage;
	Uint32 ipAddress;
	Uint16 portNo;
	
};// struct CdSuMdUdpNhTxBufMsg

//----------------------------------------------------------------------------
// Struct Name	: CdSuMdUdpNhDelMsg 
// Description	: This message structure is for sending the request to Udp
//	          Network Handler to delete itself.
//----------------------------------------------------------------------------
struct CdSuMdUdpNhDelMsg : CdMessage 
{
	CdSuMdUdpNhDelMsg (const CdModuleId dest, const CdModuleId src)
	{
		msgHdr.dst = dest;
		msgHdr.src = src;
		msgHdr.opcode = CDSU_MD_DEL_UDPNH_MSG; 
		msgHdr.msgLen = sizeof (CdSuMdUdpNhDelMsg);

	}

	~CdSuMdUdpNhDelMsg ()
	{	
		// Currently nothing.
	}

}; // struct CdSuMdUdpNhDelMsg

#endif
//=============================================================================
//  End of cdsunhudpglobals.h
//=============================================================================

