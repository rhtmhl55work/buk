//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testfunctions.cpp
// Description  : Test Functions
// Author       : 
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//#include "cdsunhudpapi.h"
#include "cdsutrace.h"
#include "parameter.h"
#include "cdsumdapi.h"
#include "testfunctions.h"

void smMgmtEventHlr (void*);
void applEventHlr (void*);

extern void cdCreateManagerReceiverThread (CdModuleId* mId);
extern void cdCreateApplReceiverThread (CdModuleId* mId);

#ifdef __ECOS
char applThreadSpace[PTHREAD_STACK_MIN * 32];
char sysMgmtThreadSpace [PTHREAD_STACK_MIN * 32];
#endif


// Helper Functions.
//---------------------------------------------------------------------------
// Function	: cdSuNhUdpConvIp2Int ()
// Description  : 
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

Uint32 cdSuNhUdpConvIp2Int (const char* ipaddr)
{
	Uint32 ipa;
	char* p = (char*)&ipa;
	p[0] = ipaddr [0];
	p[1] = ipaddr [1];
	p[2] = ipaddr [2];
	p[3] = ipaddr [3];
	
	return ipa;
} 

//---------------------------------------------------------------------------
// Function	: cdSuNhUdpConvInt2Ip ()
// Description  : 
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void cdSuNhUdpConvInt2Ip (char* ipaddr,  Uint32 a)
{
	memcpy (ipaddr, (char*) a, 4);
} 

//---------------------------------------------------------------------------
// Function	: cdSuNhUdpConvInt2Addr ()
// Description  : 
// Inputs       :   
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

char* cdSuNhUdpConvInt2Addr (Uint32 a)
{
	struct sockaddr_in addr;
	memset (&addr,0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = a;
	addr.sin_port = 0;
	return inet_ntoa (addr.sin_addr);
} 



void displayErrorType (CdSuNhUdpRspFailCode failCode)
{
	if (CDSU_NHUDP_RSPFCODE_INVALID > failCode && failCode > -1)
	{
		printf ("Error Type %s\n", failCodeString [failCode]);
	}	
}

//----------------------------------------------------------------------------
// Function	: sysMgmtStart ()
// Description	: This is System Management Thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void* sysMgmtStart (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	printf ("System Manager Thread Started\n");
	
	
	while (true)
	{	
		CdMessage* cdMessage = NULL;
		if (cdSuMdRecvMsg (&cdMessage, smModId) ==  false)
		{
			cdMessage = NULL;
			printf ("sysMgmtStart::cdSuMdRecvMsg Failed\n");
			continue;
		} else
		{
			printf ("sysMgmtStart:Message received from stack\n");
		}
		
		switch (cdMessage->msgHdr.opcode)
		{
		case CDSU_NHUDP_MI_INIT_RSP:
		{
		smMgmtEventHlr ( (void*) &( (CdSuNhUdpMiInitRspMsg*)cdMessage)->initRspEvent);		
		}
		break;
	
		case CDSU_NHUDP_MI_DELNH_RSP:
		{
		smMgmtEventHlr ( (void*) &( (CdSuNhUdpMiDelNhRspMsg*)cdMessage)->delNhRspEvent);	
		}
		
		break;
		case CDSU_NHUDP_MI_CTRL_RSP:
		{
		smMgmtEventHlr ( (void*) &( (CdSuNhUdpMiCtrlRspMsg*)cdMessage)->ctrlRspEvent);	
		}
		break;
		
		case CDSU_NHUDP_MI_STATUS_RSP:
		{
		smMgmtEventHlr ( (void*) &( (CdSuNhUdpMiStatusRspMsg*)cdMessage)->statusRspEvent);	
		}
		break;

		case CDSU_NHUDP_MI_STATISTICS_RSP:
		{
		smMgmtEventHlr ( (void*) &( (CdSuNhUdpMiStsRspMsg*)cdMessage)->stsRspEvent);	
		}
		break;
	
		//TODO: case for unsolicated rsp to be added.
		default:
		{
		printf ("Invalid Message\n");	
		}	
		break;
		} 
		if (cdMessage != NULL) CDSU_FREEMSGBUF (cdMessage);
	} 	
} // sysMgmtStart ()

//----------------------------------------------------------------------------
// Function	: applStart ()
// Description	: This is Application Receiver Thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void* applStart (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	printf ("Application Thread Started\n");
	
	
	while (true)
	{	
		CdMessage* cdMessage = NULL;
		if (cdSuMdRecvMsg (&cdMessage, applnModId) ==  false)
		{
			printf ("applStart::cdSuMdRecvMsg Failed\n");
			cdMessage = NULL;
			continue;
		} else
		{
			printf ("applStart:Message received from stack\n");
		}

		switch (cdMessage->msgHdr.opcode)
		{
		case CDSU_NHUDP_UI_ADD_UDPPORT_RSP:
		{
		applEventHlr ( (void*) &( (CdSuNhUdpUiAddUdpPortRspMsg*)cdMessage)->addUdpPortRspEvent);	
		}
		break;
	
		case CDSU_NHUDP_UI_REM_UDPPORT_RSP:
		{
		applEventHlr ( (void*) &( (CdSuNhUdpUiRemUdpPortRspMsg*)cdMessage)->remUdpPortRspEvent);	
		}
		break;
		
		case CDSU_NHUDP_UI_DATA_RSP:
		{
		applEventHlr ( (void*) &( (CdSuNhUdpUiDataRspMsg*)cdMessage)->dataRspEvent);	
		}
		break;

		case CDSU_NHUDP_UI_DATA_IND:
		{
		applEventHlr ( (void*) &( (CdSuNhUdpUiDataIndMsg*)cdMessage)->dataIndEvent);	
		}
		break;
	
		//TODO: case for unsolicated rsp to be added.
		default:
		{
		printf ("Invalid Message\n");	
		}	
		break;
		} 
		if (cdMessage != NULL)	CDSU_FREEMSGBUF (cdMessage);
	} 	
} // applStart ()

//----------------------------------------------------------------------------
// Function	: createSMThr ()
// Description	: It creates System Management (application) thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void createSMThr ()
{

	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	//----------------------------------------------------------------------------
	// Creating System Mgmt Thread.
	//----------------------------------------------------------------------------
	pthread_t sysMgmtThread;
#ifdef __ECOS
	pthread_attr_t sysMgmtAttr;
	pthread_attr_init (&sysMgmtAttr);
	pthread_attr_setstackaddr (&sysMgmtAttr, 
		(void*)&sysMgmtThreadSpace [sizeof (sysMgmtThreadSpace)]);
	pthread_attr_setstacksize (&sysMgmtAttr, 
					sizeof (sysMgmtThreadSpace));
	pthread_create (&sysMgmtThread, &sysMgmtAttr, 
						sysMgmtStart, 0);
#elif sun
	pthread_attr_t sysMgmtAttr;
	pthread_attr_init (&sysMgmtAttr);
	pthread_attr_setscope (&sysMgmtAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&sysMgmtThread, &sysMgmtAttr,
						sysMgmtStart, 0);
#else
	pthread_create (&sysMgmtThread, 0, sysMgmtStart, 0);
#endif

} // createSMThr ()

//----------------------------------------------------------------------------
// Function	: createApplThr ()
// Description	: It creates  application thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void createApplThr ()
{

	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	//----------------------------------------------------------------------------
	// Creating System Mgmt Thread.
	//----------------------------------------------------------------------------
	pthread_t applThread;
#ifdef __ECOS
	pthread_attr_t applAttr;
	pthread_attr_init (&applAttr);
	pthread_attr_setstackaddr (&applAttr, 
		(void*)&applThreadSpace [sizeof (applThreadSpace)]);
	pthread_attr_setstacksize (&applAttr, 
					sizeof (applThreadSpace));
	pthread_create (&applThread, &applAttr, 
						applStart, 0);
#elif sun
	pthread_attr_t applAttr;
	pthread_attr_init (&applAttr);
	pthread_attr_setscope (&applAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&applThread, &applAttr,
						applStart, 0);
#else
	pthread_create (&applThread, 0, applStart, 0);
#endif

} // createSMThr ()

//----------------------------------------------------------------------------
// Function	: smMgmtEventHlr ()
// Description	: This is Listener Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void smMgmtEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	printf ("Response to System Manager\n");
	
	CdSuNhUdpMiEvent* eventParam = (CdSuNhUdpMiEvent*) arg;
	switch (eventParam->opcode)
	{
	case CDSU_NHUDP_MI_INIT_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiInitRspEvent*)eventParam)->rspCode;
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			++noOfInitRsp;
			printf ("Init Nh Success\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
		} else 
		{
			printf ("Init Nh fail\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
			displayErrorType (((CdSuNhUdpMiInitRspEvent*)eventParam)->failCode);
		}
		}
		break;
		
		case CDSU_NHUDP_MI_DELNH_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiDelNhRspEvent*)eventParam)->rspCode;
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Del Nh Success\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);

		} else 
		{
			printf ("Del Nh fail\n");
			printf ("Instance Id=%d\n", eventParam->nhInstId);
			displayErrorType (((CdSuNhUdpMiDelNhRspEvent*)eventParam)->failCode);
			
		}
		}
		break;

	case CDSU_NHUDP_MI_CTRL_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiCtrlRspEvent*)eventParam)->rspCode;
		CdSuNhUdpSubOpcode subOpcode = ((CdSuNhUdpMiCtrlRspEvent*)eventParam)->subOpcode;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			if (subOpcode == CDSU_NHUDP_SO_START_RSP)
			{
			printf ("Start UDP NH Failed\n");
			} else  if (subOpcode == CDSU_NHUDP_SO_STOP_RSP)
			{
			printf (" Stop UDP NH Failed\n");
			} else
			{
				printf ("Control Operation Failed\n");
			}
			displayErrorType (((CdSuNhUdpMiCtrlRspEvent*)eventParam)->failCode);

		} else 
		{
			if (subOpcode == CDSU_NHUDP_SO_START_RSP)
			{
			++noOfStartRsp;	
			printf ("Start UDP NH Success\n");
			nhInstID [ instId2Index [ eventParam->nhInstId ] ] = eventParam->nhInstId;
			isValidInstId [ instId2Index [ eventParam->nhInstId ] ]= true;

			} else  
			{
			printf (" Stop UDP NH Success\n");
			}
		
		}
		}
		break;
	
	case CDSU_NHUDP_MI_STATUS_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpMiStatusRspEvent*)eventParam)->rspCode;
		CdSuNhUdpState nhState = ((CdSuNhUdpMiStatusRspEvent*)eventParam)->nhState;
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Status Nh Success\n");
			if (nhState == CDSU_NHUDP_ST_CREATED)
			{
			printf ("State Of NH = CREATED\n");
			} else if (nhState == CDSU_NHUDP_ST_RUNNING)
			{
			printf ("State Of NH = RUNNING\n");
			}
		} else 
		{
			printf ("Status Nh fail\n");
			displayErrorType (((CdSuNhUdpMiStatusRspEvent*)eventParam)->failCode);

		}
		}
		break;

	case CDSU_NHUDP_MI_STATISTICS_RSP:
		{
		CdSuNhUdpMiStsRspEvent* stsRspEvnetParam = 
					((CdSuNhUdpMiStsRspEvent*)eventParam);
		CdSuNhUdpRspCode rspCode = stsRspEvnetParam->rspCode;
		
		if (rspCode != CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Statistics Nh Success. Statistics Report==>\n");
			for (int i = 0 ; i < CDSU_NHUDP_MAX_CTR; ++i)
				{
				printf ("COUNTER [%s]== %d\n",  counterString [i], stsRspEvnetParam->counters [i]);
				}

		} else 
		{
			printf ("Statistics Nh fail\n");
			displayErrorType (((CdSuNhUdpMiStsRspEvent*)eventParam)->failCode);

		}
		}
		break;

	} // switch
	
	//Delete event.
	if (!isMiMsgIntf)
	{
		delete eventParam;
	}
	eventParam = NULL;


} // smMgmtEvnetHlr ()

//----------------------------------------------------------------------------
// Function	: applEventHlr()
// Description	: This is Application Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void applEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	printf ("Response to Application\n");
	
	CdSuNhUdpUiEvent* eventParam = (CdSuNhUdpUiEvent*) arg;
	switch (eventParam->opcode)
	{
	case CDSU_NHUDP_UI_ADD_UDPPORT_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		CdSuIpv4UdpPortNo localPortNo=  ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->localPortNo;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Add Udp Port Failed\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
			displayErrorType (failCode);
		} else 
		{
			++noOfAddUdpPortRsp;
			printf ("Add Udp Port Succes\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
			nhUdpPortKey [ portId2Index [ portId ] ] =( (CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->nhUdpPortKey;
			isValidPortKey [ portId2Index [ portId ] ] = true;
		}
		}
		break;
	
	case CDSU_NHUDP_UI_REM_UDPPORT_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiRemUdpPortRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiRemUdpPortRspEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiRemUdpPortRspEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Rem Udp Port Failed\n");
			printf ("Rem Port ID = %d\n", portId);
			displayErrorType (failCode);

		} else 
		{
			printf ("Rem Udp Port Succes\n");
			printf ("Rem Port ID = %d\n", portId);
		}
		}
		break;
	
	case CDSU_NHUDP_UI_DATA_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiDataRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiDataRspEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiDataRspEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		Uint32 opaqueData = ((CdSuNhUdpUiDataRspEvent*)eventParam)->opaqueData;
		time_t t ;
		time (&t);
		timegap = (t - recordedTime) * 1000;

		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			++noOfDataFailRsp;
			printf ("Send Data Failed\n");
			printf ("Port ID = %d\n", portId);
			printf ("Reason %d\n", failCode);
			printf ("Opaque Data %d\n", opaqueData);
			displayErrorType (failCode);

		} else 
		{
			++noOfDataRsp;
			
			printf ("Send Data Succes\n");
			printf ("Port ID = %d\n", portId);
			printf ("Opaque Data %d\n", opaqueData);
		}
		}
		break;
	
	case CDSU_NHUDP_UI_DATA_IND:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiDataIndEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiDataIndEvent*)eventParam)->failCode;
		CdSuNhUdpPortId portId = ((CdSuNhUdpUiDataIndEvent*)eventParam)->nhUdpPortKey.nhUdpPortId;
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Data Ind Failed\n");
			printf ("Port ID = %d\n", portId);
			printf ("Reason %d\n", failCode);
			printf ("Data Len = %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen);
			printf ("Data = %s\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr);
			printf ("Remote PortNo %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo);
			printf ("Dest Addr = %s\n", cdSuNhUdpConvInt2Addr (((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr));
			displayErrorType (((CdSuNhUdpUiDataIndEvent*)eventParam)->failCode);
						
		} else 
		{
			++noOfDataRecvd;
			printf ("Data Indication\n");
			printf ("Port ID = %d\n", portId);
			printf ("Data Len = %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen);
			printf ("Data = %s\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr);
			printf ("Remote PortNo %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo);
			printf ("Dest Addr = %s\n", cdSuNhUdpConvInt2Addr (((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr));
			if (((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen > 0)
			{
				if (doReply)
				{
				sendDataReqParam.udpPortKey = ((CdSuNhUdpUiDataIndEvent*)eventParam)->nhUdpPortKey;
				sendDataReqParam.destPortNo = ((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo;
				sendDataReqParam.destAddr = ((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr;
				sendDataReqParam.dataPtr = ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
				sendDataReqParam.dataLen = ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen;	
				if (isUiMsgIntf)
				{
				sendDataReqParam.applEventHlr = applEventHlr;
				} else
				{
				sendDataReqParam.applEventHlr = NULL;
				}
				++noOfDataSent;
				if (cdSuNhUdpUiDataReq (sendDataReqParam)  == CDSU_NHUDP_API_RT_SUCCESS)
				{
				++noOfDataSentSuccess;
				}

				}
				delete [] ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
			}
			
			//sendData ();
			
		}
		}
		break;
	
	default:
		break;
	} // 	switch (eventParam->opcode)
	if (!isUiMsgIntf)
	{
		delete  eventParam;
	}
	

} // applEventHlr ()

//----------------------------------------------------------------------------
// Function	: resetPortKeyFlag ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void resetPortKeyFlag ()
{
	for (int i = 0; i < MAXPORT; i++)
	{
		isValidPortKey [ i ] = false;
	}
} // void resetPortKeyFlag ()

//----------------------------------------------------------------------------
// Function	: resetInstIdFlag ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void resetInstIdFlag ()
{
	for (int i = 0; i < MAXPORT; i++)
	{
		isValidInstId [ i ] = false;
	}
} // void resetInstIdFlag ()
//----------------------------------------------------------------------------
// Function	: setPorts ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setPorts ()
{
	for (int i = 0; i < MAXPORT; i++)
	{
		setOfLocalPort [ i ] = startingLocalPort + i;
		if (!isSelf)
		{	
			setOfRemotePort [ i ] = startingDestPort + i;
		} else
		{
			setOfRemotePort [ i ] = startingLocalPort + MAXPORT - i - 1;
		}
	}
	
} // void resetInstIdFlag ()

void initAllGlobals ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	CdSuMdInitParam msgDstbInitParam;
	memset ((void *)&msgDstbInitParam.selfMid, 0, sizeof (CdModuleId));
	if (isRemote)
	{
		msgDstbInitParam.ipaddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
	} else
	{
		msgDstbInitParam.ipaddr = cdSuNhUdpConvIp2Int (localIpAddr);
	} // if (isRemote)
	
	msgDstbInitParam.udpLpn = MSGDISTUDPPORT;
	
	if (cdSuMdInit (msgDstbInitParam) == true)
	{
		CDSU_TRACE (1, "Message distributor is created successfully.\n");
	} else 
	{
		printf ("Message distributor creation fail.\n");		
		return ;	
	}

	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	for (int i = 0; i < MAXINST; i++)
	{
	nhModId [i].hwUnitId.rackId = 0;
	nhModId [i].hwUnitId.unitId = 0;
	nhModId [i].hwUnitId.slotId = 0;
	nhModId [i].hwUnitId.procId = 0;

	nhModId [i].srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId [i].srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId [i].srvcHlrId.appSrvcHlrId.appId = i + 1;
	nhModId [i].srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.
	cdSuMdRegisterMid (nhModId [i]);

	}
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (System Management Module ID).
	smModId.hwUnitId.rackId = 0;
	smModId.hwUnitId.unitId = 0;
	smModId.hwUnitId.slotId = 0;
	smModId.hwUnitId.procId = 0;

	smModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	smModId.srvcHlrId.appSrvcHlrId.appType = 2;
	smModId.srvcHlrId.appSrvcHlrId.appId = 2;
	smModId.srvcHlrId.appSrvcHlrId.appModuleId = 2;	// To be changed later.

	// Note: Appl Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Appl Module ID).
	applnModId.hwUnitId.rackId = 0;
	applnModId.hwUnitId.unitId = 0;
	applnModId.hwUnitId.slotId = 0;
	applnModId.hwUnitId.procId = 0;

	applnModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	applnModId.srvcHlrId.appSrvcHlrId.appType = 4;
	applnModId.srvcHlrId.appSrvcHlrId.appId = 4;
	applnModId.srvcHlrId.appSrvcHlrId.appModuleId = 4;	// To be changed later.
	
	//if (cdSuMdRegisterMid ()
	cdSuMdRegisterMid (smModId);	
	cdSuMdRegisterMid (applnModId);	

	// if Message interface then statr thread.
	if (isMiMsgIntf)
	{
		createSMThr ();	
	}
	
	if (isUiMsgIntf)
	{
		createApplThr ();	
	}
	if (isRemote)
	{
		maxPortPerInst = (REMOTEMAXPORT / MAXINST);
	} else
	{
		maxPortPerInst = (MAXPORT / MAXINST);
	}

} // initAllGlobals ()


//----------------------------------------------------------------------------
// Function	: cdSetTraceLevel0
// Description	: set the trace level to zero
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdSetTraceLevel0 ()
{
	CdSuTrace::setLevel (0);
}

//----------------------------------------------------------------------------
// Function	: cdSetTraceLevel1
// Description	: set the trace level to one
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------


void cdSetTraceLevel1 ()
{
	CdSuTrace::setLevel (1);
}

//----------------------------------------------------------------------------
// Function	: cdSetTraceLevel2
// Description	: set the trace level to two
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------


void cdSetTraceLevel2 ()
{
	CdSuTrace::setLevel (2);
}

//----------------------------------------------------------------------------
// Function	: cdSetTraceLevel3
// Description	: set the trace level to three
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdSetTraceLevel3 ()
{
	CdSuTrace::setLevel (3);
}

//----------------------------------------------------------------------------
// Function	: cdSetTraceLevel4
// Description	: set the trace level to four
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdSetTraceLevel4 ()
{
	CdSuTrace::setLevel (4);
}

//----------------------------------------------------------------------------
// Function	: cdSetTraceLevel5
// Description	: set the trace level to five
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdSetTraceLevel5 ()
{
	CdSuTrace::setLevel (5);
}
//----------------------------------------------------------------------------
// Function	: setRemote ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setRemote ()
{
	isRemote = true;
	isSelf = false;
} // setRemote ();

//----------------------------------------------------------------------------
// Function	: setLocal ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setLocal ()
{
	isRemote = false;
	isSelf	= false;

} // setLocal ();

//----------------------------------------------------------------------------
// Function	: setSelf ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setSelf ()
{
	isSelf	= true;
	isRemote = false;
} // setLocal ();

//----------------------------------------------------------------------------
// Function	: setDelay ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setDelay ()
{
	useDelay = true;
} // setDelay ();

//----------------------------------------------------------------------------
// Function	: resetDelay ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void resetDelay ()
{
	useDelay = false;
} // setDelay ();

//----------------------------------------------------------------------------
// Function	: setMsgIntf ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setMsgIntf ()
{
	isMiMsgIntf = true;
	isUiMsgIntf = true;

} // setMsgIntf ();

//----------------------------------------------------------------------------
// Function	: setCallbackIntf ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setCallbackIntf ()
{
	isMiMsgIntf = true;
	isUiMsgIntf = true;

} // setMsgIntf ();

//----------------------------------------------------------------------------
// Function	: setZeroSelTimer ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void setZeroSelTimer ()
{
	useZeroSelTimer = true;
} // void setZeroSelTimer ()

//----------------------------------------------------------------------------
// Function	: loadTestCaseOne ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void makeNhReady ()
{
	CdSuNhUdpMiInitParam initParam;
	
	initAllGlobals ();
	setPorts ();
	resetInstIdFlag ();

	for (int i = 0; i < MAXINST; i++)
	{
		initParam.nhActorId = nhModId [ i ];
		initParam.smActorId = smModId;
		if (isMiMsgIntf)
		{
			initParam.smEventHlr = NULL;
		} else
		{
			initParam.smEventHlr = smMgmtEventHlr;
		}
		
		if (useZeroSelTimer)
		{
			initParam.dataSelectTimer = 0;
		}
		
		// Create Instance.
		if (useDelay)
		{
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = delay;//CDSU_NHUDP_DEF_NOHDL_TMR; 
			nanosleep (&sleepTime, NULL);
		}

		++noOfInit;
		cdSuNhUdpMiInit (initParam, nhInstID [ i ]);
		instId2Index [ nhInstID [ i ] ] = i;
		
		// Start The UDP Nh
		CdSuNhUdpMiCtrlReqParam ctrlReqParam;
		ctrlReqParam.nhUdpInstId = nhInstID [ i ];
		ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
		if (useDelay)
		{
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = delay;//CDSU_NHUDP_DEF_NOHDL_TMR; 
			nanosleep (&sleepTime, NULL);
		}
		++noOfStart;
		cdSuNhUdpMiCtrlReq (ctrlReqParam);
	}
} // void makeNhReady ()

//----------------------------------------------------------------------------
// Function	: loadTestCaseOne ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void doLoadTestOne ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	resetPortKeyFlag ();

	CdSuNhUdpUiAddUdpPortReqParam reqParam;
	reqParam.applActorId =  applnModId;	
	for (int k = 0; k < MAXINST; k ++)
	{
	if (isValidInstId [ k ] == false )
	{
		printf ("================================\n");
	}
	reqParam.nhUdpInstId = nhInstID [ k ];
	for (int i = 0; i < maxPortPerInst; i++)
	{
		printf ("Attempt = %d\n", k*maxPortPerInst + i);
		reqParam.localPortNo = setOfLocalPort [ k*maxPortPerInst + i ];
		reqParam.localAddr = cdSuNhUdpConvIp2Int (localIpAddr); 	
		if(!isUiMsgIntf)
		{
			reqParam.applEventHlr = applEventHlr;
		}
		
		
		cdSuNhUdpUiAddUdpPortReq (reqParam, nhUdpPortKey [ i ]);
		if (nhUdpPortKey [ i ].nhUdpPortId != CDSU_NHUDP_INV_UDPPORTID)
		{
			portId2Index [ nhUdpPortKey [ i ].nhUdpPortId] = i;
			printf ("Add Udp Port Req: Successful. PortId = %d\n", nhUdpPortKey [ i ].nhUdpPortId);
			printf ("Inst ID = %d\n", nhUdpPortKey [ i ].nhUdpInstId );
		} else
		{
			printf ("Add Udp Port Req: Fail\n");
		}
		if (useDelay)
		{
		struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = delay; //CDSU_NHUDP_DEF_NOHDL_TMR; 
		nanosleep (&sleepTime, NULL);
		}
	}
	}
} // doLoadTestOne ()

void addAUdpPort ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;
	
	addUdpPortReqParam.nhUdpInstId	= nhInstID [ 0 ];
	addUdpPortReqParam.applActorId	= applnModId;
	if (!isUiMsgIntf)
	{
		addUdpPortReqParam.applEventHlr = applEventHlr;
	} else
	{
		addUdpPortReqParam.applEventHlr = NULL;
	}
	if (isRemote)
	{
		addUdpPortReqParam.localPortNo = setOfRemotePort [ 0 ];
		addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (remoteIpAddr);
	} else
	{
		addUdpPortReqParam.localPortNo = setOfLocalPort [ 0 ];
		addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	}
	
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, nhUdpPortKey [ 0 ]);
	++noOfAddUdpPort;
	noOfOpenedPort = 1;
} // void addAUdpPort ()

void sendData ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	
	for (int k = 0; k < SENDATTEMPT; k++)
	{
	for (int i = 0; i < noOfOpenedPort; i++)
	{
		int attempt = ( k*noOfOpenedPort +i);
		printf ("Send Data Attempt %d\n", attempt);
		CdSuNhUdpUiDataReqParam dataReqParam;
		dataReqParam.applActorId = applnModId;
		if (!isUiMsgIntf)
		{
		dataReqParam.applEventHlr = applEventHlr;
		} else
		{
		dataReqParam.applEventHlr = NULL;
		}
		dataReqParam.udpPortKey = nhUdpPortKey [ i ];
		sprintf (tempData, "%d", attempt);
		strcat (tempData, data);
		dataReqParam.dataPtr = tempData;
		dataReqParam.dataLen = strlen (tempData) + 1;
		dataReqParam.opaqueData = i;
		if (isRemote)
		{
			dataReqParam.destPortNo = setOfLocalPort [ 0 ];
			dataReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);

		} else
		{
			if (isSelf)
			{
				dataReqParam.destPortNo = setOfLocalPort [ 0 ];
				dataReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);
			} else
			{
				dataReqParam.destPortNo = setOfRemotePort [ 0 ];
				dataReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
			}
		}
 		if (useDelay)
		{
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = delay;//CDSU_NHUDP_DEF_NOHDL_TMR; 
			nanosleep (&sleepTime, NULL);
		}

		// Send Data.
		++noOfDataSent;
		if (cdSuNhUdpUiDataReq (dataReqParam) == CDSU_NHUDP_API_RT_SUCCESS)
		{
			++ noOfDataSentSuccess;
		}	
	}
	}
} // void sendData ()

void sendDataForNUdpPort ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpUiDataReqParam dataReqParam;
	dataReqParam.applActorId = applnModId;
	if (!isUiMsgIntf)
	{
		dataReqParam.applEventHlr = applEventHlr;
	} else
	{
		dataReqParam.applEventHlr = NULL;
	}
	
	time (&recordedTime);
	
	for (int k = 0; k < SENDATTEMPT; k++)
	{
	for (int i = 0; i < noOfOpenedPort; i++)
	{
		if (useDelay)
		{
		struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = delay;//CDSU_NHUDP_DEF_NOHDL_TMR; 
		nanosleep (&sleepTime, NULL);
		}

		dataReqParam.udpPortKey = nhUdpPortKey [ i ];
		for (int j = 0; j < REMOTEMAXPORT; j++)
		{	
			int attempt = (k*noOfOpenedPort*MAXPORT + i*noOfOpenedPort + j);
			// printf ("Send Data Attempt %d(%d, %d, %d)\n", attempt, k , i , j);
			sprintf (tempData, "%d", attempt);
			strcat (tempData, data);
			dataReqParam.dataPtr = tempData;
			dataReqParam.dataLen = strlen (tempData) + 1;
			dataReqParam.opaqueData = attempt;
			if (isRemote)
			{
			dataReqParam.destPortNo = setOfLocalPort [ j ];
			dataReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);
			} else
			{
			
				dataReqParam.destPortNo = setOfRemotePort [ j ];
				if (isSelf)
				{
				dataReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);
				} else
				{
				dataReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
				}
			}
		
                	if (useDelay)
                	{
                	printf ("Delay....................................\n");
			struct timespec sleepTime;
               	 	sleepTime.tv_sec = 0;
                	sleepTime.tv_nsec = delay;//CDSU_NHUDP_DEF_NOHDL_TMR;
                	nanosleep (&sleepTime, NULL);
                	}
			// Send Data.
			++noOfDataSent;
			if (cdSuNhUdpUiDataReq (dataReqParam) == CDSU_NHUDP_API_RT_SUCCESS)
			{
				++noOfDataSentSuccess;
                		printf ("Send Data Success [%d] ...................................\n", noOfDataSentSuccess);
			} else
			{
				printf ("Send Data Failed [%d]...................................\n", noOfDataSentSuccess);
			}
		}
	} // for ()
 	} // for ()
	

} // sendDataForNUdpPor ()

void addNUdpPort ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;
	noOfOpenedPort = MAXINST * maxPortPerInst;
	for (int k = 0; k < MAXINST; k++)
	{
		for (int i = 0; i < maxPortPerInst; i++)
		{
			printf ("Add Udp Port Attempt %d\n", k*maxPortPerInst + i);
			addUdpPortReqParam.nhUdpInstId	= nhInstID [ k ];
			addUdpPortReqParam.applActorId	= applnModId;
			if (!isUiMsgIntf)
			{
				addUdpPortReqParam.applEventHlr = applEventHlr;
			} else
			{
				addUdpPortReqParam.applEventHlr = NULL;
			}
			if (isRemote)
			{
			addUdpPortReqParam.localPortNo = setOfRemotePort [ k*maxPortPerInst + i ];
			addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (remoteIpAddr);
			} else
			{
			addUdpPortReqParam.localPortNo = setOfLocalPort [ k*maxPortPerInst + i ];
			addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
			}
			if (useDelay)
			{
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = delay;//CDSU_NHUDP_DEF_NOHDL_TMR; 
			nanosleep (&sleepTime, NULL);
			}

			rtType = cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, 	nhUdpPortKey [ k*maxPortPerInst + i ]);
			++noOfAddUdpPort;
			if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
			{
			printf ("Add Udp Port Req: Successful\n");
			} else
			{
			printf ("Add Udp Port Req: Fail\n");
			}
 			
		}
	}
	

} // void addNUdpPort ()

void printStats ()
{
	printf ("No Of Init Request %d\n", noOfInit);
	printf ("No Of Init Response%d\n", noOfInitRsp);
	printf ("No Of Nh Start Request %d\n", noOfStart);
	printf ("No Of Nh Start Response %d\n", noOfStartRsp);
	printf ("No Of Add Udp Port Request %d\n", noOfAddUdpPort);
	printf ("No Of Add Udp Port Response %d\n", noOfAddUdpPortRsp);
	printf ("No Of Data Recvd %d\n", noOfDataRecvd);
	printf ("No Of Data Sent %d\n", noOfDataSent);
	printf ("No Of Data Sent Success %d\n", noOfDataSentSuccess);
	printf ("No Of Data Response %d\n", noOfDataRsp);
	printf ("No Of Data fail Response %d\n", noOfDataFailRsp);
	printf ("Milli Seconds Spent %ld\n", timegap);
	printf ("No Of Data Sent per milli seconds %lf\n", (double)noOfDataRsp /
	timegap);
	printf ("No Of Data Sent Seconds %lf\n", (double)noOfDataRsp *1000/timegap );
}
void resetStats ()
{
	noOfInit = 0;
	noOfStart = 0;
	noOfInitRsp = 0;
	noOfStartRsp = 0;
	noOfStop = 0;
	noOfStopRsp = 0;
	noOfDel =0;
	noOfDelRsp = 0;
	noOfTerm = 0;
	noOfAddUdpPort = 0;
	noOfAddUdpPortRsp = 0;
	noOfDataSent = 0;
	noOfDataSentSuccess = 0;
	noOfDataRsp = 0;
	noOfDataFailRsp = 0;
	noOfDataRecvd = 0;
	noOfUdpPortRem = 0;
	noOfUdpPortRemRsp = 0;

}

//============================================================================
// <End of testfunctions.cpp>
//============================================================================

