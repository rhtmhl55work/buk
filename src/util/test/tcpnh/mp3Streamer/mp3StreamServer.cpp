//=============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : mp3StreamServer.cpp
// Description  : Server Of Mp3 Streaming.
// Author       : Vijaya Kumar
// Created On   : Fri Apr  1 16:13:01 GMT 2004
//============================================================================

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "mp3StreamServer.h"
#include "cdsumdapi.h"

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#ifdef __sun
#include <strings.h>
#endif

//----------------------------------------------------------------------------
// Define : StreamServer Port and Listner Server Port, Messge Distributor Port
//----------------------------------------------------------------------------
#define STREAMSERVERPORT 8001
#define LISTENERSERVERPORT 8000
#define MSG_DIST_UDP_PORT 8010

//----------------------------------------------------------------------------
// Ip Address (Should be Changed according to Machine on which running)
//----------------------------------------------------------------------------
char localIpAddr [ 4 ] = { 192, 168, 16, 51 };


//----------------------------------------------------------------------------
// To Wait Till Tcp Nh Response Arrives use semaphore.
//----------------------------------------------------------------------------
sem_t semaphore;


//----------------------------------------------------------------------------
// Helper Global Functions
//----------------------------------------------------------------------------
Uint32 convIp2Int (char ipaddr [])
{
	Uint32 ipa;
	char* p = (char*)&ipa;
	p[0] = ipaddr [0];
	p[1] = ipaddr [1];
	p[2] = ipaddr [2];
	p[3] = ipaddr [3];
	return ipa;

} // convIp2Int ()

//----------------------------------------------------------------------------
// Define Static Variables of Mp3Server
//----------------------------------------------------------------------------
CdSuActorId Mp3StreamServer::applnModId;		// Application Mod Id
CdSuNhTcpLstnKey Mp3StreamServer::lstnKeyOfStrServer;	// Listen Key of Listner of Mp3 Strea feeder.
CdSuNhTcpLstnKey Mp3StreamServer::lstnKeyOfLstServer; 	// Listen Key of Listener of Mp3 Songs(ex: Xmms)

// Hash Table: streamer Connection ID to Channel Info. 
CdSuIntHash <ChannelInfo*>*  Mp3StreamServer::streamerConnId2ChannelInfoTable
		= new CdSuIntHash <ChannelInfo*> (CdSuIntHash<ChannelInfo*>::TZ_2039);

// Hash Table: Mp3 Songs Listener Connection Id To Connection Key Of Streamer. 
CdSuIntHash <CdSuNhTcpConnKey*>* Mp3StreamServer::listenerConnId2ConnKeyTable
		= new CdSuIntHash <CdSuNhTcpConnKey*> (CdSuIntHash<CdSuNhTcpConnKey*>::TZ_2039);

//----------------------------------------------------------------------------
// Class	: Mp3StreamServer
// Method	: constructor
//----------------------------------------------------------------------------
Mp3StreamServer::Mp3StreamServer ()
{

	streamerConnId2ChannelInfoTable = new CdSuIntHash <ChannelInfo*> (CdSuIntHash<ChannelInfo*>::TZ_2039);
	listenerConnId2ConnKeyTable = new CdSuIntHash <CdSuNhTcpConnKey*> (CdSuIntHash<CdSuNhTcpConnKey*>::TZ_2039);
} // Mp3StreamServer::Mp3StreamServer ()

//----------------------------------------------------------------------------
// Method 	: setUpEventHlr
// Class 	: Mp3StreamServer
// Description	: Nh Tcp Management Event Handler
//----------------------------------------------------------------------------
		
void  Mp3StreamServer::setUpEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpMiEvent* evt = (CdSuNhTcpMiEvent*) arg;
	switch (evt->opcode)
	{
	case CDSU_NHTCP_MI_INIT_RSP:
		{
			printf ("Init Rsp Received\n");
			if (((CdSuNhTcpMiInitRspEvent*) evt)->rspCode == CDSU_NHTCP_RSPCODE_SUCCESS)
			{
				printf ("Init Success\n");
			}
			//Delete event.
			delete (CdSuNhTcpMiInitRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_DELNH_RSP:
		{
			// Delete event.
			delete (CdSuNhTcpMiDelNhRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_CTRL_RSP:
		{
			printf ("Control Response received.\n");
			if (((CdSuNhTcpMiCtrlRspEvent*) evt)->rspCode == CDSU_NHTCP_RSPCODE_SUCCESS)
			{
				printf ("Control NH success\n");
			}
			// Delete Event.
			delete (CdSuNhTcpMiCtrlRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_CFG_RSP:
		{
			printf ("Config Response received\n");
			delete (CdSuNhTcpMiCfgRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_STATUS_RSP:
		{
			delete (CdSuNhTcpMiStatusRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_STATISTICS_RSP:
		{
			delete (CdSuNhTcpMiStsRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_USTATUS_IND:
		{
			delete (CdSuNhTcpMiUStaIndEvent*) evt;
			evt = NULL;
		}
		break;

	default:
		{
			// Do Nothing.
			delete evt;
			evt = NULL;
		}
		break;
	} // switch
	sem_post (&semaphore);
	
} // setUpEventHlr

//----------------------------------------------------------------------------
// Method	: lstEventHlr (Static Method)
// Class	: Mp3StreamServer
// Description	: Nh Tcp Listen Event Handler
//----------------------------------------------------------------------------

void Mp3StreamServer::lstEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	CdSuNhTcpUiLstnEvent* evt = (CdSuNhTcpUiLstnEvent*) arg;
	switch (evt->opcode)
	{
	case CDSU_NHTCP_UI_ADD_LISTENER_RSP:
			{
			CdSuNhTcpRspCode rsp = ((CdSuNhTcpUiAddLstnRspEvent*)evt)->rspCode;
			if (rsp != CDSU_NHTCP_RSPCODE_SUCCESS)
			{
				printf ("Add Listener failed.\n");
			} else 
			{
				printf ("Add Listener succesful.\n");
			}
			//Delete event.
			delete (CdSuNhTcpUiAddLstnRspEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_CON_IND:
		{
			printf ("New Connection on Listener.\n");

			CdSuNhTcpUiConIndEvent* conIndEvent =
						(CdSuNhTcpUiConIndEvent*) evt;
			// Print Remote Ipaddr and Port No.
			printf ("Port No from where connection has arrived: %d\n",
						conIndEvent->dstPortNo);	
			unsigned char ipaddr [4];
			memcpy ((void*) ipaddr, (void*) &conIndEvent->dstIpv4Addr, 4);
			for (int i = 0; i < 4; i++)
			{
				printf ("%d\n", ipaddr [i]);
			}
			
			// Check For Connection Indication For Which Listener.
			// There are Two Listener.
			// *) Stream Listner
			// *) Mp3 Client Listner
			if ((evt->lstnKey).lstnId == lstnKeyOfStrServer.lstnId)
			{
				// Coneection request from stream 
				printf ("Connection request from streamer...................\n");
				// Now Add Empty Channel Info 
				// Later fill up Channel Info when streamer client tells mount point
				ChannelInfo* channelInfo = new ChannelInfo; 
				channelInfo->streamerConnKey = conIndEvent->connKey;
				streamerConnId2ChannelInfoTable->insert (conIndEvent->connKey.connId, channelInfo);	

			} else if ((evt->lstnKey).lstnId == lstnKeyOfLstServer.lstnId)
			{
				// Connection Request from Mp3 Listner
				printf ("Connection request from Mp3 listener.................\n");
				printf ("Destination Port %d\n", conIndEvent->dstPortNo);
				// Get Connection Key
				CdSuNhTcpConnKey* connKey = new CdSuNhTcpConnKey;
				*connKey = conIndEvent->connKey;
				// Send Ack To Mp3 Listener
				char data [] = "HTTP/1.0 200 OK\nServer: Prakash_Streaming_Server 1.0\n";
				int len = strlen (data);
				CdSuNhTcpUiDataReqParam	dataReqParam;
				dataReqParam.init (*connKey, applnModId, conEventHlr, data, len);
				CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
				if (retType != CDSU_NHTCP_API_RT_SUCCESS)
				{
					printf ("Send: Mp3 Listner Ack Failed\n");
				} else
				{
					// Succesfully Ack Sent to Mp3 Listner Then Add Info To Table.
					// Now we will add Connectio Key of Listner of Mp3
					// Once Mp3 Listner Tells Channel Wanted then Change This Connection
					// Key to Streamer Connection Key.
					// So Two table Convey Information like follow
					// *) Stremmer Connection Key -> Mp3 Listener List(Contains Listner Connection Keys).
					// *) Listner Connection Key -> Streamer Connection Key 
					// First One Is required to write mp3 data to mp3 listener when mp3 streamer sends data
					// to Server.
					// Second one is required to remove entry in Mp3 Listner List when Mp3 listner closes. 
					listenerConnId2ConnKeyTable->insert (conIndEvent->connKey.connId, connKey);
				}
						
			} else
			{
				// This never Happens
				printf ("Unknown client LstnID = %d, %d\n", lstnKeyOfStrServer.lstnId, conIndEvent->lstnKey.lstnId);
			}
			// Delete event.
			delete (CdSuNhTcpUiConIndEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_REM_LISTENER_RSP:
		{
			if (((CdSuNhTcpUiRemLstnRspEvent*)evt)->rspCode != CDSU_NHTCP_RSPCODE_SUCCESS)
			{
				printf ("Remove Listener failed.\n");
			} else 
			{
				printf ("Remove Listener succesful.\n");
			}
			// Delete Event.
			delete (CdSuNhTcpUiRemLstnRspEvent*) evt;
			evt = NULL;
		}
		break;

	default:
		{
			// Do Noting.
			delete evt;
			evt = NULL;
		}
		break;
	} // switch
	sem_post (&semaphore);
} // lstEventHlr ()

//----------------------------------------------------------------------------
// Method 	: conEventHlr
// Class	: Mp3StreamServer
// Description: Connection Event Handler of NH TCP (Static Method)
//----------------------------------------------------------------------------

void Mp3StreamServer::conEventHlr (void* arg)
{	
	CdSuNhTcpUiConnEvent* evt = (CdSuNhTcpUiConnEvent*) arg;

	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	switch (evt->opcode)
	{
	
	case CDSU_NHTCP_UI_DATA_IND:
		{
			printf ("Data Indication...........................\n");
			CdSuNhTcpUiDataIndEvent* dataIndEvent = 
						(CdSuNhTcpUiDataIndEvent*) evt;
			int len = dataIndEvent->dataLen;
			printf ("Length of data is %d\n", len);
			char data [ len ];
			memcpy (data, dataIndEvent->dataPtr, len);
			ChannelInfo* channelInfo;
			CdSuNhTcpConnKey* connKey;
			// Check Data From Stream Client or Mp3 Listner
			if (streamerConnId2ChannelInfoTable->lookup (dataIndEvent->connKey.connId, channelInfo) == true)
			{	
				// Data From Stream Client
				printf ("Data received from stream Client\n");
				writeStreamData (channelInfo, data, len);	
			} else if (listenerConnId2ConnKeyTable->lookup (dataIndEvent->connKey.connId, connKey) == true)
			{
				// Data From Mp3 Listner: It Must be Information of Channel Wanted.
				printf ("Data Received From Mp3 Listner\n");
				tellChannelWanted (dataIndEvent->connKey.connId, data);
			}
			// Delete event.
			// delete dataIndEvent->dataPtr;
			delete (CdSuNhTcpUiDataIndEvent*) evt;
			evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_IND:
		{
			ChannelInfo* channelInfo;
			CdSuNhTcpConnKey* connKey;
			printf ("Close Connection Indication received\n");
			if (streamerConnId2ChannelInfoTable->lookup (evt->connKey.connId, channelInfo) == true)
			{	
				// Stream Client Closed
				printf ("stream Client Closed\n");
				closeChannel (channelInfo);	
			} else if (listenerConnId2ConnKeyTable->lookup (evt->connKey.connId, connKey) == true)
			{
				// Close From Mp3 Listner:
				printf ("Connection Closed From Mp3 Listner\n");
				closeMp3ListnerConnection (connKey->connId);
			}
	// Delete event.
			delete (CdSuNhTcpUiCloseConRspEvent*) evt;
			evt = NULL;
		}
		break;

	default:
		{
			delete evt;
			evt = NULL;
		}
		break;
	} // switch
}


//----------------------------------------------------------------------------
// Method	: start
// Class	: Mp3StreamServer
// Description	: Start The Operation
//----------------------------------------------------------------------------
void Mp3StreamServer::start ()
{	
	// Initialize The Varible 
	initAll ();
	
	// Create Nh Tcp
	CdSuNhTcpMiInitParam initParam;
	initParam.init (nhModId, applnModId, setUpEventHlr);
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiInit (initParam, nhInstId);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		printf ("NH Creation Failed\n");
	} else
	{
		printf ("NH Created Success Fully\n");
	}
	int rv;
	do 
	{
		rv =  sem_wait (&semaphore);	// Wati Till Ack Arrives
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return;
	} // if ()

	// Start Nh Tcp
	CdSuNhTcpMiCtrlReqParam ctrlReqParam;
	ctrlReqParam.nhTcpInstId = nhInstId;
	ctrlReqParam.subOpcode = CDSU_NHTCP_SO_START_REQ;
	retType = cdSuNhTcpMiCtrlReq (ctrlReqParam);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		printf ("Start NH API call::ERROR. %d\n", retType);
	} else
	{
		printf ("Nh Tcp Started Successfully\n");
	}
	do 
	{
		rv =  sem_wait (&semaphore);	// Wati Till Ack Arrives
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return;
	} // if ()
	
	// Open Listener For Stream Client
	CdSuIpv4Addr lsipv4 = convIp2Int (localIpAddr); 
	CdSuNhTcpUiAddLstnReqParam listenReqParam;
	listenReqParam.init (nhInstId, applnModId, lstEventHlr, conEventHlr,
				lsipv4, STREAMSERVERPORT, false);
	retType = cdSuNhTcpUiAddLstnReq (listenReqParam, lstnKeyOfStrServer);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		printf ("Add Listener For Stream Client Fail\n");
	} else
	{
		printf ("Listner Id Of Stream Client Listner %d\n", lstnKeyOfStrServer.lstnId);
	}
	do 
	{
		rv =  sem_wait (&semaphore);	// Wati Till Ack Arrives
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return;
	} // if ()
	
	// Open Listner For Mp3 Listener
	listenReqParam.init (nhInstId, applnModId, lstEventHlr, conEventHlr,
				lsipv4, LISTENERSERVERPORT, false);
	retType = cdSuNhTcpUiAddLstnReq (listenReqParam, lstnKeyOfLstServer);
	if (retType != CDSU_NHTCP_API_RT_SUCCESS)
	{
		printf ("Add Listener For Mp3 Listner Fail\n");
	} else
	{
		printf ("Listner Id Of Mp3 Listner's Listner %d\n", lstnKeyOfLstServer.lstnId);
	}
	do 
	{
		rv =  sem_wait (&semaphore);	// Wati Till Ack Arrives
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return;
	} // if ()
} // start ()

//----------------------------------------------------------------------------
// Method	: initAll 
// class	: Mp3StreamServer
//----------------------------------------------------------------------------

void Mp3StreamServer::initAll ()
{
	sem_init(&semaphore, 0, 0);
	nhModId.hwUnitId.rackId = 0;
	nhModId.hwUnitId.unitId = 0;
	nhModId.hwUnitId.slotId = 0;
	nhModId.hwUnitId.procId = 0;

	nhModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId.srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appId = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.
	

	// Note: Appl Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Appl Module ID).
	applnModId.hwUnitId.rackId = 0;
	applnModId.hwUnitId.unitId = 0;
	applnModId.hwUnitId.slotId = 0;
	applnModId.hwUnitId.procId = 0;

	applnModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	applnModId.srvcHlrId.appSrvcHlrId.appType = 2;
	applnModId.srvcHlrId.appSrvcHlrId.appId = 2;
	applnModId.srvcHlrId.appSrvcHlrId.appModuleId = 3;	// To be changed later.

	CdSuMdInitParam msgDstbInitParam;
	memset ((void *)&msgDstbInitParam.selfMid, 0, sizeof (CdModuleId));
	msgDstbInitParam.ipaddr = convIp2Int (localIpAddr);
	msgDstbInitParam.udpLpn = MSG_DIST_UDP_PORT;
	if (cdSuMdInit (msgDstbInitParam) == true)
	{
		printf ("Msg Dst Init Success\n");	
	} else 
	{
		printf ("Msg Dst Init Failed\n");
	}
	if (cdSuMdRegisterMid (nhModId) == false)
	{
		printf ("NH Mod Id Registration Fail\n");
	} else
	{
		printf ("NH Mod Id Registration Success\n");
	}	
	if (cdSuMdRegisterMid (applnModId) == false)
	{
		printf ("Appl Mod Id Registration Failed\n");
	} else
	{
		printf ("Appl Mod Id Registration Success\n");
	}	

} // initAllGlobals ()

//----------------------------------------------------------------------------
// Method	: writeStreamData
// Class	: Mp3Stream Server
// Description	: If First Time Data Arrived From Stream Cleint Then It Must
// 		: Be Mount Point. Store This Information. Otherwise
// 		: it Must Be Mp3 Data. Then Send This Data To Each Mp3 Listner
// 		: who are listening On This Channel
//----------------------------------------------------------------------------

void Mp3StreamServer::writeStreamData (ChannelInfo* channelInfo, char* data, int dataLen)
{
	CdSuNhTcpConnId connId = channelInfo->streamerConnKey.connId;
	if (channelInfo->mountPoint == NULL)
	{
		//TODO: if this Mount Point Is Already In Use Then Send Neg Ack
		// Otherwise Send +ve Ack.
		bool found = false;
		CdSuIntHash<ChannelInfo*>::iterator iter;
		iter = streamerConnId2ChannelInfoTable->begin ();
		for (; iter != streamerConnId2ChannelInfoTable->end (); ++iter)
		{
		 	ChannelInfo* cInfo = *iter;
			if (cInfo->mountPoint != NULL && strcmp (cInfo->mountPoint, data) == 0)
			{
				// Mount Point Already Exist
				found = true;
				break;
			}
		}
		if (found == false)
		{
			// Not Found update table And Send 
			printf ("Getting Mounting Point from streamer\n");
			channelInfo->mountPoint = new char [dataLen];
			strcpy (channelInfo->mountPoint, data);
			printf ("Mount Point:%s\n", channelInfo->mountPoint);	
						
			CdSuNhTcpUiDataReqParam	dataReqParam;
			dataReqParam.init (channelInfo->streamerConnKey, applnModId, conEventHlr, "OK", 3);
			CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
			if (retType == CDSU_NHTCP_API_RT_SUCCESS)
			{
				printf ("Ack Sent To Stream Client \n");
			} else
			{
				printf ("Ack Send To Stream Client Failed\n");
			}
		} else
		{
			CdSuNhTcpUiDataReqParam	dataReqParam;
			dataReqParam.init (channelInfo->streamerConnKey, applnModId, conEventHlr, "NK", 3);
			CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
			if (retType == CDSU_NHTCP_API_RT_SUCCESS)
			{
				printf ("Ack Sent To Stream Client \n");
			} else
			{
				printf ("Ack Send To Stream Client Failed\n");
			}
			// Free Entry
			streamerConnId2ChannelInfoTable->remove (channelInfo->streamerConnKey.connId);
			delete channelInfo;
		}
				
	} else
	{
		// get the list and send mp3 data to all mp3 lisntener in list.
		CdSuList<CdSuNhTcpConnKey*>::iterator iter;
		iter = channelInfo->listenerList->begin ();
		printf ("List Size : %d\n",  channelInfo->listenerList->size ());
		for (int i = 1; iter !=  channelInfo->listenerList->end (); ++iter, ++i)
		{
			
			CdSuNhTcpConnKey* key = *iter;
			CdSuNhTcpUiDataReqParam	dataReqParam;
			dataReqParam.init (*key, applnModId, conEventHlr, data, dataLen);
			CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
			
			printf ("Writing MP3 To Listener %s %d\n", channelInfo->mountPoint, i);
			if (retType == CDSU_NHTCP_API_RT_SUCCESS)
			{
				printf ("Writing MP3 To Listener Success\n");
			} else
			{
				printf ("Writing MP3 to Listener:: ERROR\n" );
			}
		
		} 
		if (channelInfo->listenerList->size () == 0)
		{
			printf ("NO Listener for %s\n", channelInfo->mountPoint);
		}
	}
} // void writeStreamData ()

//----------------------------------------------------------------------------
// Method	: tellChannelWanted
// class 	: Mp3StreamServer
// Description  : Get The Information about channel Wanted by MP3 Listner
//----------------------------------------------------------------------------
void Mp3StreamServer::tellChannelWanted (const CdSuNhTcpConnId& connId, const char* data)
{
	CdSuNhTcpConnKey* connKey;
	if (listenerConnId2ConnKeyTable->lookup (connId, connKey) == true)
	{
		printf ("Getting Mounting Point from Listener........................\n");
		// Get Mount Point
		char* mountPoint;
		mountPoint = new char [20];
		bzero (mountPoint, 20);
		int i = 0;
		for (; data [i + 4] != ' ' && data [i + 4] != 0 ; i++)
		{
			mountPoint [ i ] = data [i + 4];
		}
		mountPoint [ i ] = 0;	
		//strncpy (mountPoint,  &data [4], strlen (&data[4]) - 86);
		printf ("Mount Point Wanted From Mp3 Lisnter:%s\n", mountPoint);
		
		// Find Streamer Corresponding to this mount point
		// Then Update Table
		bool found = false;;
		for (CdSuIntHash<ChannelInfo*>::iterator i = streamerConnId2ChannelInfoTable->begin ();
		i != streamerConnId2ChannelInfoTable->end (); ++i)
		{
			ChannelInfo* info = *i;
			if (info != NULL)
			{
				if (info->mountPoint != NULL && strcmp (info->mountPoint, mountPoint) == 0)
				{
					printf ("Listner requested Mount Point found\n");
					found = true;
					// Add Mp3 Listner Connection Key to List
					// Change Connection Key of Mp3 Listner in Table to Streamer Connection Key
					CdSuNhTcpConnKey* cKey = new CdSuNhTcpConnKey;
					*cKey = *connKey;	// get Value of Connection Key of MP3 Lisntener
					connKey->connId = info->streamerConnKey.connId; // Change To Streamer Conn ID.
					// Add Connection Key of Mp3 Listener To List
					info->listenerList->push_back (cKey);
					printf ("Listener Added To Channel\n");
				} else
				{
					printf ("Comp: *%s* *%s*\n", info->mountPoint, mountPoint);
				}
			} else
			{
			}
		}
		if (found == false)
		{
			printf ("Channel %s not found\n", mountPoint);
			//  Close The Connection.
			CdSuNhTcpUiCloseConReqParam closeConReqParam;
			closeConReqParam.init (*connKey, applnModId, CDSU_NHTCP_CCR_NORMAL);
			CdSuNhTcpApiRetType retType = cdSuNhTcpUiCloseConReq (closeConReqParam);
		
			printf ("Closing MP3 Listener Connection since Mount Point Not Found.....................%d\n", i);
			if (retType == CDSU_NHTCP_API_RT_SUCCESS)
			{
				printf ("Closing MP3 Listener Connection Success\n");
			} else
			{
				printf ("Closing MP3 Listener Connection :: ERROR\n" );
			}

			}

	} else
	{
		printf ("listenerConnId2ConnKeyTable lookup failed\n");
	}
} // void tellChannelWanted ()


//----------------------------------------------------------------------------
// Method 	: closeChannel
// Class	: Mp3StreamServer
// Description	: 
//----------------------------------------------------------------------------

void Mp3StreamServer::closeChannel (const ChannelInfo* channelInfo)
{
	printf ("Clossing Channel..............................\n");
	
	// get the list and close all mp3 lisntener in list.
	CdSuList<CdSuNhTcpConnKey*>::iterator iter;
	iter = channelInfo->listenerList->begin ();
	printf ("List Size : %d\n",  channelInfo->listenerList->size ());
	for (int i = 1; iter !=  channelInfo->listenerList->end (); ++iter, ++i)
	{
		CdSuNhTcpConnKey* key = *iter;
		CdSuNhTcpUiCloseConReqParam closeConReqParam;
		closeConReqParam.init (*key, applnModId, CDSU_NHTCP_CCR_NORMAL);
		CdSuNhTcpApiRetType retType = cdSuNhTcpUiCloseConReq (closeConReqParam);
		
		printf ("Closing MP3 Listener Connection.....................%d\n", i);
		if (retType == CDSU_NHTCP_API_RT_SUCCESS)
		{
			printf ("Closing MP3 Listener Connection Success\n");
		} else
		{
			printf ("Closing MP3 Listener Connection :: ERROR\n" );
		}
		// Delet Entrys
		CdSuNhTcpConnKey* connKeyOfStreamer;
		if (listenerConnId2ConnKeyTable->lookup (key->connId, connKeyOfStreamer) == true)
		{
			delete connKeyOfStreamer;
		}
		listenerConnId2ConnKeyTable->remove(key->connId);
		delete key;

	} 
	
	// remove entry in table
	streamerConnId2ChannelInfoTable->remove (channelInfo->streamerConnKey.connId);
	
	delete channelInfo->mountPoint;
	delete channelInfo;

} // void closeChannel ()

//----------------------------------------------------------------------------
// Method 	: closeMp3ListnerConnection
// Class	: Mp3StreamServer
// Description	:
//----------------------------------------------------------------------------

void Mp3StreamServer::closeMp3ListnerConnection (const CdSuNhTcpConnId& connId)
{
	
	printf ("Clossing Mp3 Listner Connection ..........................\n");
	/*// Delet Entrys
	CdSuNhTcpConnKey* connKeyOfStreamer;
	if (listenerConnId2ConnKeyTable->lookup (connId, connKeyOfStreamer) == true)
	{
		delete connKeyOfStreamer;
	}
	listenerConnId2ConnKeyTable->remove(key->connId);
	delete key;
	try to delete
	*/
} // void closeMp3ListnerConnection ()
