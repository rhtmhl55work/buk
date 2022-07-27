//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : testreceiver.cpp
// Description  : This contains thread function which recievs messages from 
// 		  and displays.
// Ref Docs     :   
// Author       : Vijaya Kumar
// Created On   : Thu Feb 12 09:32:20 IST 2004
// Last Modified:
//=============================================================================

#include <pthread.h>
#include <stdio.h>
#include "cdsumdapi.h"
#include "parameter.h"
#include "sctpglobals.h"
#include "sctpmsgs.h"
#ifdef __ECOS
char cdRecvThreadStackSpace [1024 * 128];
#endif

//----------------------------------------------------------------------------
// Function	: cdManagerReceiverThread 
// Description	: Receives message from stack displays the message to 
// 		: user.
//----------------------------------------------------------------------------


void* cdManagerReceiverThread (void * arg)
{
	CdModuleId sysMgmtMid = *((CdModuleId*) (arg));
	printf ("Manager Receiver Thread Started\n");
	while (true)
	{
		CdMessage* cdMessage = NULL;
		if (cdSuMdRecvMsg (&cdMessage, sysMgmtMid) ==  false)
		{
			printf ("cdSuMdRecvMsg Failed\n");
		} else
		{
			printf ("Message received from stack\n");
		}
		// if
		switch (cdMessage->msgHdr.opcode)
		{
		case CDSCTP_SM_CREATE_STK_THREAD_ACK_MSG:
			{
			printf ("Stack Manager thread creation +ve Ack
				received\n");
			applDb.stackId = ( (CdSctpSmSkCreateStkThreadAckMsg*)cdMessage)->stackId;	
			applDb.state = ApplDb::NOTCONFGD;
			} // CDSCTP_SM_CREATE_STK_THREAD_ACK_MSG	
			break;
			
		case CDSCTP_SM_CONFIG_ACK_MSG:
			{
			printf ("Stack Init Config Ack received.\n");
			applDb.state = ApplDb::CONFGD;
			} 
			break;
			
		case CDSCTP_SM_CONFIG_FAIL_MSG:
			{
			printf ("Stack Init Config Fail Ack received\n");
			}
			break;
		case CDSCTP_SM_CREATE_STK_ACK_MSG:
			{
			printf ("Stack creation ack message received\n");
			applDb.state = ApplDb::CREATED;
			}
			break;
		case CDSCTP_SM_CREATE_STK_FAIL_MSG:
			{
			printf ("Stack creation fail ack received\n");
			}
			break;
		case CDSCTP_SM_START_STK_ACK_MSG:
			{
			printf ("Stack start ack received\n");
			applDb.state = ApplDb::RUNNING;
			}
			break;

		case CDSCTP_SM_START_STK_FAIL_MSG:
			{
			printf ("Stack start fail ack received\n");
			}
			break;	
		
		
		default:
			break;	
		} // switch
		cdSuFreeMsgBuf (cdMessage);
		printf ("   -------------\n");
		printf (" /		 \\\n");
		switch (applDb.state)
		{
		case  	ApplDb::NOTCONFGD:	
			printf ("X    NOTCONFGD    X\n");
			break;
		case  	ApplDb::CONFGD:	
			printf ("X    CONFGD    X\n");
			break;
		case  	ApplDb::CREATED:	
			printf ("X    CREATED    X\n");
			break;
		case  	ApplDb::RUNNING:	
			printf ("X    RUNNING    X\n");
			break;
		default:
			printf ("X  	    X\n");
			break;
		} // switch (applDb.state)
		printf ("   -------------\n");

	} // while (true)
	
} // cdReceiverThread

void cdCreateManagerReceiverThread (CdModuleId* mId)
{
	// create stack manager thread.
	pthread_t recvTid;
#ifdef __ECOS
	pthread_attr_t sctpStackAttr;
	pthread_attr_init (&sctpStackAttr);
	pthread_attr_setstackaddr (&sctpStackAttr,
		       		  (void*)&cdRecvThreadStackSpace [sizeof (cdRecvThreadStackSpace)]);
	pthread_attr_setstacksize (&sctpStackAttr, sizeof (cdRecvThreadStackSpace));
	pthread_create (&recvTid, &sctpStackAttr, cdManagerReceiverThread, NULL);
#elif sun
	
	pthread_attr_t sctpStackAttr;
	pthread_attr_init (&sctpStackAttr);
	pthread_attr_setscope (&sctpStackAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&recvTid, &sctpStackAttr, cdManagerReceiverThread, NULL);

#else
	// create the Stack Thread.
	pthread_create (&recvTid, 0, cdManagerReceiverThread, mId);
#endif
}


//----------------------------------------------------------------------------
// Function	: cdApplReceiverThread 
// Description	: Receives message from stack displays the message to 
// 		: user.
//----------------------------------------------------------------------------


void* cdApplReceiverThread (void * arg)
{
	CdModuleId applMid = *((CdModuleId*)arg);
	printf ("Application Receiver Thread Started\n");
	while (true)
	{
		CdMessage* cdMessage = NULL;
		if (cdSuMdRecvMsg (&cdMessage, applMid) ==  false)
		{
			printf ("cdSuMdRecvMsg Failed\n");
			continue;
		} else
		{
			printf ("Message received from stack\n");
		}
		// if
		switch (cdMessage->msgHdr.opcode)
		{
		case CDSCTP_AP_OPEN_SCTP_INST_ACK_MSG:
			{
				CdSctpApplOpenSctpInstanceAckMsg* openInstAckMsg =
				(CdSctpApplOpenSctpInstanceAckMsg*) cdMessage;
				if (openInstAckMsg->negAckReason == CDSCTP_NEGACK_NOT_SET)
				{	
					// Sctp Instance Initialize operation success.
					applDb.instanceId = openInstAckMsg->instanceId;
					printf ("Sctp Instance Creation Ack Message received.InstanceId ==> %d\n",
					       	applDb.instanceId);
				} else 
				{
					printf ("Sctp Instance Creation Fail Ack Message received\n");
				} 
			}
			break;	
		
		case CDSCTP_AP_ASSOC_ACK_MSG:
			{
			CdSctpApplAssocAckMsg* assocAckMsg=
				(CdSctpApplAssocAckMsg*) cdMessage;
			if (assocAckMsg->negAckReason == CDSCTP_NEGACK_NOT_SET)
			{
				// Positive ack. So success.
				printf ("Application received Association ack message\n");
				applDb.assocId = assocAckMsg->associationId;
				
			} else 
			{	
				// Negative ack. So fail.
				if (assocAckMsg->negAckReason == CDSCTP_NEGACK_INSTANCE_NOT_FOUND)
				{
					printf ("Association Fail ack received\n");
				}
			} // if	
			}
			break;
		case CDSCTP_AP_SEND_DATA_ACK_MSG:
			{
			CdSctpApplSendDataAckMsg* sendDataAckMsg = NULL;
			sendDataAckMsg = (CdSctpApplSendDataAckMsg*) cdMessage; 
			if (sendDataAckMsg->msgHdr.opcode == CDSCTP_AP_SEND_DATA_ACK_MSG)
			{
				if (sendDataAckMsg->negAckReason == CDSCTP_NEGACK_NOT_SET)
				{
					// +ve ack.
					if (sendDataAckMsg->assocId == applDb.assocId &&
				   	   sendDataAckMsg->streamId == applDb.lastDataStrId)
					{
					printf ("data send +ve ack received\n");
					printf ("StreamID==>%d\n", sendDataAckMsg->streamId);
					} else
					{
					printf ("data send invalid +ve ack received\n");
					}// if
				} else 
				{
					// -ve ack.	
					printf ("data send -ve ack received\n"); 
				} // if	
			} // if

			} // CDSCTP_AP_SEND_DATA_ACK_MSG
			break;
		case CDSCTP_AP_RECV_DATA_ACK_MSG:
			{
				CdSctpApplRecvDataAckMsg* recvDataAckMsg =
				       	(CdSctpApplRecvDataAckMsg* )cdMessage;
				if (recvDataAckMsg->negAckReason == CDSCTP_NEGACK_NOT_SET)
				{
				printf ("data received\n");
				printf ("data len==>%d", recvDataAckMsg->bufLen);
				printf ("data ==>%s\n", recvDataAckMsg->dataBuf);
				delete [] recvDataAckMsg->dataBuf;
				} else //
				{
					printf ("receive data -ve ack received\n"); 
				} // if
			}	
			break;
		case CDSCTP_AP_SHUTDOWN_ACK_MSG:
			{
			 	CdSctpApplShutdownAckMsg* shutdownAckMsg = (CdSctpApplShutdownAckMsg*)cdMessage;
				if (shutdownAckMsg->negAckReason == CDSCTP_NEGACK_NOT_SET &&
			    			shutdownAckMsg->stackId == applDb.stackId &&
	   		    			shutdownAckMsg->assocId == applDb.assocId  )
				{
					printf ("Shutdown +ve ack received\n");
				} else
				{
					printf ("Shutdown -ve ack received\n");
				} // if
			} // CDSCTP_AP_SHUTDOWN_ACK_MSG
			break;

		case CDSCTP_AP_COMMUP_NOTIF_MSG:
			{
				printf ("Application recvd CDSCTP_AP_COMMUP_NOTIF_MSG ");
				CdSctpApplCommUpNotifMsg* commUpNotifMsg =
				       (CdSctpApplCommUpNotifMsg*)cdMessage;	
				CdSctpStackId stackId = commUpNotifMsg->stackId;
				CdSctpAssocId assocId = commUpNotifMsg->assocId;
				CdSctpMsgCommUpEventType eMsgType = commUpNotifMsg->eventType;
				CdSctpAddrCount noOfAddr = commUpNotifMsg->noOfDestAddr;
				CdSctpStreamCount noOfInStreams = commUpNotifMsg->noOfInStreams;
				CdSctpStreamCount noOfOutStreams = commUpNotifMsg->noOfOutStreams;		
				applDb.assocId = assocId;
				//CdSctpUiCommUpEvent commUpEventType;
				if (eMsgType == CDSCTP_COMM_UP_VALID_COOKIE_RECEIVED)
				{
					printf ("eventType : CDSCTP_COMM_UP_VALID_COOKIE_RECEIVED \n");
				} else if (eMsgType == CDSCTP_COMM_UP_COOKIE_ACK_RECEIVED)
				{
					printf ("eventType : CDSCTP_COMM_UP_COOKIE_ACK_RECEIVED \n");
				} // if
				
						
			} // CDSCTP_AP_COMMUP_NOTIF_MSG
			break;
			
		case CDSCTP_AP_COMM_LOST_NOTIF_MSG:
			{	
				printf ("Application recvd CDSCTP_AP_COMM_LOST_NOTIF_MSG \n");
				CdSctpApplCommLostNotifMsg* commLostNotifMsg =
				       (CdSctpApplCommLostNotifMsg*)cdMessage;	
				CdSctpStackId stackId = commLostNotifMsg->stackId;
				CdSctpAssocId assocId = commLostNotifMsg->assocId;
				CdSctpTotalUnSentUnAckData totalUnSentUnAckData=
				       	commLostNotifMsg->totalUnSentUnAckData ;
				CdSctpTSN lastAckedTsn = commLostNotifMsg->lastAckedTsn;
				CdSctpTSN lastSentTsn = commLostNotifMsg->lastSentTsn;
				//CdSctpUiEventCallbacks* callbacks =
				  //     	(CdSctpUiEventCallbacks*)(commLostNotifMsg->callbacks);
				CdSctpMsgCommLostEventType eMsgType = commLostNotifMsg->eventType;
				//CdSctpUiCommLostEvent commLostEventType = CDSCTP_UI_COMM_LOST_INVALID;
				if (eMsgType == CDSCTP_COMM_LOST_NOT_REACHABLE)
				{
					//commLostEventType = CDSCTP_UI_COMM_LOST_UNREACHABLE;
					printf ("eventType: CDSCTP_UI_COMM_LOST_UNREACHABLE");
				} else if (eMsgType == CDSCTP_COMM_LOST_SHUTDOWN)
				{
					printf ("eventType: CDSCTP_COMM_LOST_SHUTDOWN ");
				} // if	
				printf ("\n");
			} // CDSCTP_AP_COMM_LOST_NOTIF_MSG 
			break;

		case CDSCTP_AP_DATA_ARRIVE_NOTIF_MSG:
			{	
				printf ("Data Arrive notification to application received\n");
				CdSctpApplDataArriveNotifMsg* dataArriveNotifMsg =
				       (CdSctpApplDataArriveNotifMsg*)cdMessage;	
				CdSctpStackId stackId =  dataArriveNotifMsg->stackId;
				CdSctpAssocId assocId = dataArriveNotifMsg->assocId;
				applDb.notifiedStreamId = dataArriveNotifMsg->streamId;
				applDb.notifiedDataLen = dataArriveNotifMsg->bufLen;
				printf ("StreamId %d\n", applDb.notifiedStreamId);
				printf ("DataLen %d\n", applDb.notifiedDataLen);
			} // CDSCTP_AP_DATA_ARRIVE_NOTIF_MSG		
			break;
		case CDSCTP_AP_SHUTDOWN_COMPL_NOTIF_MSG:
			{
				printf("Shutdown complete notification received\n");
			}
			break;
			
		default:
			printf ("Invalid message received\n");	
			break;	
		} // switch
		cdSuFreeMsgBuf (cdMessage);
		printf ("   -------------\n");
		printf (" /		 \\\n");
		switch (applDb.state)
		{
		case  	ApplDb::NOTCONFGD:	
			printf ("X    NOTCONFGD    X\n");
			break;
		case  	ApplDb::CONFGD:	
			printf ("X    CONFGD    X\n");
			break;
		case  	ApplDb::CREATED:	
			printf ("X    CREATED    X\n");
			break;
		case  	ApplDb::RUNNING:	
			printf ("X    RUNNING    X\n");
			break;
		default:
			printf ("X  	    X\n");
			break;
		} // switch (applDb.state)
		printf ("   -------------\n");

	} // while (true)
	
} // cdReceiverThread

void cdCreateApplReceiverThread (CdModuleId* mId)
{
	// create stack manager thread.
	pthread_t recvTid;
#ifdef __ECOS
	pthread_attr_t sctpStackAttr;
	pthread_attr_init (&sctpStackAttr);
	pthread_attr_setstackaddr (&sctpStackAttr,
		       		  (void*)&cdRecvThreadStackSpace [sizeof (cdRecvThreadStackSpace)]);
	pthread_attr_setstacksize (&sctpStackAttr, sizeof (cdRecvThreadStackSpace));
	pthread_create (&recvTid, &sctpStackAttr, cdApplReceiverThread, NULL);
#elif sun
	
	pthread_attr_t sctpStackAttr;
	pthread_attr_init (&sctpStackAttr);
	pthread_attr_setscope (&sctpStackAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&recvTid, &sctpStackAttr, cdApplReceiverThread, NULL);

#else
	// create the Stack Thread.
	pthread_create (&recvTid, 0, cdApplReceiverThread, mId);
#endif
}

