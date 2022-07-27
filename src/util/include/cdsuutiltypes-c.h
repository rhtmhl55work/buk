#ifndef CDSU_UTILTYPES_H
#define CDSU_UTILTYPES_H
//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsuutiltypes-c.h
// Description	: Consists of the basic typedefs required by util.
// Author       : Meghna Doshi
// Created On   : 
// Last Modified: 1. Wed Jun 18 18:46:15 IST 2003
// 		  	Added to Utility Kit from SS7AMF on 
// 		  	Wed Apr 19 18:19:26 IST 2006
// 		  2. Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>

// In ECOS there are defines for Uint8, Uint16 and Uint32 because for which 
// powerpc gcc compiler fails. 

#ifdef __ECOS
#undef Uint8
#undef Uint16
#undef Uint32
#endif


//----------------------------------------------------------------------------
// Typedefs.
//----------------------------------------------------------------------------
typedef char Sint8;          	// signed byte (Range: -128 to +127)
typedef unsigned char Uint8; 	// unsigned byte (Range: 0 - 255)
typedef unsigned char Uchar; 	// unsigned char (Range: 0 - 255)
typedef signed short Sint16;  	// 16-bit signed int (Range: -32768 to +32767)
typedef unsigned short Uint16;	// 16-bit unsigned int (Range: 0 to 65535)
typedef int Sint32;	   	// 32-bit signed int (Range:-2147483648,2147483647)
typedef unsigned int Uint32; 	// 32-bit unsigned int (Range: 0 to 4294967295
typedef unsigned long Ulong;	// Ulong-32 bit unsigned integer in 32 bit m/c and 64 bit integer in 64 bit m/c

//----------------------------------------------------------------------------
// Structure	: Sm service type 
// Description	: TODO: Remove this later
//----------------------------------------------------------------------------

typedef enum SmServiceType
{
	SM_SERVICE_TYPE_INVALID,		// Invalid service type
	SM_SERVICE_TYPE_PLATFORM_SERVICE,
	SM_SERVICE_TYPE_APPLICATION_SERVICE,
	SM_SERVICE_TYPE_MAX,
} SmServiceType;

typedef enum SmRsrcType
{
        SM_RSRC_TYPE_INVALID,
        SM_RSRC_TYPE_DEV,       // Resource is a device.
        SM_RSRC_TYPE_APP,       // Resource is an app.
        SM_RSRC_TYPE_MAX
} SmRsrcType;



//----------------------------------------------------------------------------
// Common header for all messages
//----------------------------------------------------------------------------

typedef struct  CdHardwareUnitId
{
        Uint8   rackId;
        Uint8   unitId;
        Uint8   slotId;
        Uint8   procId;

} CdHardwareUnitId;

typedef struct CdAppSrvcHlrId
{
	Uint8   appType;        // Application type
	Uint8   appId;          // Instance number
	Uint8   appModuleId;    // indicates submodules 
} CdAppSrvcHlrId; // __attribute__ ((packed)) appSrvcHlrId;

typedef struct CdServiceHandlerId
{
        Uint8   srvcType;       // Platform or App services
	CdAppSrvcHlrId appSrvcHlrId;
	
} CdServiceHandlerId; 


//----------------------------------------------------------------------------
// Structure	: Message header
// Description	: Common message header for all the inter-module messages
//----------------------------------------------------------------------------

typedef struct CdModuleId
{
        CdHardwareUnitId          hwUnitId;
        CdServiceHandlerId        srvcHlrId;

} CdModuleId; // __attribute__ ((packed));


//----------------------------------------------------------------------------
// Structure	: Message header
// Description	: Common message header for all the inter-module messages
//----------------------------------------------------------------------------

typedef struct CdMessageHdr
{
	CdModuleId      dst;                    // destination module id
        CdModuleId      src;                    // source module id
        Uint16          msgLen;                 // message length
        Uint16          opcode;			// opcode

} CdMessageHdr;

//----------------------------------------------------------------------------
// Structure	: Message 
// Description	: Base class for all inter-module messages
//----------------------------------------------------------------------------


typedef struct CdMessage
{
	CdMessageHdr msgHdr;			// message header

} CdMessage;

//----------------------------------------------------------------------------
// Enumeration	: Application id
// Description	: Application ids
//----------------------------------------------------------------------------

typedef enum MgcApplicationType
{
	CD_APPLICATION_TYPE_MGC = 1,
	CD_APPLICATIONID_TYPE_INVALID

} MgcApplicationType;

//=============================================================================
// <End>
//=============================================================================
#endif
