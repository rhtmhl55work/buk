//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudp.cpp
// Description	: 
// Author       : Vijaya Kumar
// Created On   : Tue Mar  9 15:57:01 IST 2004
// Last Modified: 1.Fri Jul 30 12:20:57 IST 2004
// 		: 	compiler warnings are removed
// 		: 2. Mon Aug 16 10:33:51 IST 2004
// 			New Reactor Algorithm [Activity Flag]
// 		: 3. Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

//----------------------------------------------------------------------------
// Include Files.
//----------------------------------------------------------------------------
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsumdapi.h"
#include "cdsunhudptypes.h"
#include "cdsunhudpapiparam.h"
#include "cdsunhudpdb.h"
#include "cdsunhudpmsg.h"
#include "cdsunhudpstatspkg.h"
#include "cdsuinthash.h"
#include "cdsunhudp.h"


//----------------------------------------------------------------------------
// Extern Declaration of Default Event Handler
//----------------------------------------------------------------------------

extern void cdSuNhUdpDefApplEventHlr (void*);

//----------------------------------------------------------------------------
// Public Methods Section
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: CdSuNhUdp () 
// Description  : Constructor of CdSuNhUdp Class.
// Inputs       : pointer to CdSuNhUdpDb
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdp::CdSuNhUdp (CdSuNhUdpDb* nhUdpDb)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// NH DB.
	nhDb = nhUdpDb;

	// Receive Buffer.
	if (nhUdpDb->getMaxBufferLen () > 0)
	{
		recvBuffer = new char [nhUdpDb->getMaxBufferLen ()];
	} else
	{
		recvBuffer = NULL;
	}
	
	// NH Statistic Package.
	statisticsPkg = new CdSuNhUdpStatisticsPkg ();

	// Chane NH state to created.
	changeState (CDSU_NHUDP_ST_CREATED);	

	// Max no. of Udp Port Handle (fds).
	maxUdpPortHandle = nhDb->getMaxNoOfUdpPort ();
	
	deleted = false;

	// Udp Port Info Table (Udp Port Info LookUp by udp port id.)
	udpPortId2UdpPortInfoTable = new CdSuIntHash <CdSuNhUdpPortInfo*> (CdSuIntHash<CdSuNhUdpPortInfo*>::TZ_2039);
	// Udp Port Info Table (Udp Port Info LookUp by udp handle.)
	handle2UdpPortInfoTable =  new CdSuIntHash <CdSuNhUdpPortInfo*> (CdSuIntHash<CdSuNhUdpPortInfo*>::TZ_2039);

} // CdSuNhUdp::CdSuNhUdp ()

//---------------------------------------------------------------------------
// Method	: ~CdSuNhUdp () 
// Description  : Destructor of CdSuNhUdp class.
// Inputs       : Nothing
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------


CdSuNhUdp::~CdSuNhUdp ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// Delete NH Statistic Package.
       	delete statisticsPkg;

	// Delete All UDP Port Info stored in the info table.
	for (CdSuIntHash<CdSuNhUdpPortInfo*>::iterator i = handle2UdpPortInfoTable->begin ();
		i != handle2UdpPortInfoTable->end (); ++i)
	{
		CdSuNhUdpPortInfo* info = *i;
		delete info;
	}
	 // Delete Udp Port Info Table.
	 delete udpPortId2UdpPortInfoTable;
	 delete handle2UdpPortInfoTable;

	 // Delete Receive Buffer.
	if (recvBuffer != NULL)
	{
	 	delete [] recvBuffer;
	}

} // CdSuNhUdp::~CdSuNhUdp ()

//---------------------------------------------------------------------------
// Method	: Constructor  of CdSuNhUdpPortInfo
// Description  : 
// Inputs       : Nothing
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdp::CdSuNhUdpPortInfo::CdSuNhUdpPortInfo ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// Local Address Fixed flag set to false.
	isLTAddrFixed = false;
	// Destination Address Fixed flag set to false.
	isDTAddrFixed = false;
	// Local ephemeral port flag set to false.
	isEphemeral = false;
	
} // CdSuNhUdp::CdSuNhUdpPortInfo::CdSuNhUdpPortInfo ()

//---------------------------------------------------------------------------
// Method	: run () 
// Description  : This will call the spawn function  defined in the base 
//		  class CdThread. The spawn  function in turn calls the 
//		  thread entry function.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::run ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	if (spawn (threadEntry, this) == CdSuThread::CDSU_ERROR)
	{
		CDSU_TRACE (1, "CdSuNhUdp thread spawn error\n");
	}

} // CdSuNhUdp::run ()

//---------------------------------------------------------------------------
// Method	: threadEntry () 
// Description  : This will call the start function here. This function
//		  hides all the intricacies of thread creation from
//		  the application.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void* CdSuNhUdp::threadEntry (void* objPtr)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// Wait here for Start control request.
	((CdSuNhUdp*) objPtr)->start ();
	return (NULL);

} // CdSuNhUdp::threadEntry ()

//---------------------------------------------------------------------------
// Method	: start () 
// Description  : This is the main function which keeps the object
//	  	  active. It will continuously wait for connection
//		  indication and data from remote and messages from
//		  the Ui, Mi Interface.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    : 
//---------------------------------------------------------------------------

void CdSuNhUdp::start ()
{
	CDSU_TRACE (5, "UDP Nh Started\n");
	
	// Send Init Response Message that UDP Nh Started.
	CdSuNhUdpMiInitRspEvent* initRspParam = new CdSuNhUdpMiInitRspEvent; 
	initRspParam->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHUDP_MI_INIT_RSP,
				CDSU_NHUDP_RSPCODE_SUCCESS);
	
	// call callback function.
	nhDb->getSmEventHlr () ( (void*)initRspParam);	
	
	
	while (1)
	{
		this->activityFlag = false;
		if (nhState == CDSU_NHUDP_ST_RUNNING)
		{
			//----------------------------------------------------
			//	// Step 1: Check reactor for any data.
			//----------------------------------------------------
			// printf (">>>>>>>>>>>CdSuNhUdp::start () before reactor\n<<<<<<<<<");
			reactor ();
		}
		
		//------------------------------------------------------------
		// Step 2: Handle Message from UI and MI.
		//------------------------------------------------------------
		CdMessage* rcvMsg;
		if (cdSuMdRecvMsgNonBlock (&rcvMsg, nhDb->getNhActorId ()) != false)
		{
			activityFlag = true;
			handleMsg (rcvMsg);
			// printf (">>>>>>>>>CdSuNhUdp::start () user Message Handled<<<<<<<<<<<\n");
		} // if
		
		if (activityFlag == false)
		{
			// Descheduling the thread.
			// printf (">>>>>>>>>>... No Activity Sleeping....<<<<<<<\n");
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = nhDb->getNoUdpHandleTmr (); 
			nanosleep (&sleepTime, NULL);
		}

	} // while (1)
	
} // CdSuNhUdp::start ()

//---------------------------------------------------------------------------
// Method	: reactor () 
// Description  :
//		* Takes as input maximum message size and application event hdlr.  
//		* Sets select time. 
//		* Creates buffer to write received data. 
//		* In a loop, if atleast one udp port is existing(local fixed), 
//		  clear all udpHandle (sockfds), and set it to current
//		  udpPortList. 
//		* Calls select. If there is some input to read, check for the
//		  active udpHandle and read from them. 
//		* Calls the appropriate application event handler.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhUdp::reactor ()
{
	// If there is no udp port to read then sleep.
	if (statisticsPkg->counters [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR] == 0)
	{
		/*struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = nhDb->getNoUdpHandleTmr (); 
		nanosleep (&sleepTime, NULL);
		*/
		// printf ("Nothing in reactor................\n");
		return false;
	}
	
	//--------------------------------------------------------------------
	// There is at least one Udp Port. (maxUdpPortHandle > 0)
	// 
	// * Set the FD list.
	// * Do a "select" on the active file descriptors.
	// * Check for activity
	//	- select <= 0		: No activity.
	//	- select = 0		: Some activity.
	//--------------------------------------------------------------------
	fd_set rfds;			// udpHandle.
	FD_ZERO (&rfds); 		// udpHandle.
	int maxRfd = 0;
	// Add handle to FDSET.
	for (CdSuIntHash<CdSuNhUdpPortInfo*>::iterator i = handle2UdpPortInfoTable->begin ();
		i != handle2UdpPortInfoTable->end (); ++i)
	{
		CdSuNhUdpPortInfo* info = *i;
		if (info != NULL)
		{
			FD_SET (info->handle, &rfds);
			if (maxRfd < info->handle)
			{
				// Set Max Read Fd.
				maxRfd = info->handle;
			}
		}
	}

	// Setting of select time.
	struct timeval tv;
	tv.tv_sec = 0; 
	tv.tv_usec = 0; // nhDb->getDataSelTmr();	// Time to call waiting for reading. (microsecs)
	int retval = select ((maxRfd+ 1), &rfds, NULL, NULL, &tv);
	//----------------------------------------------------------------------------
	// Check # 1 : retVal == -1
	// Action : look for error and return.
	//----------------------------------------------------------------------------
	if (retval == -1)
	{
		if (errno == EINTR)
		{
			CDSU_TRACE (1, "CdSuNhUdp::reactor (): Caught Non Blocked Signal.\n");
		} else 
		{
			CDSU_TRACE (1, "CdSuNhUdp::reactor (): Select error.\n");
			return false;
		}
	}

	//----------------------------------------------------------------------------
	// Check # 2 : retVal <= 0
	// Action : there was no activity return.
	//----------------------------------------------------------------------------
	if (retval <= 0)
	{
		return false;
	}

	//----------------------------------------------------------------------------
	// Check # 3 : retVal > 0
	// Action : Some activity, handle the inputs on this activity.
	//----------------------------------------------------------------------------
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;

	// Check for handle which are ready for read.
	for (CdSuIntHash<CdSuNhUdpPortInfo*>::iterator i = handle2UdpPortInfoTable->begin ();
		i != handle2UdpPortInfoTable->end (); ++i)
	{
		CdSuNhUdpPortInfo* info = *i;

		if (info != NULL)
		{
			// Check if there is any read even on fd.
			if (FD_ISSET (info->handle, &rfds)) 
			{
				CdSuIpv4Addr destAddr;
				CdSuIpv4UdpPortNo localPortNo;
				CdSuIpv4UdpPortNo destPortNo;
				Uint16 dlen = nhDb->getMaxBufferLen ();
				int readDataLen;
				activityFlag = true;
				//---------------------------------------------
				// *) If Udp Port is connected then there is
				//    no need of passing argument for 
				//    destination address and destination port.
				// *) else there is need of passing argument
				//    for getting destination address and port.
				//---------------------------------------------
				if (info->isDTAddrFixed)
				{
					// Get destination address and destination port.
					destAddr = info->destIpv4Addr;
				        destPortNo = info->destPortNo;	
					// Read Data (connected udp port read)
					if ( (readDataLen = readNoOfBytes (info, recvBuffer, dlen)) < 0)
					{
						// Set Fail Code.				
						failCode = CDSU_NHUDP_RSPFCODE_READERROR;	
					}						
				} else
				{
					// Read Data (Unconnected udp port read)
					if ( (readDataLen = readNoOfBytes (info, recvBuffer, dlen,
							destPortNo, destAddr)) < 0)
					{
						// Set Fail Code.
						failCode = CDSU_NHUDP_RSPFCODE_READERROR;
					}		
				} //if (info->isDTAddrFixed)
				
				// If readDataLen < 0 Then set it to 0 since datalen to be passed 
				// to application is unsigned short int.
				if(readDataLen < 0)
				{
					readDataLen = 0;
				} // if ()
				
				//---------------------------------------------
				// Update Stats counter. If Local port no is 
				// ephemeral then pass local port = 0 to 
				// update stats counter function else pass
				// actual port no.
				//---------------------------------------------
				if (info->isEphemeral && info->isLTAddrFixed)
				{
					localPortNo = 0;
				} else if (info->isLTAddrFixed)
				{
					localPortNo = info->selfPortNo;
				} else 
				{
					localPortNo = CDSU_NHUDP_INV_UDPPORT;
				}
				// Update statistics counter.
				// TODO:One more counter can be added for else
				if (info->isLTAddrFixed)
				{
					updateStatsPkgCounter (CDSU_NHUDP_UI_DATA_IND, localPortNo, info->destPortNo, failCode);
				}

				
				// Form Udp Port Key.
				CdSuNhUdpPortKey udpPortKey;
				udpPortKey.nhUdpInstId = nhDb->getNhInstId ();
				udpPortKey.nhUdpPortId = info->udpPortId;
				
				//---------------------------------------------
				// If local address is fixed then only pass
				// data to application otherwise discard data.
				//---------------------------------------------
				if (info->isLTAddrFixed)
				{
					// Build Data Ind Event Parameter.
					CdSuNhUdpUiDataIndEvent* evtParam = new CdSuNhUdpUiDataIndEvent ();
					
					// Initialize Event parameter depending upon fail or success.	
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						evtParam->init (udpPortKey, info->applActorId,
							       	recvBuffer, readDataLen,
								info->selfPortNo, info->selfIpv4Addr,
							       	destPortNo, destAddr );														
					} else
					{
						evtParam->init (udpPortKey, info->applActorId,
							       	recvBuffer, readDataLen,
						      		info->selfPortNo, info->selfIpv4Addr,
							       	destPortNo, destAddr, failCode );
					}
					// Call callback function.
					info->applEventHlr ( (void*)evtParam);
				} else
				{
					// Discard data .
					//TODO:
				} // if (info->isLTAddrFixed)
			} // (FD_ISSET (info->handle, &rfds)) 
		} else
		{
			CDSU_TRACE (1, "CdSuNhTcp::reactor(): Error! Got null entry in handle2UdpPortInfoTable.\n");
			//TODO: Report to manangement
			return false;
		} // if (info != NULL)
	} // for
	return (false);

} // CdSuNhUdp::reactor ()


//----------------------------------------------------------------------------
//			Private Methods Section
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// State Handlers.
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: handleMsg () 
// Description  : This will handle all the message which has come to NH from
//		  upper layer.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::handleMsg (CdMessage* msg)
{
	// CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	switch (nhState)
	{
	
	//--------------------------------------------------------------------
	// Handling Message In Created State.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_ST_CREATED:
		createdStHlr (msg);
		break;
		
	//--------------------------------------------------------------------
	// Handlin Message In Running State.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_ST_RUNNING:
		runningStHlr (msg);
		break;

	//--------------------------------------------------------------------
	// This case never occurs.
	//--------------------------------------------------------------------
	default:
		// Do nothing.
		break;

	} // switch 
	
	//--------------------------------------------------------------------
	// Free Msg Received.
	// If there is any internal data to be freed that will be
	// freed by corresponding handler.
	//--------------------------------------------------------------------
	CDSU_FREEMSGBUF (msg);
	
} // CdSuNhUdp::handleMsg ()

//---------------------------------------------------------------------------
// Method	: createdStHlr() 
// Description  : It handles message for state CDSU_NHUDP_ST_CREATED. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::createdStHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	switch (msg->msgHdr.opcode)
	{

	//--------------------------------------------------------------------
	// Handle Config Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_CFG_REQ:	
		configReqMsgHlr (msg);	
		break;

	//--------------------------------------------------------------------
	// Handle Control Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_CTRL_REQ:
		stCreatedCtrlReqMsgHlr (msg);
		break;

	//--------------------------------------------------------------------
	// Handle Delete NH Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_DELNH_REQ:
		delNhReqMsgHlr (msg);
		break;

	//--------------------------------------------------------------------
	// Handle Status Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_STATUS_REQ:
		statusReqMsgHlr (msg);
		break;

	//--------------------------------------------------------------------
	// Invalid Request In Created State. Send Negative 
	// Ack to Application.
	//--------------------------------------------------------------------
	default:
		outOfSeqMsgHlr (msg); 
		break;

	} // switch ()

} // CdSuNhUdp::createdStHlr ()

//---------------------------------------------------------------------------
// Method	: runningStHlr() 
// Description  : It handles message for state CDSU_NHUDP_ST_RUNNING . 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::runningStHlr (CdMessage* msg)
{
	// CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	switch (msg->msgHdr.opcode)
	{
	
	//--------------------------------------------------------------------
	// Handle Config Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_CFG_REQ:
		configReqMsgHlr (msg);
		break;

	//--------------------------------------------------------------------
	// Handle Control Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_CTRL_REQ:
		stRunningCtrlReqMsgHlr (msg);
		break;

	//--------------------------------------------------------------------
	// Handle Status Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_STATUS_REQ:
		statusReqMsgHlr (msg);
		break;

	//--------------------------------------------------------------------
	// Handle Statistics Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_STATISTICS_REQ:
		statsReqMsgHlr (msg);
		break;
		
	//--------------------------------------------------------------------
	// Handle Add Udp Port Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_ADD_UDPPORT_REQ:
		addUdpPortReqMsgHlr (msg);
		break;
		
	//--------------------------------------------------------------------
	// Handle Remove Udp Port Request. 
	//--------------------------------------------------------------------
		
	case CDSU_NHUDP_UI_REM_UDPPORT_REQ:
		remUdpPortReqMsgHlr (msg);
		break;
		
	//--------------------------------------------------------------------
	// Handle Data Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_DATA_REQ:
		dataReqMsgHlr (msg);
		break;
		
	//--------------------------------------------------------------------
	// Handle Send Request. 
	// To Be Removed.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_SEND_REQ:
		sendReqMsgHlr (msg);
		break;
	
	//--------------------------------------------------------------------
	// Invalid Request In Running State.
	// Send Neg Response to Application.
	//--------------------------------------------------------------------
	default:
		outOfSeqMsgHlr (msg); 
		break;

	} // switch ()

} // CdSuNhUdp::runningStHlr ()

//----------------------------------------------------------------------------
// Out Of Sequence Request Handler.
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: outOfSeqMsgHlr () 
// Description  : It handles out of sequence message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::outOfSeqMsgHlr (CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	
	//--------------------------------------------------------------------
	//  Delete Nh Request : Send Neg Response.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_DELNH_REQ:
		{
		// Build Del Nh Response Event Param.	
		CdSuNhUdpMiDelNhRspEvent*  delNhRspEvent = new CdSuNhUdpMiDelNhRspEvent;
		delNhRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHUDP_RSPFCODE_INV_OP);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)delNhRspEvent);	
		}
		break;
	
	//--------------------------------------------------------------------
	// Statistics Request : Send Neg Response.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_MI_STATISTICS_REQ:
		{
		// Build Statistics Response Event Param.
		CdSuNhUdpMiStsRspEvent* stsRspEvent = new CdSuNhUdpMiStsRspEvent; 
		stsRspEvent->init (nhDb->getNhInstId (), nhDb->getSmActorId (),
						CDSU_NHUDP_RSPFCODE_INV_OP);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)stsRspEvent);	
		}
		break;
	
	//--------------------------------------------------------------------
	// Add Udp Port Request: Send Neg Response.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_ADD_UDPPORT_REQ:
		{
		CdSuNhUdpUiAddUdpPortReqMsg* reqMsg = (CdSuNhUdpUiAddUdpPortReqMsg*) msg;
		// Form Udp Port Key.
		CdSuNhUdpPortKey udpPortKey;
		udpPortKey.nhUdpPortId = reqMsg->udpPortId;
		udpPortKey.nhUdpInstId = reqMsg->addUdpPortReqParam.nhUdpInstId;
		
		// Build Add Udp Port Response Event Param.
		CdSuNhUdpUiAddUdpPortRspEvent* evtParam = new CdSuNhUdpUiAddUdpPortRspEvent ();
		// Callback Function Pointer : If Callback Function pointer is invalid then set 
		// Default Event Handler.
		if (reqMsg->addUdpPortReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
		{
			reqMsg->addUdpPortReqParam.applEventHlr = (CdSuNhUdpEventHlr) cdSuNhUdpDefApplEventHlr;
		}
		evtParam->init (udpPortKey,
			       	reqMsg->addUdpPortReqParam.applActorId,
			       	reqMsg->addUdpPortReqParam.localPortNo,
			       	CDSU_NHUDP_RSPFCODE_INV_OP);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		reqMsg->addUdpPortReqParam.applEventHlr ((void*) evtParam);
		}
		break;
	
	//--------------------------------------------------------------------
	// Rem Udp Port Request : Send Neg Response.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_REM_UDPPORT_REQ:
		{
		CdSuNhUdpUiRemUdpPortReqMsg* reqMsg = (CdSuNhUdpUiRemUdpPortReqMsg*) msg; 
		// Callback Function Pointer : If Invalid function pointer then set Default Event Handler.
		if (reqMsg->remUdpPortReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
		{
			reqMsg->remUdpPortReqParam.applEventHlr = (CdSuNhUdpEventHlr) cdSuNhUdpDefApplEventHlr;
		}
		// Build Remove Udp Port Response Event Param.
		CdSuNhUdpUiRemUdpPortRspEvent* evtParam = new CdSuNhUdpUiRemUdpPortRspEvent ();
		evtParam->init (reqMsg->remUdpPortReqParam.udpPortKey,
			       	reqMsg->remUdpPortReqParam.applActorId,
			       	CDSU_NHUDP_RSPFCODE_INV_OP);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		reqMsg->remUdpPortReqParam.applEventHlr ((void*) evtParam);
		}
		break;
	
	//--------------------------------------------------------------------
	// Data Request : Send Neg Response.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_DATA_REQ:
		{
		CdSuNhUdpUiDataReqMsg* reqMsg = (CdSuNhUdpUiDataReqMsg*) msg; 
		// free data Pointer.
		delete [] reqMsg->dataReqParam.dataPtr;
		
		// Callback Function Pointer : If Invalid function pointer then set Default Event Handler.
		if (reqMsg->dataReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
		{
			reqMsg->dataReqParam.applEventHlr = (CdSuNhUdpEventHlr) cdSuNhUdpDefApplEventHlr;
		}

		CdSuNhUdpUiDataRspEvent* evtParam = new  CdSuNhUdpUiDataRspEvent ();
		evtParam->init (reqMsg->dataReqParam.udpPortKey,
			       	reqMsg->dataReqParam.applActorId,
			       	reqMsg->dataReqParam.opaqueData,
			       	CDSU_NHUDP_RSPFCODE_INV_OP);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		reqMsg->dataReqParam.applEventHlr ((void*) evtParam);
		}
		break;
	
	//--------------------------------------------------------------------
	// Send Request : Send Neg Response.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_SEND_REQ:
		{
		CdSuNhUdpUiSendReqMsg* reqMsg = (CdSuNhUdpUiSendReqMsg*) msg; 
		// Free data pointer.
		delete [] reqMsg->sendReqParam.dataPtr;
	       		
		// Callback Function Pointer : If Invalid Function Pointer Then Set Default Event Hlr.
		if (reqMsg->sendReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
		{
			reqMsg->sendReqParam.applEventHlr = (CdSuNhUdpEventHlr) cdSuNhUdpDefApplEventHlr;
		}
		// Build Send Response Event param.
		CdSuNhUdpUiSendRspEvent* evtParam = new  CdSuNhUdpUiSendRspEvent ();
		evtParam->init (reqMsg->sendReqParam.nhUdpInstId,
			       	reqMsg->sendReqParam.applActorId,
			       	reqMsg->sendReqParam.opaqueData,
			       	CDSU_NHUDP_RSPFCODE_INV_OP);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		reqMsg->sendReqParam.applEventHlr ((void*) evtParam);

		}
		break;
		
	//--------------------------------------------------------------------
	// Invalid Request
	//--------------------------------------------------------------------
	default:
		// Discard.
		break;

	} // switch ()
	
} // void CdSuNhUdp::outOfSeqMsgHlr (CdMessage* msg)

//----------------------------------------------------------------------------
// SM Message Handlers.
// --------------------
//----------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Method	: configReqMsgHlr () 
// Description  : It handles configure message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::configReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	// TODO:

} // CdSuNhUdp::configReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: delNhReqMsgHlr () 
// Description  : It handles delete NH message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::delNhReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiDelNhReqMsg* delNhReqMsg  = (CdSuNhUdpMiDelNhReqMsg*) msg;

	// close all udp handle.
	for (CdSuIntHash<CdSuNhUdpPortInfo*>::iterator i = handle2UdpPortInfoTable->begin ();
		i != handle2UdpPortInfoTable->end (); ++i)
	{
		CdSuNhUdpPortInfo* info = *i;
		if (info != NULL)
		{
		 	close (info->handle);
		}
	}

	// Set Deleted Flag.	
	deleted = true;
	
	//--------------------------------------------------------------------
	// This Message Should be freed Here. Since this fucntion 
	// executes pthread_exit control doesn't goes to main handler.
	// So other than all message are freed in main handler.
	//--------------------------------------------------------------------
	CDSU_FREEMSGBUF (delNhReqMsg);
	
	// Build Delete Response  Event.
	CdSuNhUdpMiDelNhRspEvent*  delNhRspEvent = 
		 	new CdSuNhUdpMiDelNhRspEvent;
	delNhRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId ());
	//--------------------------------------------------------------------
	// call callback function.
	//--------------------------------------------------------------------
	nhDb->getSmEventHlr () ( (void*)delNhRspEvent);	

	// Exit.
	pthread_exit ((void*) NULL);

} // CdSuNhUdp::delNhReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: stCreatedCtrlReqMsgHlr () 
// Description  : It handles Control NH message in CREATED state. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::stCreatedCtrlReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiCtrlReqMsg* ctrlMsg = (CdSuNhUdpMiCtrlReqMsg*) msg;
	CdSuNhUdpMiCtrlRspEvent* ctrlRspEvent = new CdSuNhUdpMiCtrlRspEvent; 
	
	switch (ctrlMsg->ctrlReqParam.subOpcode)
	{
	
	//--------------------------------------------------------------------
	// Start NH Handler.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_SO_START_REQ:
		{
		// Change the state.
		changeState (CDSU_NHUDP_ST_RUNNING);
		//------------------------------------------------------------
		// call callback function.
		//------------------------------------------------------------
		ctrlRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHUDP_RSPCODE_SUCCESS,
				CDSU_NHUDP_SO_START_RSP);
		
		//------------------------------------------------------------
		// Call Callback functions.
		//------------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)ctrlRspEvent);	
		} // case 
		break;
		
	//--------------------------------------------------------------------
	// Invalid Control Request.
	//--------------------------------------------------------------------
	default: 
		{
		// Alaready in Created State. Build Neg Response Event Param.
		ctrlRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHUDP_RSPCODE_FAIL,
				getCorrectSubOpcodeRsp (ctrlMsg->ctrlReqParam.subOpcode));
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)ctrlRspEvent);
		} // case
		break;
	} // switch ()
	
} // CdSuNhUdp::ctrlReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: stRunningCtrlReqMsgHlr () 
// Description  : It handles Control NH message in RUNNING state. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::stRunningCtrlReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiCtrlReqMsg* ctrlMsg = (CdSuNhUdpMiCtrlReqMsg*) msg;
	CdSuNhUdpMiCtrlRspEvent* ctrlRspEvent = new CdSuNhUdpMiCtrlRspEvent; 
	
	switch (ctrlMsg->ctrlReqParam.subOpcode)
	{
		
	//--------------------------------------------------------------------
	// Stop Nh Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_SO_STOP_REQ:
		{
		// Change the state.
		changeState (CDSU_NHUDP_ST_CREATED);
		// Build Response Event Param.
		ctrlRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHUDP_RSPCODE_SUCCESS,
				CDSU_NHUDP_SO_STOP_RSP);
		//------------------------------------------------------------
		// call callback function.
		//------------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)ctrlRspEvent);	
		} // case 
		break;
		
	//--------------------------------------------------------------------
	// Invalid Control Message.
	//--------------------------------------------------------------------
	default: 
		{
		// Alaready in Created State.
		// Build Neg Response Event Param.
		ctrlRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHUDP_RSPCODE_FAIL,
				getCorrectSubOpcodeRsp (ctrlMsg->ctrlReqParam.subOpcode));
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)ctrlRspEvent);
		} // case
		break;
	} // switch ()
	
} // CdSuNhUdp::ctrlReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: statusReqMsgHlr () 
// Description  : It handles delete NH message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::statusReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	// CdSuNhUdpMiStatusReqMsg* statusReqMsg = (CdSuNhUdpMiStatusReqMsg*) msg;
	CdSuNhUdpMiStatusRspEvent* statusRspEvent = new CdSuNhUdpMiStatusRspEvent; 
		
	// Build Status Response Event Param.
	statusRspEvent->init (nhDb->getNhInstId (),
			nhDb->getSmActorId (),
			CDSU_NHUDP_RSPCODE_SUCCESS,
			nhState);
	//--------------------------------------------------------------------
	// call callback function.
	//--------------------------------------------------------------------
	nhDb->getSmEventHlr () ( (void*)statusRspEvent);	
	
} // CdSuNhUdp::statusReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: statsReqMsgHlr () 
// Description  : It handles delete NH message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::statsReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	//CdSuNhUdpMiStsReqMsg* stsReqMsg = (CdSuNhUdpMiStsReqMsg*) msg;
	
	CdSuNhUdpMiStsRspEvent* stsRspEvent = 
					new CdSuNhUdpMiStsRspEvent; 
	//--------------------------------------------------------------------
	// call callback function.
	//--------------------------------------------------------------------
	stsRspEvent->init (nhDb->getNhInstId (),
			nhDb->getSmActorId (),
			statisticsPkg->counters);
	nhDb->getSmEventHlr () ( (void*)stsRspEvent);	

	// Free rxvd msg buf.
	// CDSU_FREEMSGBUF (stsReqMsg);

} // CdSuNhUdp::statsReqMsgHlr ()

//----------------------------------------------------------------------------
// Application Message Handlers.
//----------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Method	: addUdpPortMsgHlr () 
// Description  : It handles add udp port request message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::addUdpPortReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	CdSuNhUdpUiAddUdpPortReqMsg* addUdpPortMsg =  (CdSuNhUdpUiAddUdpPortReqMsg*) msg;
	CdSuIpv4UdpPortNo lPortNo = addUdpPortMsg->addUdpPortReqParam.localPortNo;

	// If Event Hlr is invalid then set it to default event hlr.
	if (addUdpPortMsg->addUdpPortReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
	{
		addUdpPortMsg->addUdpPortReqParam.applEventHlr = cdSuNhUdpDefApplEventHlr;
	}
	
	// Check for UDP port overflow.
	if (maxUdpPortHandle == (int)statisticsPkg->counters [ CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR ])
	{
		failCode = CDSU_NHUDP_RSPFCODE_UDPPORT_OVERFLOW;
	}

	// If Operation Not Fail then Add Udp Port
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		failCode = addUdpPort (addUdpPortMsg->udpPortId,
				addUdpPortMsg->addUdpPortReqParam.localPortNo,
				addUdpPortMsg->addUdpPortReqParam.localAddr,
				addUdpPortMsg->addUdpPortReqParam.destPortNo,
			       	addUdpPortMsg->addUdpPortReqParam.destAddr,
				addUdpPortMsg->addUdpPortReqParam.applActorId,
		       		addUdpPortMsg->addUdpPortReqParam.applEventHlr);

	}
	
	// Form Udp Port Key .
	CdSuNhUdpPortKey udpPortKey;
	udpPortKey.nhUdpInstId = nhDb->getNhInstId ();
	udpPortKey.nhUdpPortId = addUdpPortMsg->udpPortId;
	CdSuNhUdpUiAddUdpPortRspEvent* evtParam = new CdSuNhUdpUiAddUdpPortRspEvent ();
	
	// Build Add Udp Port Response Event Parameter.
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		evtParam->init (udpPortKey, addUdpPortMsg->addUdpPortReqParam.applActorId,
			       addUdpPortMsg->addUdpPortReqParam.localPortNo);
	} else
	{
		evtParam->init (udpPortKey, addUdpPortMsg->addUdpPortReqParam.applActorId,
			       addUdpPortMsg->addUdpPortReqParam.localPortNo, failCode);
	}
	
	// Update Statistics counter.
	updateStatsPkgCounter (CDSU_NHUDP_UI_ADD_UDPPORT_REQ,
			lPortNo, addUdpPortMsg->addUdpPortReqParam.destPortNo,
		       	failCode);
	
	// Call callback function.
	addUdpPortMsg->addUdpPortReqParam.applEventHlr ((void*) evtParam);
	
} // CdSuNhUdp::addUdpPortReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: remUdpPortMsgHlr () 
// Description  : It handles rem udp port request message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::remUdpPortReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	CdSuIpv4UdpPortNo localPortNo;
	CdSuIpv4UdpPortNo destPortNo;
	CdSuNhUdpPortInfo* udpPortInfo;
	CdSuNhUdpUiRemUdpPortReqMsg* remUdpPortMsg =  (CdSuNhUdpUiRemUdpPortReqMsg*) msg;

	// If Callback Function Pointer is Invalid Then Set to Default Event Handler
	if (remUdpPortMsg->remUdpPortReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
	{
		remUdpPortMsg->remUdpPortReqParam.applEventHlr = cdSuNhUdpDefApplEventHlr;
	}
	
	// Get Udp Port Info.	
	if (udpPortId2UdpPortInfoTable->lookup (remUdpPortMsg->remUdpPortReqParam.udpPortKey.nhUdpPortId, udpPortInfo) != true)
	{
		CDSU_TRACE (5, "CdSuNhUdp::remUdpPort Udp Port Not Found\n" );
		failCode = CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST;
	} else
	{
		//Get Local and Destination Udp Port
		destPortNo = udpPortInfo->destPortNo;
		if (udpPortInfo->isEphemeral && udpPortInfo->isLTAddrFixed)
		{
			localPortNo = 0;
		} else if (udpPortInfo->isLTAddrFixed)
		{
			localPortNo = udpPortInfo->selfPortNo;
		} else
		{
			localPortNo = CDSU_NHUDP_INV_UDPPORT;
		}	
	}
	
	// If Operation Not Fail Then Remove Udp Port
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{	
		failCode = remUdpPort (remUdpPortMsg->remUdpPortReqParam.udpPortKey.nhUdpPortId);
	} // if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)

	// Update Statistics counter.
	updateStatsPkgCounter (CDSU_NHUDP_UI_REM_UDPPORT_REQ,
			localPortNo, destPortNo,
		       	failCode);
	
	// Build Rem Udp Port Rsp Event Param.
	CdSuNhUdpUiRemUdpPortRspEvent* evtParam = new CdSuNhUdpUiRemUdpPortRspEvent ();
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		evtParam->init (remUdpPortMsg->remUdpPortReqParam.udpPortKey,
			       	remUdpPortMsg->remUdpPortReqParam.applActorId);
	} else
	{
		evtParam->init (remUdpPortMsg->remUdpPortReqParam.udpPortKey,
			       	remUdpPortMsg->remUdpPortReqParam.applActorId,
				failCode);
	}
	
	//--------------------------------------------------------------------
	// Call Callback Function
	//--------------------------------------------------------------------
	remUdpPortMsg->remUdpPortReqParam.applEventHlr ((void*) evtParam);
	
} // CdSuNhUdp::remUdpPortReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: dataReqMsgHlr () 
// Description  : It handles send data request message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::dataReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpPortInfo* udpPortInfo = NULL;
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	CdSuIpv4UdpPortNo localPortNo;
	CdSuIpv4UdpPortNo destPortNo;
	CdSuNhUdpUiDataReqMsg* dataReqMsg =  (CdSuNhUdpUiDataReqMsg*) msg;
	
	// If Callback function pointer is invalid then set to Default Event Handler.
	if (dataReqMsg->dataReqParam.applEventHlr == (CdSuNhUdpEventHlr)CDSU_NHUDP_INV_UDPEVENTHLR)
	{
		dataReqMsg->dataReqParam.applEventHlr = cdSuNhUdpDefApplEventHlr;
	}
	
	//--------------------------------------------------------------------
	// Check if udp port id is 0(Always opened Udp Port Type)
	// *) If not 0 then get udp port info to get destination and local port no.
	// then Call Send datagram Function (which expects added udp port)
	// *) Else Call Send Datagram Function (which opens udp socket temporaly
	// to send data)
	//--------------------------------------------------------------------
	if (dataReqMsg->dataReqParam.udpPortKey.nhUdpPortId != 0)
	{
		// get udp port info.
		if (udpPortId2UdpPortInfoTable->lookup (dataReqMsg->dataReqParam.udpPortKey.nhUdpPortId, udpPortInfo) != true)
		{
			CDSU_TRACE (5, "CdSuNhUdp::remUdpPort Udp Port Not Found\n" );
			failCode = CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST;
			udpPortInfo = NULL;
		} else	 
		{
			// Get Destination and Local Port No.
			destPortNo = udpPortInfo->destPortNo;
			if (udpPortInfo->isEphemeral && udpPortInfo->isLTAddrFixed == true)
			{
				localPortNo = 0;
			} else if (udpPortInfo->isLTAddrFixed == true)
			{
				localPortNo = udpPortInfo->selfPortNo;
			} else 
			{
				localPortNo = CDSU_NHUDP_INV_UDPPORT;
			}
	
			// Send Data.
			failCode = sendDatagram (dataReqMsg->dataReqParam.udpPortKey.nhUdpPortId,
							dataReqMsg->dataReqParam.dataPtr,
							dataReqMsg->dataReqParam.dataLen,
							dataReqMsg->dataReqParam.destPortNo,
							dataReqMsg->dataReqParam.destAddr);	
		}
		// Update Statistics counter.
		updateStatsPkgCounter (CDSU_NHUDP_UI_DATA_REQ,
					localPortNo, destPortNo,
					failCode);
	} else
	{
		// Send Data
		failCode = sendDatagram (dataReqMsg->dataReqParam.dataPtr,
						dataReqMsg->dataReqParam.dataLen,
						dataReqMsg->dataReqParam.destPortNo,
						dataReqMsg->dataReqParam.destAddr);	
			// Update Statistics counter.
		updateStatsPkgCounter (CDSU_NHUDP_UI_DATA_REQ,
						failCode);
	}
	
	// Free Data Pointer.
	delete [] dataReqMsg->dataReqParam.dataPtr;

	// Build data response event and
	// Call callback function if ack 
	// required by application
 	if (dataReqMsg->dataReqParam.isAckReq)
	{
		CdSuNhUdpUiDataRspEvent* evtParam = new  CdSuNhUdpUiDataRspEvent ();
		if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
		{
			evtParam->init (dataReqMsg->dataReqParam.udpPortKey,
						dataReqMsg->dataReqParam.applActorId, 
						dataReqMsg->dataReqParam.opaqueData);
		} else 
		{
			evtParam->init (dataReqMsg->dataReqParam.udpPortKey,
					dataReqMsg->dataReqParam.applActorId,
					dataReqMsg->dataReqParam.opaqueData,
					failCode);
		}
		//  call callback function.  
		dataReqMsg->dataReqParam.applEventHlr ((void*) evtParam);
	} // if (isAckReq)
	
} // CdSuNhUdp::remUdpPortReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: sendReqMsgHlr () 
// Description  : It handles send data request message
// 		: without adding udp port 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::sendReqMsgHlr (CdMessage* msg)
{
	
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// TO Be Removed.	
	CdSuNhUdpUiSendReqMsg* sendReqMsg =  (CdSuNhUdpUiSendReqMsg*) msg;
	//sendDatagram ();
	// delete data pointer
	delete [] sendReqMsg->sendReqParam.dataPtr;
} // CdSuNhUdp::sendReqMsgHlr ()

//---------------------------------------------------------------------------
// Other Functions.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: remUdpPort () 
// Description  : This method removes a UdpPort from Info Table and Closes 
// 		: Udp Port.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpRspFailCode CdSuNhUdp::remUdpPort (const CdSuNhUdpPortId& udpPortId)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpPortInfo *udpPortInfo = NULL;
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	
	// lookup in udpPortId2UdpPortInfoTable and get udp port info.
	if (udpPortId2UdpPortInfoTable->lookup (udpPortId, udpPortInfo) != true)
	{
		CDSU_TRACE (5, "CdSuNhUdp::remUdpPort Udp Port Not Found\n" );
		failCode = CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST;
	} else
	{
		// Remove Entry from handle2UdpPortInfoTable 
		// and udpPortId2UdpPortInfoTable.
		if (udpPortCleanUp (udpPortInfo) != true)
		{
			CDSU_TRACE (1, "CdSuNhUdp::remUdpPort UDP PORT CLOSED fail\n");
			failCode = CDSU_NHUDP_RSPFCODE_UNKNOWNERROR;			
		} else
		{		
			// Close udp port handle(socket)
			if ( (close (udpPortInfo->handle)) == -1)
			{
				// Strange Error. 
				CDSU_TRACE (1, "CdSuNhUdp::remUdpPort UDP PORT CLOSED fail\n");
				failCode = CDSU_NHUDP_RSPFCODE_UNKNOWNERROR;
			} else
			{
				CDSU_TRACE (3, "CdSuNhUdp::remUdpPort UDP PORT CLOSED Successfully\n");
			}

			// Free Udp Port
			delete udpPortInfo;
		} // if (udpPortCleanUp)
	} // if (udpPortId2UdpPortInfoTable->lookup (udpPortInfo) != true)
	
	return failCode;
	
} // void CdSuNhUdp::remUdpPort ()

//---------------------------------------------------------------------------
// Method	: changeState () 
// Description  : To change state of NH
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::changeState (CdSuNhUdpState udpNhState)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	// Set State.
	nhState = udpNhState;
} // CdSuUdp::changeState ()

//---------------------------------------------------------------------------
// Method	: udpPortCleanUp () 
// Description  : This method removes udp port info from tables.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhUdp::udpPortCleanUp (const CdSuNhUdpPortInfo* info)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	// Remove Udp Port Info.
	if (udpPortId2UdpPortInfoTable->remove (info->udpPortId) != true)
	{
		// Strange Error. 
		return false;
	} else
	{
		// Remove Udp Port Info from second Table.
		if (handle2UdpPortInfoTable->remove (info->handle) != true)
		{
			//Strange Error. 
			return false;
		} else
		{
			return true;
		} // if (udpPortId2UdpPorInfoTable->remove (udpPortInfo->udpPortId) != true)
		
	} // if (udpPortId2UdpPorInfoTable->remove (udpPortInfo->udpPortId) != true)

} // CdSuNhUdp::udpPortCleanUp ()

//---------------------------------------------------------------------------
// Method	: addUdpPort () 
// Description  : This method adds a UdpPort.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpRspFailCode CdSuNhUdp::addUdpPort (const CdSuNhUdpPortId& udpPortId,
			    CdSuIpv4UdpPortNo& localPortNo, 
		            const CdSuIpv4Addr& localAddr, 
		   	    const CdSuIpv4UdpPortNo& destPortNo, 
		            const CdSuIpv4Addr& destAddr,
			    const CdSuActorId& applActorId,
			    CdSuNhUdpEventHlr applEventHlr) 
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
		
	CdSuNhUdpHandle udpHandle = CDSU_NHUDP_INV_UDPHANDLE;
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	CdSuIpv4UdpPortNo lPortNo = localPortNo;
	CdSuNhUdpPortInfo* udpPortInfo = NULL;
	//--------------------------------------------------------------------
	// Phase 1: Prepare the socket and bind. (Deal with socket operations)
	//--------------------------------------------------------------------
	
	// Create Udp handle 
	if ((udpHandle = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		CDSU_TRACE (1, "CdSuNhUdp::addUdpPort(): Socket creation error.\n");	
		failCode = CDSU_NHUDP_RSPFCODE_OPENERROR;
	} else
	{
		if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
		{
			// So Local Fixed
			// Create address structure
			struct sockaddr_in servaddr;
			memset (&servaddr,0, sizeof (servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = localAddr;
			servaddr.sin_port = htons (localPortNo);
			
			// Bind  Port to Socket
			CDSU_TRACE3 (5, "CdSuNhUdp::addUdpPort(): Binding socket %d to %s:%d\n", udpHandle, inet_ntoa (servaddr.sin_addr), localPortNo);
			
			if ((bind (udpHandle, (sockaddr *) & servaddr, sizeof (servaddr))) == -1)
			{
				CDSU_TRACE (1, "socket bind error\n");	
				failCode = CDSU_NHUDP_RSPFCODE_BINDERROR;	
				// Bind Error So close the socket
				close (udpHandle); 
			} else
			{
				// Get The  Port Bound by system (ephemeral)
				if (localPortNo == 0)
				{
					#ifdef sun
						int len = sizeof (servaddr);
					#else 
						socklen_t len = sizeof (servaddr);
					#endif
					if ((getsockname (udpHandle,(sockaddr *) &servaddr, &len)) == -1)
					{
						// TODO : Error Handling But It never happens
					} else
					{
						localPortNo = ntohs (servaddr.sin_port);
						CDSU_TRACE1 (5, "CdSuNhUdp::addUdpPort(): Bound Port =%d\n", localPortNo);
					}
				} 
				
			}// if ((bind (udpHandle, (sockaddr *) & servaddr, sizeof (servaddr))) == -1)
		} // if (localPortNo != CDSU_NHUDP_INV_UDPPORTID)
		
	} // if ((udpHandle = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
	
	// If operation not failed then continue in other operation.
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		// If Destination Port No is not invalid then
		// User wants connected udp socket.
		if (destPortNo != CDSU_NHUDP_INV_UDPPORT)
		{
			// If Dest Port No is 0 then fail.
			if (destPortNo == 0)
			{
				failCode = CDSU_NHUDP_RSPFCODE_INVPARAM;
				close (udpHandle);	
			} else
			{
			//--------------------------------------------------------------------
			// Phase 2: Connect Socket.
			//-------------------------------------------------------------------
			struct sockaddr_in addr;
			memset (&addr,0, sizeof (addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = destAddr;
			addr.sin_port = htons (destPortNo);
			CDSU_TRACE3 (5, "CdSuNhUdp::addUdpPort(): connect socket %d to %s:%d\n", udpHandle, inet_ntoa (addr.sin_addr), destPortNo);
			if ((connect (udpHandle, (sockaddr *) & addr, sizeof (addr))) == -1)
			{
				CDSU_TRACE (1, "socket connect error\n");	
				failCode = CDSU_NHUDP_RSPFCODE_CONNECTERROR;	
				// close the socket
				close (udpHandle); 
			}  // if ((connect (udpHandle, (sockaddr *) & addr, sizeof (addr))) == -1)
		
			} // if (destPortNo == 0)

		} // if (destPortNo != CDSU_NHUDP_INV_UDPPORTID)
		
	} // if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		//--------------------------------------------------------------------
		// Phase 3: Socket ready. Update related data structures.
		//	*) udpPortId2UdpPortInfoTable 
		//	*) handle2UdpPortInfoTable
		//--------------------------------------------------------------------
	 	udpPortInfo = new CdSuNhUdpPortInfo;
		udpPortInfo->udpPortId = udpPortId;		// Udp Port ID
		udpPortInfo->handle = udpHandle;		// Socket FD
		udpPortInfo->selfIpv4Addr = localAddr;		// Local Address
		udpPortInfo->selfPortNo = localPortNo;		// Local Port NO
		udpPortInfo->destIpv4Addr = destAddr;		// destination Address
		udpPortInfo->destPortNo = destPortNo;		// Destination Port NO
		udpPortInfo->applEventHlr = applEventHlr; 	//  Application Event Handler.
		udpPortInfo->applActorId = applActorId;
		
		// Update Property of UDP Port (local transport address fixed and destination 
		// transport address fixed) by looking at the value of local and destination 
		// address.
		if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
		{
			udpPortInfo->isLTAddrFixed = true;
			udpPortInfo->isDTAddrFixed = true;
		} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
		{
			udpPortInfo->isLTAddrFixed = true;
		} else if (destPortNo != CDSU_NHUDP_INV_UDPPORT)
		{
			udpPortInfo->isDTAddrFixed = true;
		} else
		{
			// TODO:
		} // if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)

		// If ephermal port then set flag.
		if (lPortNo == 0)
		{
			udpPortInfo->isEphemeral = true;
		} 

		// Add Udp Port Info to table.
		if (udpPortId2UdpPortInfoTable->insert (udpPortId, udpPortInfo) != true)
		{
			delete udpPortInfo;
			failCode = CDSU_NHUDP_RSPFCODE_UNKNOWNERROR;
		} else 
		{
			if (handle2UdpPortInfoTable->insert (udpHandle, udpPortInfo) != true)
			{
				 udpPortId2UdpPortInfoTable->remove (udpPortId);
				 failCode = CDSU_NHUDP_RSPFCODE_UNKNOWNERROR;
				  delete udpPortInfo;
			}
		}	
	} // if (failCode != CDSU_NHUDP_RSPFCODE_INVALID)
	
	return failCode;
	
} // bool CdSuNhUdp::addUdpPort ()

//---------------------------------------------------------------------------
// Method	: sendDatagram () 
// Description  : This method Sends DataGram (For Always opened Udp Port)
// 		: [User will give udp port key = 0]
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpRspFailCode CdSuNhUdp::sendDatagram (const void* dataPtr,
			const Uint16& dataLen,	
			const CdSuIpv4UdpPortNo& destPortNo,
			const CdSuIpv4Addr& destAddr)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpHandle sendDataHandle = CDSU_NHUDP_RSPFCODE_INVALID;
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	
	// If Dest Port is invalid then fail
	if (destPortNo == CDSU_NHUDP_INV_UDPPORT || destPortNo == 0)
	{
		failCode = CDSU_NHUDP_RSPFCODE_INVPARAM;
	}
	
	// If Operation Not failed Then do remaining job.
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		// Open Udp Socket To send data.
		if ( (sendDataHandle  = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			failCode = CDSU_NHUDP_RSPFCODE_OPENERROR;
		} // if ( (sendDataHandle  = socket (AF_INET, SOCK_DGRAM, 0)) < 0)			
	} // if ()
	
	CdSuNhUdpPortInfo udpPortInfo;
	udpPortInfo.udpPortId = 0;
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		// Send Data
		udpPortInfo.handle = sendDataHandle;
		if ( (writeNoOfBytes (&udpPortInfo,
					(const char*)dataPtr,
					dataLen,
				       	destPortNo,
					destAddr)) < dataLen)
		{
			failCode = CDSU_NHUDP_RSPFCODE_SENDERROR;
		} // if ( (writeNoOfBytes ())
		// Once Send Data Finished then close socket Fd.
		close (sendDataHandle);
	} // if ()
	
	return failCode;
	
} // sendDatagram ()

//---------------------------------------------------------------------------
// Method	: sendDatagram () 
// Description  : This method Sends DataGram.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhUdpRspFailCode CdSuNhUdp::sendDatagram (const CdSuNhUdpPortId& udpPortId,
			const void* dataPtr,
			const Uint16& dataLen,	
			const CdSuIpv4UdpPortNo& destPortNo,
			const CdSuIpv4Addr& destAddr)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpPortInfo* udpPortInfo = NULL; 
	CdSuNhUdpRspFailCode failCode = CDSU_NHUDP_RSPFCODE_INVALID;
	
	// Lookup in udp port info tables.
	if (udpPortId2UdpPortInfoTable->lookup (udpPortId, udpPortInfo) != true)
	{
		// No UDP PORT.
		CDSU_TRACE (1, "Udp Port doesnt exist .\n");
		failCode = CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST;
	} else 
	{
		//-------------------------------------------------------------
		// Udp Port  exists, write on it.
		// *) If Udp Port Connected Then  Destination Address And Port should be
		// equal to connected one or destination port must be invalid
		// *) If Not Connected Then Destination Port Must not be 0
		// or Invalid. 
		//-------------------------------------------------------------
		if (udpPortInfo->isDTAddrFixed && destPortNo != CDSU_NHUDP_INV_UDPPORT)
		{
			if (udpPortInfo->destPortNo != destPortNo || 
					udpPortInfo->destIpv4Addr != destAddr)
			{
				failCode = CDSU_NHUDP_RSPFCODE_INVPARAM;	
				CDSU_TRACE (1, "Send Error Invalid Param .\n");
			}
		} else if (udpPortInfo->isDTAddrFixed == false && (destPortNo == 0 ||
			       	destPortNo == CDSU_NHUDP_INV_UDPPORT))
		{
			failCode = CDSU_NHUDP_RSPFCODE_INVPARAM;	
			CDSU_TRACE (1, "Send Error Invalid Param .\n");
		}
		// If Operation not failes so far then do other part of operation.
		if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
		{
			// Send Data.
			if (udpPortInfo->isDTAddrFixed && (writeNoOfBytes (udpPortInfo, (char*)dataPtr, dataLen)) < dataLen)
			{
				failCode = CDSU_NHUDP_RSPFCODE_SENDERROR;
			} else if (udpPortInfo->isDTAddrFixed == false &&
					(writeNoOfBytes (udpPortInfo, (char*) dataPtr, dataLen,
						       	destPortNo, destAddr)) < dataLen)
			{
				failCode = CDSU_NHUDP_RSPFCODE_SENDERROR;
			}

		} // if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)

	} // if (lookup...)
	
	return failCode;	
	
} // void CdSuNhUdp::sendDatagram ()

//---------------------------------------------------------------------------
// Method	: writeNoOfBytes write no of bytes () 
// Description  : write no of bytes (for destination connected udp port).
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------
int CdSuNhUdp::writeNoOfBytes (CdSuNhUdpPortInfo* udpPortInfo, const char* vptr, int noOfBytes)
{
	int nleft = noOfBytes;
	int nwritten;
	const char* ptr = vptr;

	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	while (nleft > 0)
	{
		CDSU_TRACE (5, "Sending Data............... .\n");
		CDSU_TRACE (5, "Udp Port Dest Fixed............... .\n");
		// Send Data
		nwritten = send (udpPortInfo->handle, ptr, nleft, 0);
		// Try to write all data.
		if (nwritten <= 0) 
		{
			if (errno == EINTR)
			{
				nwritten = 0;	// and call send () again 
			} else
			{
				// error 
				CDSU_TRACE (1, "Send Error If Connected Socket Then Destination Not Reachable .\n");
				return (noOfBytes - nleft);
			} // if (errno = EINTR)

		} // if ( (nwritten = send (fd, ptr, nleft, 0)) <= 0)
		nleft -= nwritten;
		ptr += nwritten;

	} // while (nleft > 0 && )
	return (noOfBytes - nleft);

} // int CdSuNhUdp::writeNoOfBytes () 

//---------------------------------------------------------------------------
// Method	: writeNoOfBytes write no of bytes () 
// Description  : write no of bytes (where udp port is not connected).
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------
int CdSuNhUdp::writeNoOfBytes (CdSuNhUdpPortInfo* udpPortInfo, const char* vptr,
	       	int noOfBytes, CdSuIpv4UdpPortNo destPortNo, CdSuIpv4Addr destAddr)
{
	int nleft = noOfBytes;
	int nwritten;
	const char* ptr = vptr;

	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	
	while (nleft > 0)
	{
		CDSU_TRACE (1, "Sending Data............... .\n");
		CDSU_TRACE (5, "Udp Port Dest Not Fixed............... .\n");
		struct sockaddr_in addr;
		memset (&addr,0, sizeof (addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = destAddr;
		addr.sin_port = htons (destPortNo);
		nwritten = sendto (udpPortInfo->handle, ptr, nleft, 0,
				(sockaddr *) & addr, sizeof (addr));
		if (nwritten <= 0) 
		{
			if (errno == EINTR)
			{
				nwritten = 0;	// and call write() again 
			} else
			{
				// error 
				CDSU_TRACE (1, "Send Error .\n");
				return (noOfBytes - nleft);
			} // if (errno = EINTR)
		} // if ( (nwritten = send (fd, ptr, nleft, 0)) <= 0)
		nleft -= nwritten;
		ptr += nwritten;
	} // while (nleft > 0 && )
	return (noOfBytes - nleft);
	
} // int CdSuNhUdp::writeNoOfBytes () 

//---------------------------------------------------------------------------
// Method	: readNoOfBytes write no of bytes () 
// Description  : read no of bytes(where udp port is connected).
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------
int CdSuNhUdp::readNoOfBytes (CdSuNhUdpPortInfo* udpPortInfo, char* vptr, int noOfBytes)
{

	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	int len;
	while (true)
	{
	 	CDSU_TRACE1 (5, "Reading Port Id(Dest Fixed) =%d...........\n", udpPortInfo->udpPortId);
		// Receive data
		if ( (len = recv (udpPortInfo->handle, vptr, noOfBytes, 0)) < 0)
		{
			if (errno == EINTR)
			{
				CDSU_TRACE1 (5, "Reading Port Id (Dest Fixed)=%d.........Interrupted\n", udpPortInfo->udpPortId);
				continue;
			}
			CDSU_TRACE1 (5, "Reading Port Id (Dest Fixed) =%d.........Error\n", udpPortInfo->udpPortId);
			
		} // if ( (len = recv (udpPortInfo->handle, vptr, noOfBytes)) < 0)
		break;	
	} // while ()
	
	return len;
	
} // int CdSuNhUdp::readNoOfBytes () 

//---------------------------------------------------------------------------
// Method	: readNoOfBytes write no of bytes () 
// Description  : read no of bytes(where udp port is not connected).
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------
int CdSuNhUdp::readNoOfBytes (CdSuNhUdpPortInfo* udpPortInfo, char* vptr,
	       	int noOfBytes, CdSuIpv4UdpPortNo& destPortNo, CdSuIpv4Addr& destAddr)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	int len;
	struct sockaddr_in addr;
	#ifdef __sun
		int addrLen = sizeof (addr);
	#else
		socklen_t addrLen = sizeof (addr);
	#endif
	while (true)
	{
	 	CDSU_TRACE1 (5, "Reading Port Id =%d...........\n", udpPortInfo->udpPortId);
		if ( (len = recvfrom (udpPortInfo->handle, vptr, noOfBytes, 0,
			(sockaddr*) &addr, &addrLen)) < 0)
		{
			if (errno == EINTR)
			{
				CDSU_TRACE1 (5, "Reading Port Id =%d.........Interrupted\n",
					       	udpPortInfo->udpPortId);
				continue;
			}
			CDSU_TRACE1 (5, "Reading Port Id =%d.........Error may be destination not reachable\n",
				       	udpPortInfo->udpPortId);
		} else
		{
			destPortNo = ntohs (addr.sin_port);
			destAddr = addr.sin_addr.s_addr;
			CDSU_TRACE3 (5, "CdSuNhUdp::readNoOfBytes(): connected socket %d to %s:%d\n", 
			udpPortInfo->handle, inet_ntoa (addr.sin_addr), destPortNo);
		}// if ( (len = recv (udpPortInfo->handle, vptr, noOfBytes)) < 0)
		break;
			
	} // while ()
	return len;
	
} // int CdSuNhUdp::readNoOfBytes () 

//---------------------------------------------------------------------------
// Method	: updateStasPkgCounter () 
// Description  : update statistics counter(this is for udp port of type
// 		: always opened)
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::updateStatsPkgCounter (CdSuNhUdpOpcode opcode,	CdSuNhUdpRspFailCode failCode)
{
	statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_ALWOP_CTR);
	
	if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
	{
		statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_ALWOP_CTR);
	}
	
} // void CdSuNhUdp::updateStatsPkgCounter  ()

//---------------------------------------------------------------------------
// Method	: updateStasPkgCounter () 
// Description  : Updates statistics counter.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhUdp::updateStatsPkgCounter (CdSuNhUdpOpcode opcode, CdSuIpv4UdpPortNo localPortNo,
				CdSuIpv4UdpPortNo destPortNo, CdSuNhUdpRspFailCode failCode)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	switch (opcode)
	{
	
	//--------------------------------------------------------------------
	// Update counter related add udp port request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_ADD_UDPPORT_REQ :
		{
			// Add Udp Port Counter
			statisticsPkg->incrementCtr (CDSU_NHUDP_ADD_UDPPORT_CTR);
			// Increment success counter.
			if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
			{
				statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR);
			}
					
			if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
			{
				// Local and Destination Both Fixed.
				statisticsPkg->incrementCtr (CDSU_NHUDP_ADD_UDPPORT_FIXED_LCLDST_CTR);
				// Increment success counter.
				if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
				{
					statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCLDST_CTR);
				}
			} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
			{
				// Local ONly fixed.
				statisticsPkg->incrementCtr (CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_CTR);
				// Increment Success Counter.
				if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
				{
					statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_CTR);
				}
			} else if (destPortNo != CDSU_NHUDP_INV_UDPPORT)
			{
				// Destination ONly Fixed.
				statisticsPkg->incrementCtr (CDSU_NHUDP_ADD_UDPPORT_FIXED_DST_CTR);
				// Increment Success Counter.
				if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
				{
					statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_DST_CTR);
				}
			} else
			{
				// TODO:
			} // if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)

			if (localPortNo == 0)
			{
				// Ephemeral Port
				statisticsPkg->incrementCtr (CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_EPH_CTR);
				// Success Counter.
				if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
				{
					statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_EPH_CTR);
				}
			} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
			{
				// Not Ephemeral Port
				statisticsPkg->incrementCtr (CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_ST_CTR);
				// Success Counter.
				if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
				{
					statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_ST_CTR);
				}
			} // if (localPortNo == 0)
			
			if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
			{
				// Active Udp Port
				statisticsPkg->incrementCtr (CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR);
				if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Active Udp Port where local is fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR);
				}
			} // if ()
		} // case :
		break;
	
	//--------------------------------------------------------------------
	// Update Counter Related to Remove Udp Port Counter
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_REM_UDPPORT_REQ:
		{
			// Remove Udp Port request
			statisticsPkg->incrementCtr (CDSU_NHUDP_REM_UDPPORT_CTR);
			if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
			{
				// Succes counter.
				statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR);
			}
			
			// If Error Type Udp Port Not Exist Then There is no need of
			// updating following Counter.
			if (failCode != CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST)
			{
				if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Local & Destination Both Fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_REM_UDPPORT_FIXED_LCLDST_CTR);
					// Success Counter.
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCLDST_CTR);
					}	
				} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Local ONly fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_CTR);
					// Success Counter.
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_CTR);
					}	
				}  else if (destPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Destination Only Fixed. 
					statisticsPkg->incrementCtr (CDSU_NHUDP_REM_UDPPORT_FIXED_DST_CTR);
					// Success Counter.
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_DST_CTR);
					}	
				} else
				{
				// TODO:
				} // if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
			
				if (localPortNo == 0)
				{
					// Ephemeral port
					statisticsPkg->incrementCtr (CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_EPH_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success Counter.
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_EPH_CTR);
					}	
				} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT) 
				{
					// Not Ephemeral
					statisticsPkg->incrementCtr (CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_ST_CTR);
					// Success Counter.
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_ST_CTR);
					}	
				} // if (localPortNo == 0)
		
				
				if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
				{	
					// Update Active Udp Port
					statisticsPkg->decrementCtr (CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR);
					if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
					{
						// Active Udp Port Where Local address is fixed.
						statisticsPkg->decrementCtr (CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR);
					}
				} // if (failCode ==)
			} // if ()	
		} // case :
		break;
		
	//--------------------------------------------------------------------
	// Update Counter Related to Data Request.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_DATA_REQ:
		{
			// Data Request.
			statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_CTR);
			// Success Data Request.
			if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
			{
				statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_CTR);
			}
			
			// If Error Type of Type Udp Port Not Exist Then There Is no
			// need of updating following counter.
			if (failCode != CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST)
			{
				if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Local And Destination Both Fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success Counter
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR);
					}	
				} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Local Only Fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success Counter.
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_CTR);
					}	
				}  else if (destPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// Destination Only Fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_FIXED_DST_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_DST_CTR);
					}	
				} else
				{
					// Both Not Fixed.(Can Be Removed)
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_NOTFIXED_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success 
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_NOTFIXED_CTR);
					}

				} // if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
			
				if (localPortNo == 0)
				{
					// Ephemeral Port
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_EPH_CTR);
					}	
				} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT) 
				{
					// Local Port Is Not Ephemeral.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR);
					if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
					{
						// Success.
						statisticsPkg->incrementCtr (CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_ST_CTR);
					}	
				} // if (localPortNo == 0)
			} // if ()
		} // case :	
		break;
	
	//--------------------------------------------------------------------
	// Update counter related to Data Indication.
	//--------------------------------------------------------------------
	case CDSU_NHUDP_UI_DATA_IND:
		{
			if (failCode == CDSU_NHUDP_RSPFCODE_INVALID)
			{
				// Success counter.
				statisticsPkg->incrementCtr (CDSU_NHUDP_DATAIND_CTR);
				if (localPortNo != CDSU_NHUDP_INV_UDPPORT && destPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
				
					// local and destination both fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAIND_FIXED_LCLDST_CTR);
				} else 
				{
					// local only fixed.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAIND_FIXED_LCL_CTR);
				} // if (info->isDTAddrFixed)
					
				if (localPortNo == 0)
				{
					// local port is ephemeral.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAIND_FIXED_LCL_EPH_CTR);
				} else if (localPortNo != CDSU_NHUDP_INV_UDPPORT)
				{
					// local port is not ephemeral.
					statisticsPkg->incrementCtr (CDSU_NHUDP_DATAIND_FIXED_LCL_ST_CTR);
				} // if (isEphemeral)

			} else
			{
				// error counter.
				statisticsPkg->incrementCtr (CDSU_NHUDP_DATAIND_ERROR_CTR);
			} // if ()
		} // case :
		break;

	default:
		break;
	} // switch 	

} // void CdSuNhUdp::updateStatsPkgCounter ()

//---------------------------------------------------------------------------
// Method	: getCorrectSubOpcodeRsp () 
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------


CdSuNhUdpSubOpcode CdSuNhUdp::getCorrectSubOpcodeRsp (CdSuNhUdpSubOpcode subOpcode)
{
	switch (subOpcode)
	{
	
	case 	CDSU_NHUDP_SO_INITIAL_CFG_REQ:
		return CDSU_NHUDP_SO_INITIAL_CFG_RSP;
		
	case   	CDSU_NHUDP_SO_GENERAL_CFG_REQ:
		return CDSU_NHUDP_SO_GENERAL_CFG_RSP;
	
	case 	CDSU_NHUDP_SO_CRE_REQ:
		return CDSU_NHUDP_SO_CRE_RSP;

	case 	CDSU_NHUDP_SO_DEL_REQ:
		return CDSU_NHUDP_SO_DEL_RSP;
	
	case	CDSU_NHUDP_SO_START_REQ:
		return CDSU_NHUDP_SO_START_RSP;
	
	case 	CDSU_NHUDP_SO_STOP_REQ:
		return CDSU_NHUDP_SO_STOP_RSP;
	
	default:
		return CDSU_NHUDP_SO_INVALID;
	} // switch	
} // 
//============================================================================
// < End of cdsunhudp.cpp>
//============================================================================

