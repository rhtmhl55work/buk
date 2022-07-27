//=============================================================================
// Internet Telephony Software - (c) Copyright 2001 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsumsgdistributor.cpp 
// Description	: CPP file for class CdSuMsgDistributor.
// Author       : 
// Created On   :  
// Last Modified: Tue Jul 22 16:02:23 IST 2003
// Modified By 	: Vijaya Kumar
// Modified On	: 1.Mon Aug  2 17:33:45 IST 2004
// 			Error handling
// 				- If Bind Fails
// 				- Sleep is removed
//		: 2.  Wed Apr 19 19:17:25 IST 2006
//			By : Vijaya Kumar
// 			Code has been changed to port over Fedora 4
//============================================================================

//----------------------------------------------------------------------------
// Include Files.
//----------------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include "cdsutrace.h"
#include "cdsuutiltypes.h"
#include "cdsumdapi.h"
#include "cdsumd.h"

void* cdsuMdUdpNwHlrStart(void* nhInitParam);
#ifdef __ECOS
char NwThreadStackSpace [1024 * 256];
#endif

//----------------------------------------------------------------------------
// Message Distributor is waiting on semaphore cdSuMdCondSem.
// Once this thread does basic operation it has to unlock
// semphore cdSuMdCondSem. Before unlocking correct value for
// cdSuMdUdpThrdStatus should be set.
//----------------------------------------------------------------------------
sem_t cdSuMdCondSem;
extern bool cdSuMdUdpThrdStatus;



//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::CdSuMsgDistributor
// Description  : Constructor of CdSuMsgDistributor class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuMsgDistributor::CdSuMsgDistributor (const CdSuMdInitParam& param)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	selfMid = param.selfMid;
	selfIpAddr = param.ipaddr;
	udpLpn = param.udpLpn;
	
	
	// Initialize nhInitParam.
	nhInitParam = new CdSuMdUdpNhInitParam;

	// Create the udpNhQueue.
	udpNhQueue = new CdSuTsQueue<CdMessage*> (CDSU_MD_MAX_NUM_MESSAGES);

	// Assign the UDP Nh ModuleId. 	
	nwHlrMid.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nwHlrMid.srvcHlrId.appSrvcHlrId.appType = CD_APPLICATION_TYPE_MGC;
	nwHlrMid.srvcHlrId.appSrvcHlrId.appId = 1;
	nwHlrMid.srvcHlrId.appSrvcHlrId.appModuleId = CDSU_MD_UDP_MID_NH;

	nhInitParam->selfIpAddr = selfIpAddr;
	nhInitParam->portNo = udpLpn;
	nhInitParam->nhMid = nwHlrMid;
	nhInitParam->udpNhQueue = udpNhQueue;

	// Create the Table.
	moduleInfoTable = new CdSuMultiByteHash <DestModInfo*> 
		((CdSuMultiByteHash <DestModInfo*>::TZ_251), sizeof (CdModuleId));

	// Initialize the mutex.
	pthread_mutex_init (&mdMutex, NULL);

	// Initialize Semaphore
	sem_init (&cdSuMdCondSem, 0, 1); 

} // CdSuMsgDistributor::CdSuMsgDistributor ()

//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::~CdSuMsgDistributor
// Description  : Destructor of CdSuMsgDistributor class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuMsgDistributor::~CdSuMsgDistributor ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	delete nhInitParam;	
	CdSuMultiByteHash <DestModInfo*>::iterator iter;

	iter = moduleInfoTable->begin ();
	
	if (cdSuMdUdpThrdStatus == true)
	{
		// Lock The Semaphore
		int rv;
		do 
		{
			rv = sem_wait (&cdSuMdCondSem);
		} while (rv == -1 && errno == EINTR);
		if (rv != 0)
		{
			CDSU_TRACE (1, "semaphore wait error\n");
		}
		CdSuMdUdpNhDelMsg* delnhmsg = new CdSuMdUdpNhDelMsg (nwHlrMid, selfMid);	
		udpNhQueue->add (delnhmsg);
		// wait till Semaphore is unlocked
		// Because Udp Network handler thread hast to die itself
		// Before dieying it will unlock.
		do 
		{
			rv = sem_wait (&cdSuMdCondSem);
		} while (rv == -1 && errno == EINTR);
		if (rv != 0)
		{
			CDSU_TRACE (1, "semaphore wait error\n");
		}
		sem_post (&cdSuMdCondSem);
	} // if (cdSuMdUdpThrdStatus)
	
	// Iterate through the table to clear it.
	for ( ; iter!= moduleInfoTable->end (); ++iter)
	{
		delete *iter;
	} // for ()

	// Delete the Table.
 	delete moduleInfoTable;

	// Delete the UDP Nh Queue.
	delete udpNhQueue;

	// Destroy the mutex.
	if (pthread_mutex_destroy (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex destroy error\n");
	} // if (pthread_mutex)
	
	// Destroy the semaphore
	sem_destroy (&cdSuMdCondSem);
} // CdSuMsgDistributor::CdSuMsgDistributor ()

//---------------------------------------------------------------------------
// Name		: start  
// Description  : Activates message distributor
// 		: This is added to fix bugs bs/34
// 		: 
// Inputs       : nothing
// Return Value : bool
// Side Effects : 
// Algorithm    : 1. start Udp Network Handler thread
// 		: 2. Wait on conditional variables to check Udp Network 
// 		:	Handler Opearation are success.
// 		: 3. If Success return true
// 		 	else Return false.
//---------------------------------------------------------------------------


bool CdSuMsgDistributor::start ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	// Lock The Semaphore And Udp Network Handler 
	// will unlock it.
	int rv;
	do 
	{
		rv = sem_wait (&cdSuMdCondSem);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
	}
	// Create the UDP Network Handler Thread.
	createUdpNwHlrThread (this->nhInitParam);

	// wait till Semaphore is unlocked
	do 
	{
		rv = sem_wait (&cdSuMdCondSem);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
	}
	if (cdSuMdUdpThrdStatus == false)
	{
		CDSU_TRACE (1, "Udp Network Handler Creation for Message Distributor Failed\n");	
	} // if (cdSuMdUdpThrdStatus == false)		
	sem_post (&cdSuMdCondSem);
	
	return cdSuMdUdpThrdStatus;
}
//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::registerMid
// Description  : Registers ModuleId when the module is in some other card
// 		  or in the same card, when the Entity registering wants to
// 		  specify the IPCP mechanism.
// Inputs       : The moduleId and IpcpInfo of the module registering.
// Return Value : Bool. Returns true if success, false if fails.
// Side Effects : 
// Algorithm    :
//			*) Set retVal to true.
//			*) Lock the mutex.
//			*) If the Module registering is in the same card 
//				*) If tsMsgQueue not equal to NULL retVal = false;
//			*) Else
//				*) Create the queue.
//				*) Add the queue to the ipcpInfo.
//			*) If retVal is true
//				*) Create the DestModInfo.
//				*) Set the ModuleId and ipcpInfo appropriately.
//			*) Unlock the mutex.
//			*) Return retVal.
//---------------------------------------------------------------------------

bool CdSuMsgDistributor::registerMid (const CdModuleId& mid, 
		const CdSuMdIpcpInfo& ipcpinfo)
{
	CDSU_TRACE (4, "CdSuMsgDistributor::registerMid\n");

	bool retVal = false;

	// Lock the mutex.
	if (pthread_mutex_lock (&mdMutex) != 0 )
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}

	// Create the destination Module Info.
	DestModInfo* destModInfo = NULL;
	CdModuleId modId = mid;
	if (moduleInfoTable->lookup (&modId, destModInfo) == true)
	{
		CDSU_TRACE (1, "Module already registered\n");
		retVal = false;
	} else
	{
		// The moduleId has not been registered already. Now, make an
		// appropriate entry in the table.
		switch (ipcpinfo.ipcpMech)
		{
		case CDSU_IPCP_MECH_INTRA:
			{
			// If the Module registering in the same card.
			if (ipcpinfo.u.tsMsgQueue != NULL)
			{
				destModInfo = new DestModInfo;
				destModInfo->modId = modId;
				destModInfo->ipcpInfo.ipcpMech = CDSU_IPCP_MECH_INTRA;
				destModInfo->ipcpInfo.u.tsMsgQueue = ipcpinfo.u.tsMsgQueue;
				if (moduleInfoTable->insert (&modId, destModInfo) == true)	
				{
					retVal = true;
				} else
				{
					CDSU_TRACE (1, "Insert in table fails\n");
					retVal = false;
				}

			} else
			{
				// Create a queue for the module.
				destModInfo = new DestModInfo;
				destModInfo->ipcpInfo.u.tsMsgQueue = new CdSuTsQueue <CdMessage*>
						(CDSU_MD_MAX_NUM_MESSAGES);
			
				if (moduleInfoTable->insert (&modId, destModInfo) == true)	
				{
					retVal = true;
				} else
				{
					CDSU_TRACE (1, "Insert in table fails\n");
					retVal = false;
				}
			}
			}
			break;

		case CDSU_IPCP_MECH_UDP:
			{
			destModInfo = new DestModInfo;
			destModInfo->modId = modId;
			destModInfo->ipcpInfo = ipcpinfo;
			if (moduleInfoTable->insert (&modId, destModInfo) == true)	
			{
				retVal = true;
			} else
			{
				CDSU_TRACE (1, "Insert in table fails\n");
				retVal = false;
			}
			}
			break;

		default:
			CDSU_TRACE (1, "Unsupported IPCP mechanism\n");
			break;
		} // switch
	}
		
	// Unlock the mutex.
	if (pthread_mutex_unlock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuMsgDistributor::registerMid ()

//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::registerMid
// Description  : Registers ModuleId when the module is within the same
// 		  processor.
// Inputs       : The moduleId of the module registering.
// Return Value : Bool. Returns true if success, false if fails.
// Side Effects : 
// Algorithm    :
//			*) Set retVal to true.
//			*) Lock the mutex.
//			*) If the registering module is not in the same card
//			   retVal = false.
//			*) If retVal is true 
//				*) Create a msgQueue.
//				*) Create DestModInfo appropriately.
//				*) Update moduleInfoTable.
//			*) Unlock the mutex.
//			*) Return retVal.
//---------------------------------------------------------------------------

bool CdSuMsgDistributor::registerMid (const CdModuleId& mid)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	bool retVal = false;

	// Check whether the Module is in the same processor.
	if (selfMid.hwUnitId != mid.hwUnitId)
	{
		CDSU_TRACE (1, "Registering module is in some other card.\n ");
		return (false);
	}

	// Lock the mutex.
	if (pthread_mutex_lock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}

	CdModuleId modId = mid;
	DestModInfo* destInfo = NULL;

	// Lookup the table to make sure that the module has not been
	// previously registered.
	if (moduleInfoTable->lookup (&modId, destInfo) == false)
	{
		// If module has not been registered yet.
		// Create the Destination Info.
		destInfo = new DestModInfo;
		destInfo->modId = mid;
		destInfo->ipcpInfo.ipcpMech = CDSU_IPCP_MECH_INTRA;
		destInfo->ipcpInfo.u.tsMsgQueue = new CdSuTsQueue <CdMessage*> (CDSU_MD_MAX_NUM_MESSAGES);

		// Insert in the moduleInfo Table.
		if (moduleInfoTable->insert (&modId, destInfo) == false)
		{
			CDSU_TRACE (1, "Insert in moduleInfoTable fails\n");
			retVal = false;
		} else
		{
			retVal = true;
		}
	} else
	{
		CDSU_TRACE (1, "Module already registered\n");
		retVal = false;
	}
		
	// Unlock the mutex.
	if (pthread_mutex_unlock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuMsgDistributor::registerMid ()

//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::unregisterMid ()
// Description  : Unregisters the ModuleId.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//			*) Lock the Mutex.
//			*) Lookup the desModInfoTable.
//			*) If lookup fail
//				*) retVal = false.
//			*) If retVal == true
//				*) If the module belongs to the same card.
//					*) delete the queue.
//				*) Delete the desModInfo.
//				*) Remove the ModuleId from the table.
//			*) Unlock the Mutex.
//---------------------------------------------------------------------------

bool CdSuMsgDistributor::unregisterMid (const CdModuleId& mid)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	bool retVal = true;

	// Lock the mutex.
	if (pthread_mutex_lock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}
	
	DestModInfo* destModInfo = NULL;
	
	CdModuleId modId = mid;
	// Search the destModInfoTable.
	if (moduleInfoTable->lookup (&modId, destModInfo) == false)
	{
		CDSU_TRACE (1, "ModuleInfoTable lookup fails, can't unregister Mid\n");
		retVal = false;
	} else
	{
		// If lookup succeeds
		if (destModInfo->ipcpInfo.ipcpMech ==  CDSU_IPCP_MECH_INTRA)		
		{
			// If the module to unregister sits in the same card.
			// Delete the Queue.
			if (destModInfo->ipcpInfo.u.tsMsgQueue != NULL)
				delete destModInfo->ipcpInfo.u.tsMsgQueue;
				destModInfo->ipcpInfo.u.tsMsgQueue = NULL;
		}
		
		// Delete the destModInfo.
		delete destModInfo;

		// Update the moduleInfoTable.
		if (moduleInfoTable->remove (&modId) == false)
			retVal = false;
	}

	// Unlock the mutex.
	if (pthread_mutex_unlock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuMsgDistributor::unregisterMid ()

//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::sendMsg ()
// Description  : This is used to send message.
// Inputs       : The message to be sent.
// Return Value : Bool. If msg is sent successfully return true else return false.
// Side Effects : 
// Algorithm    :
//			*) Lock the mutex.
//			*) Extract DestModId from the message.
//			*) Lookup the moduleInfo table.
//			*) If lookup is successful
//				*) Take action based on the ipcpMech.
//			*) Else if destHwUnitId same as selfMid.destHwUnitId
//				*) retVal = false. (Error: Can't send msg).
//			*) Else make the srvHlrId equal to 0 and search table again.
//			*) If success
//				*) Send msg based on ipcpMech. (INTRA is not allowed here.)
//			*) Else retVal = false (Error: Can't send msg).
//			*) Unlock the mutex.
//---------------------------------------------------------------------------

//
bool CdSuMsgDistributor::sendMsg (CdMessage* msg, int priority)
//
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	if (msg == NULL)
		return false;
	
	bool retVal = false;

	// Lock the mutex.
	if (pthread_mutex_lock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}

	CdModuleId destMid = msg->msgHdr.dst;
	DestModInfo* destModInfo = NULL;

	// Lookup the moduleInfoTable.	
	if (moduleInfoTable->lookup (&destMid, destModInfo) == true)
	{
		switch (destModInfo->ipcpInfo.ipcpMech)
		{
		case CDSU_IPCP_MECH_INTRA:
			{
			// If Message is for the module in the same processor.
			if (destModInfo->ipcpInfo.u.tsMsgQueue != NULL)
			{
				//
				if (priority == 0)
				{
					if (destModInfo->ipcpInfo.u.tsMsgQueue->add (msg) == true)
					{
						retVal = true;
					} else 
					{
						CDSU_TRACE (1, "MsgQueue is FULL\n");
						retVal = false;
					}
				

				} else if (priority == 1)
				{	
					if (destModInfo->ipcpInfo.u.tsMsgQueue->insert (msg) == true)
					{
						retVal = true;
					} else 
					{
						CDSU_TRACE (1, "MsgQueue is FULL\n");
						retVal = false;
					}

				} else 
				{
					CDSU_TRACE (1, "INVALID PRIORITY\n");
					CDSU_TRACE (1, "MsgQueue is FULL\n");
					CDSU_TRACE4(1, "SRC Module Id : srvcType:%d appType:%d appId:%d,appModuleId:%d,\n",msg->msgHdr.src.srvcHlrId.srvcType,msg->msgHdr.src.srvcHlrId.appSrvcHlrId.appType,msg->msgHdr.src.srvcHlrId.appSrvcHlrId.appId,msg->msgHdr.src.srvcHlrId.appSrvcHlrId.appModuleId);
					CDSU_TRACE4(1, "DEST Module Id : srvcType:%d appType:%d appId:%d,appModuleId:%d,\n",msg->msgHdr.dst.srvcHlrId.srvcType,msg->msgHdr.dst.srvcHlrId.appSrvcHlrId.appType,msg->msgHdr.dst.srvcHlrId.appSrvcHlrId.appId,msg->msgHdr.dst.srvcHlrId.appSrvcHlrId.appModuleId);
					retVal = false;
				}

				//

			}
			else
			{
				CDSU_TRACE (1, "MsgQueue is NULL\n");
				retVal = false;
			}
			}
			break;

		case CDSU_IPCP_MECH_UDP:
			{
			Uint32 len = msg->msgHdr.msgLen;
			msg->msgHdr.hton ();
			char* buf = new char [ len ];
			memcpy (buf, (char*) msg, len);
			CdSuMdUdpNhTxBufMsg* nhtbm = new 
				CdSuMdUdpNhTxBufMsg (nwHlrMid, selfMid, buf, len, 
				destModInfo->ipcpInfo.u.remoteUdpInfo.ipaddr,
				destModInfo->ipcpInfo.u.remoteUdpInfo.portNo);

			udpNhQueue->add (nhtbm);

			cdSuFreeMsgBuf (msg);

			retVal = true;
			}
			break;

		case CDSU_IPCP_MECH_TCP:
			{
			// If Message is for other processor and TCP is to be used.

			}
			break;

		case CDSU_IPCP_MECH_PROP:
			{
			// If Message is for other processor and proprietary method is to be used.

			}
			break;

		default:
			retVal = false;
			break;

		} // switch (ipcpMech)
	} else
	{
		if (destMid.hwUnitId == selfMid.hwUnitId)
		{
			CDSU_TRACE (1, "Module in same card not registered with the MsgDistributor\n");
			retVal = false;
		} else
		{
			CdModuleId modId;		
			memset (&modId, 0, sizeof (modId));
			modId.hwUnitId = destMid.hwUnitId;
			if (moduleInfoTable->lookup (&modId, destModInfo) == true)
			{
				// Extract IpcpMech.
				switch (destModInfo->ipcpInfo.ipcpMech)
				{
				case CDSU_IPCP_MECH_UDP:
					{
					Uint32 len = msg->msgHdr.msgLen;
					msg->msgHdr.hton ();
					char* buf = new char [ len ];
					memcpy (buf, (char*) msg, len);
					CdSuMdUdpNhTxBufMsg* nhtbm = new
					CdSuMdUdpNhTxBufMsg (nwHlrMid, selfMid, buf, len, 
						destModInfo->ipcpInfo.u.remoteUdpInfo.ipaddr,
						destModInfo->ipcpInfo.u.remoteUdpInfo.portNo);

					udpNhQueue->add (nhtbm);

					cdSuFreeMsgBuf (msg);

					retVal = true;
					}
					break;

				case CDSU_IPCP_MECH_TCP:
					{
					}
					break;

				case CDSU_IPCP_MECH_PROP:
					{
					}
					break;

				default:
					retVal = false;
					break;

				}

			} else
			{
				// Lookup fails for HardwareUnit Id also.
				CDSU_TRACE (1, "Module not registered with the MsgDistributor.\n");
				retVal = false;
			}
		}
	}
	
	// Unlock the mutex.
	if (pthread_mutex_unlock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex unlock error\n");
		retVal = false;
	}

	return (retVal);

} // CdSuMsgDistributor::sendMsg ()

//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::recvMsg ()
// Description  : This is used to receive messages.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuMsgDistributor::recvMsg (CdMessage** msg, const CdModuleId& modId)
{

	bool retVal = false;

	// Lock the mutex.
	if (pthread_mutex_lock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}
	
	// Extract the queue.
	DestModInfo* destModInfo = NULL;		
	CdModuleId mid = modId;
	if (moduleInfoTable->lookup (&mid, destModInfo) == true)
		retVal = true;
	else
		retVal = false;

	// Unlock the mutex.
	if (pthread_mutex_unlock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}
	
	if (retVal == true)
	{
		CdSuTsQueue<CdMessage*>* tsQueue = destModInfo->ipcpInfo.u.tsMsgQueue;
		bool result = tsQueue->popBlocking (*msg);
		if (result == false)
			retVal = false;
	}
	return (retVal);

} // CdSuMsgDistributor::recvMsg ()

//---------------------------------------------------------------------------
// Name		: CdSuMsgDistributor::recvMsgNonBlock ()
// Description  : This is used to receive messages by a NonBlocking call.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuMsgDistributor::recvMsgNonBlock (CdMessage** msg, const CdModuleId& modId)
{

	bool retVal = false;

	// Lock the mutex.
	if (pthread_mutex_lock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}
	
	// Extract the queue.
	DestModInfo* destModInfo = NULL;		
	CdModuleId mid = modId;
	if (moduleInfoTable->lookup (&mid, destModInfo) == true)
		retVal = true;
	else
		retVal = false;

	// Unlock the mutex.
	if (pthread_mutex_unlock (&mdMutex) != 0)
	{
		CDSU_TRACE (1, "mutex lock error\n");
		return (false);
	}
	
	if (retVal == true)
	{
		CdSuTsQueue <CdMessage*>* tsQueue = destModInfo->ipcpInfo.u.tsMsgQueue;
		bool result = tsQueue->popNonBlocking (*msg);
		if (result == false)
			retVal = false;
	}
	return (retVal);

} // CdSuMsgDistributor::recvMsgNonBlock ()

//---------------------------------------------------------------------------
// createUdpNwHlrThrd () 
// Description  : Creates UDP Network handler Thread.
// Inputs       : 
// Return Value : 
// Side Effects : None. 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuMsgDistributor::createUdpNwHlrThread (CdSuMdUdpNhInitParam* initParam)
{
#ifdef __ECOS
	pthread_attr_t NwAttr;
	pthread_attr_init (&NwAttr);
	pthread_attr_setstackaddr (&NwAttr,
		(void*)&NwThreadStackSpace [sizeof (NwThreadStackSpace)]);
	pthread_attr_setstacksize (&NwAttr, sizeof (NwThreadStackSpace));
	pthread_create (&udpNhThreadId, &NwAttr, 
				cdsuMdUdpNwHlrStart, initParam);
#elif sun
	pthread_attr_t NwAttr;
	pthread_attr_init (&NwAttr);
	pthread_attr_setscope (&NwAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&udpNhThreadId, &NwAttr, cdsuMdUdpNwHlrStart, initParam);
#else
	pthread_create (&udpNhThreadId, 0, cdsuMdUdpNwHlrStart, initParam);
#endif
	
	// db->setNwrkThreadPid (nwHlrThread_UDP);

} // CdSuMsgDistributor::createNwHlrThrd ()

//============================================================================
// <End of cdsumsgdistributor.cpp>
//============================================================================
