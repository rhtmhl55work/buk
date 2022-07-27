#ifndef CDSU_UTILTYPES_H
#define CDSU_UTILTYPES_H
//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsuutiltypes.h
// Description	: Consists of the basic typedefs required by util.
// Author       : Seema Chandak
// Created On   : 
// Last Modified: Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
// 		 
//============================================================================

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
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

enum SmServiceType
{
	SM_SERVICE_TYPE_INVALID,		// Invalid service type
	SM_SERVICE_TYPE_PLATFORM_SERVICE,
	SM_SERVICE_TYPE_APPLICATION_SERVICE,
	SM_SERVICE_TYPE_MAX,
};

enum SmRsrcType
{
        SM_RSRC_TYPE_INVALID,
        SM_RSRC_TYPE_DEV,       // Resource is a device.
        SM_RSRC_TYPE_APP,       // Resource is an app.
        SM_RSRC_TYPE_MAX
};



//----------------------------------------------------------------------------
// Common header for all messages
//----------------------------------------------------------------------------

struct  CdHardwareUnitId
{
        Uint8   rackId;
        Uint8   unitId;
        Uint8   slotId;
        Uint8   procId;

	bool operator != (const CdHardwareUnitId& hid)
	{
		if ((rackId == hid.rackId) && (unitId == hid.unitId) && 
			(slotId == hid.slotId) && (procId == hid.procId))
			return false;
		else 
			return true;
	}	
	
	bool operator== (const CdHardwareUnitId& hid)
	{
		if ((rackId == hid.rackId) && (unitId == hid.unitId) && 
			(slotId == hid.slotId) && (procId == hid.procId))
			return true;
		else 
			return false;
	}

	void operator= (const CdHardwareUnitId& hid)
	{
		rackId = hid.rackId;
		unitId = hid.unitId;
		slotId = hid.slotId;
		procId = hid.procId;
	}

};

struct CdServiceHandlerId
{
        Uint8   srvcType;       // Platform or App services
        union
        {
                struct
                {
                        Uint8   appType;        // Application type
                        Uint8   appId;          // Instance number
                        Uint8   appModuleId;    // indicates submodules 
                } __attribute__ ((packed)) appSrvcHlrId;
                struct
                {
                        Uint8   pad8;
                        Uint16  plfModuleId;
			void ntoh ()
			{
				plfModuleId = ntohs (plfModuleId);
			}
			void hton ()
			{
				plfModuleId = htons (plfModuleId);
			}

                } __attribute__ ((packed)) plfSrvcHlrId;
        };
	
	bool operator!= (const CdServiceHandlerId& sid)
	{
		if (srvcType != sid.srvcType)
		{
			return true;
		}

		if (srvcType == SM_SERVICE_TYPE_APPLICATION_SERVICE)
		{
			if ((appSrvcHlrId.appType == sid.appSrvcHlrId.appType)
				&& (appSrvcHlrId.appId == sid.appSrvcHlrId.appId)
				&& (appSrvcHlrId.appModuleId == sid.appSrvcHlrId.appModuleId))
			{
				return false;

			} else
			{
				return true;
			}

		} else if (srvcType == SM_SERVICE_TYPE_PLATFORM_SERVICE)
		{
			if ((plfSrvcHlrId.pad8 == sid.plfSrvcHlrId.pad8) &&
				(plfSrvcHlrId.plfModuleId == sid.plfSrvcHlrId.plfModuleId))
			{
				return false;

			} else
			{
				return true;
			}

		} else
		{
			return true;
		}
	}

	bool operator== (const CdServiceHandlerId& sid)
	{
		if (srvcType != sid.srvcType)
		{
			return false;
		}

		if (srvcType == SM_SERVICE_TYPE_APPLICATION_SERVICE)
		{
			if ((appSrvcHlrId.appType == sid.appSrvcHlrId.appType)
				&& (appSrvcHlrId.appId == sid.appSrvcHlrId.appId)
				&& (appSrvcHlrId.appModuleId == sid.appSrvcHlrId.appModuleId))
			{
				return true;

			} else
			{
				return false;
			}

		} else if (srvcType == SM_SERVICE_TYPE_PLATFORM_SERVICE)
		{
			if ((plfSrvcHlrId.pad8 == sid.plfSrvcHlrId.pad8) &&
				(plfSrvcHlrId.plfModuleId == 
					sid.plfSrvcHlrId.plfModuleId))
			{
				return true;
			} else
			{
				return false;
			}
		} else
		{
			return false;
		}
	}

	void operator= (const CdServiceHandlerId& sid)
	{
		srvcType = sid.srvcType;
		if (srvcType == SM_SERVICE_TYPE_APPLICATION_SERVICE)
		{
			appSrvcHlrId.appType = sid.appSrvcHlrId.appType;
			appSrvcHlrId.appId = sid.appSrvcHlrId.appId;
			appSrvcHlrId.appModuleId = sid.appSrvcHlrId.appModuleId;

		} else if (srvcType == SM_SERVICE_TYPE_PLATFORM_SERVICE)
		{
			plfSrvcHlrId.pad8 = sid.plfSrvcHlrId.pad8;
			plfSrvcHlrId.plfModuleId = sid.plfSrvcHlrId.plfModuleId;
		}
	}

	void ntoh ()
	{
		if (srvcType == SM_SERVICE_TYPE_PLATFORM_SERVICE)
		{
			plfSrvcHlrId.ntoh ();
		}
	}
	void hton ()
	{
		if (srvcType == SM_SERVICE_TYPE_PLATFORM_SERVICE)
		{
			plfSrvcHlrId.hton ();
		}
	}
}; 


//----------------------------------------------------------------------------
// Structure	: Message header
// Description	: Common message header for all the inter-module messages
//----------------------------------------------------------------------------

struct CdModuleId
{
        CdHardwareUnitId          hwUnitId;
        CdServiceHandlerId        srvcHlrId;
	bool operator== (const CdModuleId& mid)
	{
		if ((hwUnitId == mid.hwUnitId) && 
			(srvcHlrId == mid.srvcHlrId))
		{
			return true;
		} else
		{
			return false;
		}
	}

	bool operator!= (const CdModuleId& mid)
	{
		if ((hwUnitId == mid.hwUnitId) && 
			(srvcHlrId == mid.srvcHlrId))
		{
			return false;
		} else
		{
			return true;
		}
	} // operator!=
	
	void operator= (const CdModuleId& mid)
	{
		hwUnitId = mid.hwUnitId;	
		srvcHlrId = mid.srvcHlrId;
	}
	
	void ntoh ()
	{
		srvcHlrId.ntoh ();
	}
	void hton ()
	{
		srvcHlrId.hton ();
	}

} __attribute__ ((packed));


//----------------------------------------------------------------------------
// Structure	: Message header
// Description	: Common message header for all the inter-module messages
//----------------------------------------------------------------------------

struct CdMessageHdr
{
	CdModuleId      dst;                    // destination module id
        CdModuleId      src;                    // source module id
        Uint16          msgLen;                 // message length
        Uint16          opcode;			// opcode

	void ntoh ()
	{
		dst.ntoh ();
		src.ntoh ();
		msgLen = ntohs (msgLen);	
		opcode = ntohs (opcode);	
	}
	void hton ()
	{
		dst.hton ();
		src.hton ();
		msgLen = htons (msgLen);
		opcode = htons (opcode);
	}

};

//----------------------------------------------------------------------------
// Structure	: Message 
// Description	: Base class for all inter-module messages
//----------------------------------------------------------------------------

struct CdMessage
{
	CdMessageHdr msgHdr;			// message header
	void initMsg (CdModuleId dst, CdModuleId src, Uint16 opcode,
				Uint16 msgLen)
	{
		msgHdr.dst = dst;
		msgHdr.src = src;
		msgHdr.opcode = opcode;
		msgHdr.msgLen = msgLen;
	}
	// CdMessage is polymorphic, so the appropriate message destructor will
	// be called.
	//	virtual ~CdMessage () { }

};

//----------------------------------------------------------------------------
// Enumeration	: Application id
// Description	: Application ids
//----------------------------------------------------------------------------

enum MgcApplicationType
{
	CD_APPLICATION_TYPE_MGC = 1,
	CD_APPLICATIONID_TYPE_INVALID
};


//=============================================================================
// <End>
//=============================================================================
#endif
