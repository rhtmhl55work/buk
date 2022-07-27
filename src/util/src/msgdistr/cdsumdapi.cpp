//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsumdapi.c
// Description	: Provides api for message distribution.
// Author       : Seema Chandak.
// Created On   : Tue Jul 22 10:49:58 IST 2003
// Last Modified: Tue Jul 22 10:49:58 IST 2003
// Modified By	: Vijaya Kumar
// Modified On	: Mon Aug  2 17:34:35 IST 2004
// 			Error Case Handling
//============================================================================

//============================================================================
//	Include Files.
//============================================================================
#include <sys/types.h>
#include <stdlib.h>
#include "cdsumd.h"

static CdSuMsgDistributor* cdSuMsgDistributor = NULL;

//---------------------------------------------------------------------------
// Name		: cdSuMdInit
// Description  : It is responsible for the creation and initialization of
// 		  the message distributor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdInit (const CdSuMdInitParam& param)
{
	bool retVal = false;
	if (cdSuMsgDistributor != NULL)
	{
		return false;
	} // if ()
	
	// create the MsgDistributor.
	cdSuMsgDistributor = new CdSuMsgDistributor (param);
	if (cdSuMsgDistributor != NULL)
	{
		retVal = cdSuMsgDistributor->start ();
		if (retVal == false)
		{
			delete cdSuMsgDistributor;
			cdSuMsgDistributor = NULL;
		} // if (retVal)
	} // if (cdSuMsgDistributor)
		
	return (retVal);

} // cdSuMdInit ()

//---------------------------------------------------------------------------
// Name		: cdSuMdTerminate
// Description  : It is responsible for the termination of message distributor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdTerminate ()
{
	if (cdSuMsgDistributor != NULL)
	{
		delete cdSuMsgDistributor;
		cdSuMsgDistributor = NULL;
		return true;
	}
	return (false);

} // cdSuMdTerminate ()

//---------------------------------------------------------------------------
// Name		: cdSuMdRegisterMid
// Description  : It is responsible for registration of a Module with
// 		  Message Distributor. This is used when the module
// 		  registering is in the same processor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdRegisterMid (const CdModuleId& modId)
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}
	
	if (cdSuMsgDistributor->registerMid (modId) == true)
	{
		return true;
	} else
	{	
		return false;
	} // if

} // cdSuMdRegisterMid ()

//---------------------------------------------------------------------------
// Name		: cdSuMdRegisterMid
// Description  : It is responsible for registration of a Module with
// 		  Message Distributor. This is used when the module
// 		  registering is in some other processor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdRegisterMid (const CdModuleId& modId, CdSuMdIpcpInfo ipcpInfo)
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}

	if (cdSuMsgDistributor->registerMid (modId, ipcpInfo) == true)
		return true;
	else
		return false;

} // cdSuMdRegisterMid ()

//---------------------------------------------------------------------------
// Name		: cdSuMdRegisterHwUint.
// Description  : It is responsible for registration of a Hardware Unit Id
//		  with the Message Distributor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdRegisterHwUnit (const CdHardwareUnitId& hwid, CdSuMdIpcpInfo ipcpInfo)
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}

	CdModuleId modId;
	// Initialize it to zero.
	memset (&modId, 0, sizeof (modId));
	modId.hwUnitId = hwid;

	if (cdSuMsgDistributor->registerMid (modId, ipcpInfo) == true)
		return true;
	else
		return false;

} // cdSuMdRegisterHwUnit () 

//---------------------------------------------------------------------------
// Name		: cdSuMdUnregisterMid ()
// Description  : It is responsible for deregistering a module from the
// 		  Message Distributor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdUnregisterMid (const CdModuleId& modId)
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}

	if (cdSuMsgDistributor->unregisterMid (modId) == true)
		return true;
	else
		return false;

} // cdSuMdDeregisterMid ()

//---------------------------------------------------------------------------
// Name		: cdSuMdSendMsg ()
// Description  : It is responsible for sending messages to the same or
// 		  different processor.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

//
bool cdSuMdSendMsg (CdMessage* msg, int priority)
//
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}

	if (cdSuMsgDistributor->sendMsg (msg, priority) == true)
		return true;
	else
		return false;

} // cdSuMdSendMsg ()

//---------------------------------------------------------------------------
// Name		: cdSuMdRecvMsg ()
// Description  : It is responsible for receiving messages.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdRecvMsg (CdMessage** msg, const CdModuleId& mid)
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}

	if (cdSuMsgDistributor->recvMsg (msg, mid) == true)
		return true;
	else
		return false;

} // cdSuMdRecvMsg ()

//---------------------------------------------------------------------------
// Name		: cdSuMdRecvMsgNonBlock ()
// Description  : It is responsible for receiving messages. The call is a
// 		  Non Blocking one.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool cdSuMdRecvMsgNonBlock (CdMessage** msg, const CdModuleId& mid)
{
	if (cdSuMsgDistributor == NULL)
	{
		return false;
	}

	if (cdSuMsgDistributor->recvMsgNonBlock (msg, mid) == true)
		return true;
	else
		return false;


} // cdSuMdRecvMsgNonBlock ()

//============================================================================
// <End of cdsumdapi.cpp>
//============================================================================
