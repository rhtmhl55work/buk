#ifndef __CDSU_SANITY_MSGS_H__
#define __CDSU_SANITY_MSGS_H__
//============================================================================
// Internet Telephony Software - (c) Copyright 2001 by ITG, C-DOT, Bangalore, 
// India.
// Filename     : cdsusanitymsgs.h
// Description  : All system-level messages.
// Ref Docs     : 
// Author       : rhari, dip
// Created On   : Feb 22, 2002 
// Last Modified: Tue Jul 30 19:30:09 IST 2002
// Added to Utility Kit from SS7AMF on Wed Apr 19 18:19:26 IST 2006
//============================================================================
#include "cdsuutiltypes.h"
#include "cdsutimer.h"

//============================================================================
// Sanity Message Opcode
//============================================================================

enum SanityMessageOpcode
{
	// Sanity Feature
	SANITY_MSG = 50,			// Sanity msg
    	SANITY_FAILURE_MSG, 		// Sanity Failure msg
	SANITY_TIMEOUT_MSG,
	SANITY_TX_TI_MSG,
	SANITY_RX_TI_MSG
};

struct CdMessageHeader
{
	CdModuleId	dst;			// Id of the destination
	CdModuleId	src;			// Id of the source 
	Uint16		msgLen;			// message length
	Uint16		opcode;
};


//enum
//{
//	CDSU_TMR_MAX_USERINFO_NUM=8	// <= userInfo
//};


//============================================================================
// Sanity Messages 
//============================================================================
struct SanityMsg : CdMessage
{
	SanityMsg (){}; 
	void initMsg (const CdModuleId& dstModId, const CdModuleId& srcModId)
	{
		msgHdr.dst = dstModId;
		msgHdr.src = srcModId;
		//msgHdr.msgLen = sizeof (SanityMsg) - sizeof (CdMessageHdr);
		msgHdr.msgLen = sizeof (SanityMsg);
		msgHdr.opcode = SANITY_MSG;
	}
	~SanityMsg (){}; 
	

}; // struct SanityMsg 	

struct SanityFailureMsg : CdMessage
{
	Uint8 sanityTag;
	Uint8  pad8;
	Uint16 pad16;
	SanityFailureMsg (){};
	void initMsg (const CdModuleId& dst, const CdModuleId& src, Uint8 tag)
	{
		msgHdr.dst = dst;
		msgHdr.src = src;
		msgHdr.msgLen = sizeof (SanityFailureMsg);
		//msgHdr.msgLen = sizeof (SanityFailureMsg) - sizeof (CdMessageHdr);
		msgHdr.opcode = SANITY_FAILURE_MSG;
		
		sanityTag = tag;
		pad8 = 0;
		pad16 = 0;
	}
	~SanityFailureMsg (){};
};


struct SanityTimeoutMsg : CdMessage
{
	Uint32 opcode;					// actual timer sub opcode
	Uint32 timerId;					// which timer.
	Uint32 info [CDSU_TMR_MAX_USERINFO_NUM];	// <= userInfo
	
	//constructor 
	SanityTimeoutMsg (){};
	void initMsg (CdSuTimeout& timeOutInfo)
	{	
		msgHdr.dst = timeOutInfo.destModuleId;
		msgHdr.src = timeOutInfo.destModuleId; 

		msgHdr.opcode = SANITY_TIMEOUT_MSG;
		msgHdr.msgLen = sizeof (SanityTimeoutMsg);
		//msgHdr.msgLen = sizeof (SanityTimeoutMsg) - sizeof (CdMessageHeader);

		opcode = timeOutInfo.opcode;
		timerId = timeOutInfo.timerId;
		memcpy (info, timeOutInfo.userInfo, sizeof (info));
	}
}; // struct TimeoutMsg	






//----------------------------------------------------------------------------
// ShIdtoUint32 (const CdServiceHandlerId&);
// Description  : Uint32 given CdServiceHandlerId.
// Inputs       : CdServiceHandlerId
// Return Value : None
// Side Effects : 
// Algorithm    : 
//----------------------------------------------------------------------------

/*static inline Uint32& ShIdtoUint32 (const CdServiceHandlerId& shId)
{
	return *((Uint32*)&shId);
} // ShIdtoUnit32 () */


#endif
//============================================================================
// <end of sanity.h>
//============================================================================
