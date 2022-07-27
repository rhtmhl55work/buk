
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : bukudpfunctions.cpp
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


// Helper Functions
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
	printf ("Error Type %s\n", failCodeString [failCode]);
}

void displayApiRtType (CdSuNhUdpApiRetType apiRtType)
{
	printf ("Return Type %s\n", apiRtTypeString [apiRtType]);
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

		case CDSU_NHUDP_UI_SEND_RSP:
		{
		applEventHlr ( (void*) &( (CdSuNhUdpUiSendRspMsg*)cdMessage)->sendRspEvent);	
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
			printf ("Start UDP NH Success\n");
			} else if (subOpcode == CDSU_NHUDP_SO_STOP_RSP)
			{
			printf (" Stop UDP NH Success\n");
			} else
			{
			printf ("Invalid Success\n");
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
				statCounterOfNh [i] =  stsRspEvnetParam->counters [i];
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
	if (!ISMIMSGINTF)
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
			displayErrorType (failCode);
		} else 
		{
			printf ("Add Udp Port Succes\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
			nhUdpPortKey = ((CdSuNhUdpUiAddUdpPortRspEvent*)eventParam)->nhUdpPortKey;
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
		
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Send Data Failed\n");
			printf ("Port ID = %d\n", portId);
			printf ("Reason %d\n", failCode);
			printf ("Opaque Data %d\n", opaqueData);
			displayErrorType (failCode);

		} else 
		{
			printf ("Send Data Succes\n");
			printf ("Port ID = %d\n", portId);
			printf ("Opaque Data %d\n", opaqueData);
		}
		}
		break;

	case CDSU_NHUDP_UI_SEND_RSP:
		{
		CdSuNhUdpRspCode rspCode = ((CdSuNhUdpUiSendRspEvent*)eventParam)->rspCode;
		CdSuNhUdpRspFailCode failCode = ((CdSuNhUdpUiSendRspEvent*)eventParam)->failCode;
		CdSuNhUdpInstId nhInstId; // = ((CdSuNhUdpUiSendRspEvent*)eventParam)->nhUdpInstId;
		Uint32 opaqueData = ((CdSuNhUdpUiSendRspEvent*)eventParam)->opaqueData;
		
		if (rspCode == CDSU_NHUDP_RSPCODE_FAIL)
		{
			printf ("Send Data Failed\n");
			printf ("NH Inst ID = %d\n", nhInstId);
			printf ("Reason %d\n", failCode);
			printf ("Opaque Data %d\n", opaqueData);
			displayErrorType (failCode);

		} else 
		{
			printf ("Send Data Succes\n");
			printf ("NH Inst ID = %d\n", nhInstId);
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
			printf ("Data Indication\n");
			printf ("Port ID = %d\n", portId);
			printf ("Data Len = %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen);
			printf ("Data = %s\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr);
			printf ("Remote PortNo %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo);
			printf ("Dest Addr = %s\n", cdSuNhUdpConvInt2Addr (((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr));
			if (((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen > 0)
			{
				if (DOREPLY)
				{
				sendDataReqParam.udpPortKey = ((CdSuNhUdpUiDataIndEvent*)eventParam)->nhUdpPortKey;
				sendDataReqParam.destPortNo = ((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo;
				sendDataReqParam.destAddr = ((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr;
				sendDataReqParam.dataPtr = ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
				sendDataReqParam.dataLen = ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataLen;	
				cdSuNhUdpUiDataReq (sendDataReqParam);

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
	if (!ISUIMSGINTF)
	{
		delete  eventParam;
	}
	

} // applEventHlr ()

void setParameterValue ()
{
	if (!useInvNhModId)
	{
		initParam.nhActorId = nhModId;
	} else
	{
		//TODO:
	}

	if (!useInvSmModId)
	{
		initParam.smActorId = smModId;
	} else
	{
		//TODO:
	}

	if (!useInvApplModId)
	{
		addUdpPortReqParam.applActorId	= applnModId;
		remUdpPortReqParam.applActorId = applnModId;
		sendDataReqParam.applActorId = applnModId;
		sendReqParam.applActorId = applnModId;

	} else
	{
		// TODO:
	}

	if (useZeroSelTimer)
	{
		initParam.dataSelectTimer = 0;
	} else if (useMoreThanMaxSelTimer)
	{
		initParam.dataSelectTimer = CDSU_NHUDP_DEF_DATA_SELTMR +  1;
	} else
	{
		initParam.dataSelectTimer = CDSU_NHUDP_DEF_DATA_SELTMR;
	}
	
	if (useZeroNoHlrTimer)
	{
		initParam.noHandleTimer = 0;
	} else if (useMoreThanMaxNoHlrTimer)
	{
		initParam.noHandleTimer = CDSU_NHUDP_DEF_NOHDL_TMR + 1;
	} else
	{
		initParam.noHandleTimer = CDSU_NHUDP_DEF_NOHDL_TMR;
	}

	if (useMoreThanMaxBufferLen)
	{
		initParam.maxBufferLen = CDSU_NHUDP_DEF_BUFLEN + 1;
	} else if (useZeroBufferLen)
	{
		initParam.maxBufferLen = 0;
	} else 
	{
		initParam.maxBufferLen = CDSU_NHUDP_DEF_BUFLEN;
	}

	if (useMoreThanMaxUdpPort)
	{
		initParam.maxNoOfUdpPort = CDSU_NHUDP_MAX_UDPPORT + 1;
	} else
	{
		initParam.maxNoOfUdpPort = CDSU_NHUDP_MAX_UDPPORT;
	}
			

	if (useMoreThanMaxNhInstId)
	{
		terminateParam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		delnhparam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		ctrlReqParam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		cfgParam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		statusReqParam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		statsReqParam.nhUdpInstId  = CDSU_NHUDP_MAX_INST;
		
		addUdpPortReqParam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		remUdpPortReqParam.udpPortKey.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		sendDataReqParam.udpPortKey.nhUdpInstId = CDSU_NHUDP_MAX_INST;
		//sendReqParam.nhUdpInstId = CDSU_NHUDP_MAX_INST;
	} else if (useInvNhInstId)
	{
		terminateParam.nhUdpInstId = INVALNHINSTID;
		delnhparam.nhUdpInstId = INVALNHINSTID;
		ctrlReqParam.nhUdpInstId = INVALNHINSTID;
		cfgParam.nhUdpInstId = INVALNHINSTID;
		statusReqParam.nhUdpInstId = INVALNHINSTID;
		statsReqParam.nhUdpInstId  = INVALNHINSTID;
		
		addUdpPortReqParam.nhUdpInstId = INVALNHINSTID;
		remUdpPortReqParam.udpPortKey.nhUdpInstId = INVALNHINSTID;
		sendDataReqParam.udpPortKey.nhUdpInstId = INVALNHINSTID;
		//sendReqParam.nhUdpInstId = INVALNHINSTID;
	}else 
	{
		terminateParam.nhUdpInstId = nhInstID;
		delnhparam.nhUdpInstId = nhInstID;
		ctrlReqParam.nhUdpInstId = nhInstID;
		cfgParam.nhUdpInstId = nhInstID;
		statusReqParam.nhUdpInstId = nhInstID;
		statsReqParam.nhUdpInstId  = nhInstID;
		addUdpPortReqParam.nhUdpInstId = nhInstID;
		remUdpPortReqParam.udpPortKey.nhUdpInstId = nhInstID;
		sendDataReqParam.udpPortKey.nhUdpInstId = nhInstID;
		//sendReqParam.nhUdpInstId = nhInstID;
	} 	
	
	if (useInvPortId)
	{
		remUdpPortReqParam.udpPortKey.nhUdpPortId = INVALUDPPORTID ;
		sendDataReqParam.udpPortKey.nhUdpPortId = INVALUDPPORTID ;
	} else
	{
		remUdpPortReqParam.udpPortKey.nhUdpPortId = nhUdpPortKey.nhUdpPortId ;
		sendDataReqParam.udpPortKey.nhUdpPortId = nhUdpPortKey.nhUdpPortId;
	}



	if (localPortType == STANDARD)
	{
		addUdpPortReqParam.localPortNo = LOCALPORT; 
	} else if (localPortType == EPHERMAL)
	{
		addUdpPortReqParam.localPortNo = 0;
	} else if (localPortType == DIF_LCL_PORT)
	{
		addUdpPortReqParam.localPortNo = EXTRADESTPORT;
	} else 
	{
		addUdpPortReqParam.localPortNo = CDSU_NHUDP_INV_UDPPORT;
	}

	if (destPortType == DEST_DEFAULT)
	{
		addUdpPortReqParam.destPortNo = CDSU_NHUDP_INV_UDPPORT;
		sendDataReqParam.destPortNo = CDSU_NHUDP_INV_UDPPORT;
		sendReqParam.destPortNo = CDSU_NHUDP_INV_UDPPORT;
	} else if (destPortType == SELF_PORT)
	{
		sendDataReqParam.destPortNo = LOCALPORT;
		sendReqParam.destPortNo = LOCALPORT;
	}
       	else if (destPortType == ZERO) 
	{
		addUdpPortReqParam.destPortNo = 0;
		sendDataReqParam.destPortNo = 0;
		sendReqParam.destPortNo = 0;
	} else if (destPortType == DIF_DEST_PORT) 
	{
		// addUdpPortReqParam.destPortNo = 0;
		sendDataReqParam.destPortNo = EXTRADESTPORT;
		sendReqParam.destPortNo = EXTRADESTPORT;
	} else
	{
		addUdpPortReqParam.destPortNo = DESTPORT;
		sendDataReqParam.destPortNo = DESTPORT;
		sendReqParam.destPortNo = DESTPORT;
	}
	

	if (useDiffLocalAddr)
	{
		addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (diffLocalIpAddr);
	} else
	{
		addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	}
	
	if (useDiffDestAddr)
	{
		addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (diffRemoteIpAddr);
		sendDataReqParam.destAddr = cdSuNhUdpConvIp2Int (diffRemoteIpAddr);
		sendReqParam.destAddr = cdSuNhUdpConvIp2Int (diffRemoteIpAddr);
	} else if (useLocalAddrAsDestAddr)
	{
		addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);
		sendDataReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);
		sendReqParam.destAddr = cdSuNhUdpConvIp2Int (localIpAddr);
	} else
	{
		addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
		sendDataReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
		sendReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
	}

	if (noAckReq)
	{
		sendDataReqParam.isAckReq = false;
	} else
	{
		sendDataReqParam.isAckReq = true;
	}	
	
	if(!ISUIMSGINTF)
	{
		initParam.smEventHlr = smMgmtEventHlr;
		sendDataReqParam.applEventHlr = applEventHlr;
		sendReqParam.applEventHlr = applEventHlr;
		remUdpPortReqParam.applEventHlr = applEventHlr;
		addUdpPortReqParam.applEventHlr = applEventHlr;

	} else
	{
		initParam.smEventHlr =  (void (*) (void *)) CDSU_NHUDP_INV_UDPEVENTHLR;	
		sendDataReqParam.applEventHlr = (void (*) (void *)) CDSU_NHUDP_INV_UDPEVENTHLR;	
		sendReqParam.applEventHlr = (void (*) (void *)) CDSU_NHUDP_INV_UDPEVENTHLR;	
		remUdpPortReqParam.applEventHlr = (void (*) (void *)) CDSU_NHUDP_INV_UDPEVENTHLR;	 
		addUdpPortReqParam.applEventHlr = (void (*) (void *)) CDSU_NHUDP_INV_UDPEVENTHLR;	 
	}

	
}
void setDefaultValueToParameter ()
{

	useInvNhInstId = false; 	
	useMoreThanMaxNhInstId = false;
	useInvApplModId = false;	
	useInvSmModId = false;		
	useInvNhModId = false;	
	useZeroSelTimer	= false;
	useMoreThanMaxSelTimer = false;
	useZeroNoHlrTimer = false;
	useMoreThanMaxNoHlrTimer = false;
	useZeroBufferLen = false; 
	useMoreThanMaxBufferLen = false;
	useMoreThanMaxUdpPort = false;
	useInvPortId = false; 
	useInvDataPtr = false;
	useZeroDataLen  = false;
	useDiffLocalAddr = false;
	useDiffDestAddr = false;
	useLocalAddrAsDestAddr = false;
	localPortType = STANDARD;
	destPortType = DEST_DEFAULT;
	noAckReq = false;
}

void displayParamSetting ()
{
	
	if (useInvNhInstId)
	{
		printf ("InValid Nh Instance ID\n");
	} else if (useMoreThanMaxNhInstId)
	{
		printf ("Instance ID More Than max");
	} else
	{
		printf ("Valid NH Instance ID\n");
	}

	if (useInvNhModId)
	{
		printf ("Invalid NH Actor ID\n");
	} else
	{
		printf ("Valid NH Actor ID\n");
	}
	
	if (useInvSmModId)
	{
		printf ("Invalid SM Actor ID\n");
	} else
	{
		printf ("Valid SM Actor ID\n");
	}

	if (useInvApplModId)
	{
		printf ("Invalid Application Actor ID\n");
	} else
	{
		printf ("Valid Application Actor ID\n");
	}
	
	if (useZeroSelTimer)
	{
		printf ("Zero Select Timer\n");
	} else if (useMoreThanMaxSelTimer)
	{
		printf ("Select Timer More Than Max\n");
	} else
	{
		printf ("Valid Select Timer\n");
	}

	if (useZeroNoHlrTimer)
	{
		printf ("Zero NO Hlr Timer\n");
	} else if (useMoreThanMaxNoHlrTimer)
	{
		printf ("No HLr Timer More Than Max\n");
	} else
	{
		printf ("Valid No Hlr Timer\n");
	}

	if (useZeroBufferLen)
	{
		printf ("Zero Buffer Len\n");
	} else if (useMoreThanMaxBufferLen)
	{
		printf ("Buffer Len more than Max\n");
	} else 
	{
		printf ("Valid Buffer Len\n");	
	}
	
	if (useMoreThanMaxUdpPort)
	{
		printf ("No Of Udp Port More Than Max\n");
	} else
	{
		printf ("No Of Udp Port Not More Than Max\n");
	}
	
	if (!useInvPortId)
	{
		printf ("Valid Port ID\n");
	} else
	{
		printf ("Invalid Port ID\n");
	}

	if (!useInvDataPtr)
	{
		printf ("Valid Data Pointer\n");
	} else
	{
		printf ("Invalid Data Pointer\n");
	}

	if (useZeroDataLen)
	{
		printf ("Data Len Zero\n");
	} else
	{
		printf ("Data Len Non Zero\n");
	}

	if (localPortType == STANDARD)
	{
		printf ("Local Port Stadard\n");
	} else if (localPortType == EPHERMAL)
	{
		printf ("Local Port Ephermal\n");
	} else if (localPortType == DIF_LCL_PORT) 
	{
		printf ("local Port Different\n");
	} else 
	{
		printf ("Locla Port Default\n");
	}

	if (destPortType == DEST_DEFAULT)
	{
		printf ("Dest Port Default\n");
	} else if (destPortType == ZERO) 
	{
		printf ("Dest Port Zero\n");
	} else if (destPortType == DIF_DEST_PORT) 
	{
		printf ("Dest Port Different\n");
	} else
	{
		printf ("Dest Port Valid One\n");
	}
	
	if (useDiffLocalAddr)
	{
		printf ("Local Address Different Than local Machine Address\n");
	} else
	{
		printf ("Local Address Same As Local Machines\n");
	}
	
	if (useDiffDestAddr)
	{
		printf ("Dest Address different than connected one\n");
	} else
	{
		printf ("Dest Address same as connected one\n");
	}
	setParameterValue ();
	printf ("--------------------->INIT Param<-----------------------\n");
	printf ("Use NH Actor ID = %d\n", useInvNhModId);	
	printf ("Use SM Acort ID = %d\n", useInvNhModId);
	printf ("max No of UDP Port %d\n", initParam.maxNoOfUdpPort);
	printf ("max Bufer Len %d\n", initParam.maxBufferLen);
	printf ("Data Sel Timer = %d\n", initParam.dataSelectTimer);
	printf ("NO Hlr Timer =%d\n", initParam.noHandleTimer);
	printf ("Continye?[y/n] \n");
	
	printf ("--------------------->Terminate Param<-----------------------\n");
	printf ("Nh Instance ID %d\n", terminateParam.nhUdpInstId);
	
	printf ("--------------------->DelNh Param <-----------------------\n");
	printf ("Nh Instance ID %d\n", delnhparam.nhUdpInstId);

	printf ("--------------------->Configure Nh param <-----------------------\n");
	printf ("Nh Instance ID %d\n", cfgParam.nhUdpInstId);
	
	printf ("--------------------->Ctrl Nh param <-----------------------\n");
	printf ("Nh Instance ID %d\n", ctrlReqParam.nhUdpInstId);
	
	printf ("--------------------->Status Nh param <-----------------------\n");
	printf ("Nh Instance ID %d\n", statusReqParam.nhUdpInstId);

	printf ("--------------------->Stats Nh param <-----------------------\n");
	printf ("Nh Instance ID %d\n", statsReqParam.nhUdpInstId);

	printf ("--------------------->Add UdpPort param <-----------------------\n");
	printf ("Nh Instance ID %d\n", addUdpPortReqParam.nhUdpInstId);
	printf ("Local Port NO =%d\n", addUdpPortReqParam.localPortNo);
	printf ("Local Addr %s\n", cdSuNhUdpConvInt2Addr (addUdpPortReqParam.localAddr));
	printf ("Dest Port NO =%d\n", addUdpPortReqParam.destPortNo);
	printf ("Dest Addr %s\n", cdSuNhUdpConvInt2Addr (addUdpPortReqParam.destAddr));
	printf ("Use Appl Actor ID = %d\n", useInvApplModId);	
	
	printf ("--------------------->Data Req param <-----------------------\n");
	printf ("Nh Instance ID %d\n", sendDataReqParam.udpPortKey.nhUdpInstId);
	printf ("Udp Port ID  =%d\n", sendDataReqParam.udpPortKey.nhUdpPortId);
	printf ("Dest Port NO =%d\n", sendDataReqParam.destPortNo);
	printf ("Dest Addr %s\n", cdSuNhUdpConvInt2Addr (sendDataReqParam.destAddr));
	printf ("Use Appl Actor ID = %d\n", useInvApplModId);
	
	printf ("--------------------->Rem Req param <-----------------------\n");
	printf ("Nh Instance ID %d\n", remUdpPortReqParam.udpPortKey.nhUdpInstId);
	printf ("Udp Port ID  =%d\n", remUdpPortReqParam.udpPortKey.nhUdpPortId);
	
}

void initAllGlobals ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId.hwUnitId.rackId = 0;
	nhModId.hwUnitId.unitId = 0;
	nhModId.hwUnitId.slotId = 0;
	nhModId.hwUnitId.procId = 0;

	nhModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId.srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appId = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.
	
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

	CdSuMdInitParam msgDstbInitParam;
	memset ((void *)&msgDstbInitParam.selfMid, 0, sizeof (CdModuleId));
	msgDstbInitParam.ipaddr = cdSuNhUdpConvIp2Int (localIpAddr);

	if (cdSuMdInit (msgDstbInitParam) == true)
	{
		CDSU_TRACE (1, "Message distributor is created successfully.\n");
	} else 
	{
		printf ("Message distributor creation fail.\n");		
		return ;	
	}

	//if (cdSuMdRegisterMid ()
	cdSuMdRegisterMid (nhModId);
	cdSuMdRegisterMid (smModId);	
	cdSuMdRegisterMid (applnModId);	

	// if Message interface then statr thread.
	if (ISMIMSGINTF)
	{
		createSMThr ();	
	}
	
	if (ISUIMSGINTF)
	{
		createApplThr ();	
	}


} // initAllGlobals ()

// Seting Parameter Functions :
// Each functions sets different type of parameter. Each Parameter has some
// Default value. Using Following functions tester can alterate the value of
// parameters.

// Set Invalid Instance ID.
void setInvNhInstId ()
{
	useInvNhInstId = true;
	useMoreThanMaxNhInstId = false;
} // void setInvNhInstId ()

void setInstIdMoreThanMax ()
{
	useInvNhInstId = false;
	useMoreThanMaxNhInstId = true;
}

//Set Invalid SM actor ID.
void setInvSmActorId ()
{
	useInvSmModId	= true;
} //void setInvSmActorId ()

// Set Invalid Application Actro ID.
void setInvApplActorId ()
{
	useInvApplModId = true;
} // void setInvApplActorId ()

// Set Invalid NH Actor ID.
void setInvNhActorId ()
{	
	useInvNhModId = true;
} // void setInvNhActorId ()

// set Select Timer to Zero
void setSelTimerZero ()
{
	useZeroSelTimer = true;
	useMoreThanMaxSelTimer = false;
} // void setSelTimerZero ()

void setSelTimerMoreThanMax ()
{
	useZeroSelTimer = false;
	useMoreThanMaxSelTimer = true;
} 

// set No Hlr Timer
void setNoHlrTimerZero ()
{
	useZeroNoHlrTimer = true;
	useMoreThanMaxNoHlrTimer = false;
} // void setSelTimerZero ()

void setNoHlrTimerMoreThanMax ()
{
	useZeroNoHlrTimer = false;
	useMoreThanMaxNoHlrTimer = true;
}
// Set Invalid Buffer Len
void setBufLenZero ()
{
	useZeroBufferLen = true;
	useMoreThanMaxBufferLen = false;
} // void setInvBufLen ()

void setBufLenMoreThanMax ()
{
	useZeroBufferLen = false;
	useMoreThanMaxBufferLen = true;
}

void setNoOfUdpPortMoreThanMax ()
{
	useMoreThanMaxUdpPort = true;
}

// Setting Local Port to 0
void setLocalPortZero ()
{
	localPortType = EPHERMAL;
} // void setLocalPortZero ()

// Setting Local Port to Different Than Connected
void setLocalPortDif ()
{
	localPortType = DIF_LCL_PORT;
} // void setLocalPortDif

// Setting Local port to default value
void dontSetLocalPort ()
{
	localPortType = LOCAL_DEFAULT;
} // void dontSetLocalPort ()

// Setting Dest Port To Zero
void setDestPortZero ()
{
	destPortType = ZERO;
} // void setDestPortZero ()

// Setting Dest Port To valid
void setDestPort ()
{
	destPortType = VALID;
} // void setDestPort ()

// Setting Dest Port To valid
void setDestPortSelf ()
{
	destPortType = SELF_PORT;
} // void setDestPort ()

void setDestPortDiff ()
{
	destPortType = DIF_DEST_PORT;
} // void setDestPortDiff ()

// set Invalid Port ID
void setInvPortId ()
{
	useInvPortId = true;
} // void setInvPortId ()

// Setting Invalid Data Pointer
void setInvDataPtr ()
{
	useInvDataPtr = true;
} // void setInvDataPtr ()

// Setting Data Len to 0
void setDataLenZero ()
{
	useZeroDataLen = true;
} // void setInvDataPtr ()



// Setting Local Address different than local machine
void setLocalAddrDif ()
{
	useDiffLocalAddr = true;
} // void setLocalAddrDif ()

// Setting Destination Address different than connected one.
void setDestAddrDif ()
{
	useDiffDestAddr = true;
} // void setDestAddrDif ()

void setLocalAddrToDest ()
{
	useLocalAddrAsDestAddr = true;
}
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

void initSetup ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	initAllGlobals ();
}

void initUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
			
	setParameterValue ();
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiInit (initParam, nhInstID);

	if (rtType != CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Udp network handler creation failed.\n");
		displayApiRtType (rtType);
	} else
	{
		printf ("UDP network handler creation success. Instance Id = %d.\n", nhInstID);
	}
	// Set All The Parameter to Default
	setDefaultValueToParameter ();

}

//----------------------------------------------------------------------------
// Function	: terminateUdpNh ()
// Description	: It terminates UDP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void terminateUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
	CdSuNhUdpApiRetType rtType =  cdSuNhUdpMiTerminate(terminateParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("NH Termiated successfully\n");
	} else
	{
		printf ("NH  Terminate: Fail\n");
		displayApiRtType (rtType);
	}
	
	// Set All The Parameter to Default
	setDefaultValueToParameter ();


} // deleteUdpNh ()


//----------------------------------------------------------------------------
// Function	: deleteUdpNh ()
// Description	: It deletes UDP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void deleteUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	setParameterValue ();

	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiDelNhReq(delnhparam);
	
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Delete NH  req: Successful\n");
	} else
	{
		printf ("Delete NH  req: Fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();

} // deleteUdpNh ()

void invCtrlUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
		
	CdSuNhUdpApiRetType rtType;
	
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_INITIAL_CFG_REQ;
	rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}
	
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_GENERAL_CFG_REQ;
	rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}

	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_INITIAL_CFG_RSP;
	rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}

	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_GENERAL_CFG_RSP;
	rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}

	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_CRE_RSP;
	rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}



	
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_INVALID;
	rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}


	// Set All The Parameter to Default
	setDefaultValueToParameter ();

}

void invalidConfig ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	setParameterValue ();
	CdSuNhUdpApiRetType rtType;
	
	cfgParam.subOpcode = CDSU_NHUDP_SO_CRE_REQ;
	rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Config Req Success\n");
	} else
	{
		printf ("Invalid Config Req Fail\n");
		displayApiRtType (rtType);
	}

	cfgParam.subOpcode = CDSU_NHUDP_SO_DEL_REQ;
	rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Config Req Success\n");
	} else
	{
		printf ("Invalid Config Req Fail\n");
		displayApiRtType (rtType);
	}
	
	cfgParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Config Req Success\n");
	} else
	{
		printf ("Invalid Config Req Fail\n");
		displayApiRtType (rtType);
	}

	cfgParam.subOpcode = CDSU_NHUDP_SO_STOP_REQ;
	rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}

	cfgParam.subOpcode = CDSU_NHUDP_SO_INITIAL_CFG_RSP;
	rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Invalid Ctrl NH Req : Successful\n");
	} else
	{
		printf ("Invalid Ctrl NH Req : fail\n");
		displayApiRtType (rtType);
	}


	// Set All The Parameter to Default
	setDefaultValueToParameter ();

} 


void initialConfig ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	cfgParam.subOpcode = CDSU_NHUDP_SO_INITIAL_CFG_REQ;
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Nh Initial Config Req Success\n");
	} else
	{
		printf ("Nh Initial Config Req Fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();

} 

void generalConfig ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	setParameterValue ();
	
	cfgParam.subOpcode = CDSU_NHUDP_SO_GENERAL_CFG_REQ;
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiCfgReq (cfgParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Nh General Config Req Success\n");
	} else
	{
		printf ("Nh General Config Req Fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();
} 

void startUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();


} 
//----------------------------------------------------------------------------
// Function	: stopUdpNh ()
// Description	: It stops TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void stopUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_STOP_REQ;
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiCtrlReq (ctrlReqParam);	 
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Stop NH  Req: Successful\n");
	} else
	{
		printf ("Stop NH  Req: Fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();

} // stopUdpNh ()

void statusReqUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiStatusReq (statusReqParam);
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Status NH  Req: Successful\n");
	} else
	{
		printf ("Status NH  Req: Fail\n");
		displayApiRtType (rtType);
	}
	
	// Set All The Parameter to Default
	setDefaultValueToParameter ();


} // statusReqUdpNh ()

//----------------------------------------------------------------------------
// Function	: statsReqUdpNh ()
// Description	: It requests for statistics of TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void statsReqUdpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpMiStsReq (statsReqParam);
	
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Stats NH  Req: Successful\n");
	} else
	{
		printf ("Stats NH  Req: Fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();

} // statsReqUdpNh ()


//----------------------------------------------------------------------------
// Function	: addUdpPort ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void addUdpPort ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
	//addUdpPortReqParam.nhUdpInstId = nhInstID;
	//addUdpPortReqParam.applActorId =  applnModId;	
	
	// Local Fixed
	//addUdpPortReqParam.localPortNo = localUdpPort;
	//addUdpPortReqParam.localAddr = cdSuNhUdpConvIp2Int (localIpAddr); 	
	
	// Destination Fixed
	//addUdpPortReqParam.destPortNo = remoteUdpPort;
	//addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
	
	CdSuNhUdpPortKey key;
	CdSuNhUdpApiRetType rtType = cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, key);
	
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS && key.nhUdpPortId != CDSU_NHUDP_INV_UDPPORTID)
	{
		//nhUdpPortKey = tempKey;
		printf ("Add Udp Port Req: Successful\n");
	} else
	{
		printf ("Add Udp Port Req: Fail\n");
		displayApiRtType (rtType);
	}

	// Set All The Parameter to Default
	setDefaultValueToParameter ();

} // addUdpPort ()

//----------------------------------------------------------------------------
// Function	: remUdpPort ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void remUdpPort ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	CdSuNhUdpApiRetType rtType = cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
	
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Rem Udp Port Req: Successful\n");
	} else
	{
		printf ("Rem Udp Port Req: Fail\n");
		displayApiRtType (rtType);
	}
	
	// Set All The Parameter to Default
	setDefaultValueToParameter ();


} // Rem UdpPort ()

void setNoAckReq ()
{
	noAckReq = true;
} 
//----------------------------------------------------------------------------
// Function	: sendData ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void sendData ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	counter++;
	counter = counter % maxCounter;
	
	if (useAllOpenedUdpPort)
	{
		sendDataReqParam.udpPortKey.nhUdpPortId = 0;
	}
	
	if (!useInvDataPtr)
	{
		sendDataReqParam.dataPtr = data [counter];
		sendDataReqParam.dataLen = strlen (data [counter]) +1;

	} else
	{
		sendDataReqParam.dataPtr = INVALDATAPTR;
	}
	sendDataReqParam.opaqueData = counter;
	if (useZeroDataLen)
	{
		sendDataReqParam.dataLen = 0;
	}	
	
	CdSuNhUdpApiRetType rtType = cdSuNhUdpUiDataReq (sendDataReqParam);
	
	if (rtType == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Send Data Req: Successful\n");
		printf ("Data %s\n", sendDataReqParam.dataPtr);
		printf ("Data Len %d\n", sendDataReqParam.dataLen);
	} else
	{
		printf ("Send Data Req: Fail\n");
		displayApiRtType (rtType);
	}
	
	// Set All The Parameter to Default
	setDefaultValueToParameter ();


} // sendData ()

//----------------------------------------------------------------------------
// Function	: sendDataWithoutUdpPort ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void sendDataAllOpened ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	useAllOpenedUdpPort = true;
	
	sendData ();
	
	useAllOpenedUdpPort = false;
} // sendDataWithoutUdpPort ()


//----------------------------------------------------------------------------
// Function	: statTest ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void statTest ()
{
 	CdSuNhUdpPortKey upKey[ 10 ];
	char c;
	
	enum
       	{
		LOCALFIXEDST = 0,
		LOCALFIXEDEPH,
		DESTFIXED,
		BOTHLOCALDESTFIXEDST,
		BOTHLOCALDESTFIXEDEPH,
		BOTHNOTFIXED,
		ALWOPENED,
		NONEXIST	
	};
	upKey [NONEXIST].nhUdpPortId = 100;
	upKey [NONEXIST].nhUdpInstId = nhInstID;
	upKey [ALWOPENED].nhUdpInstId = nhInstID;
	upKey [ALWOPENED].nhUdpPortId = 0;
	CdSuNhUdpApiRetType rtType;
	// 1) Add Udp Port OF Type Local ONly fixed and Standard Port.
	addUdpPortReqParam.init (nhInstID, applnModId, LOCALPORT,
				cdSuNhUdpConvIp2Int (localIpAddr), 
				applEventHlr);
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, upKey [LOCALFIXEDST]);

	statCounter [CDSU_NHUDP_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_CTR]++;	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_ST_CTR]++;
	// Here It assumed that above operation always success.
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_CTR]++;	
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_ST_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]++;
	
	// 2) Add Udp Port Of Type Local Only Fixed And Ephemeral Port.
	addUdpPortReqParam.init (nhInstID, applnModId, 0,
				cdSuNhUdpConvIp2Int (localIpAddr), 
				applEventHlr);
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, upKey [LOCALFIXEDEPH]);
	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_CTR]++;	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_EPH_CTR]++;
	// Here It assumed that above operation always success.
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_CTR]++;	
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_EPH_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]++;

	// 3) Add Udp Port OF Type Destination  Only fixed.
	addUdpPortReqParam.init (nhInstID, applnModId, CDSU_NHUDP_INV_UDPPORT,
				0,applEventHlr, DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, upKey [DESTFIXED]);

	statCounter [CDSU_NHUDP_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_DST_CTR]++;	
	// Here It assumed that above operation always success.
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_DST_CTR]++;	
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]++;

	// 4) Add Udp Port Of Type Local  and Destination Both Fixed And Local Standard Port.
	addUdpPortReqParam.init (nhInstID, applnModId, EXTRALOCALPORT,
				cdSuNhUdpConvIp2Int (localIpAddr), applEventHlr,
			       	DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, upKey [BOTHLOCALDESTFIXEDST]);
	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCLDST_CTR]++;	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_ST_CTR]++;
	// Here It assumed that above operation always success.
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCLDST_CTR]++;	
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_ST_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]++;

	// 5) Add Udp Port Of Type Local  and Destination Both Fixed And Local Ephemeral Port.
	addUdpPortReqParam.init (nhInstID, applnModId, 0,
				cdSuNhUdpConvIp2Int (localIpAddr), 
				applEventHlr, DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, upKey [BOTHLOCALDESTFIXEDEPH]);
	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCLDST_CTR]++;	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_FIXED_LCL_EPH_CTR]++;
	// Here It assumed that above operation always success.
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCLDST_CTR]++;	
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_FIXED_LCL_EPH_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]++;

	// 6) Add Udp Port Of Type Local  and Destination Both Not Fixed.
	addUdpPortReqParam.init (nhInstID, applnModId, CDSU_NHUDP_INV_UDPPORT,
				0, applEventHlr, CDSU_NHUDP_INV_UDPPORT,
			       	0);
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, upKey [BOTHNOTFIXED]);
	
	statCounter [CDSU_NHUDP_ADD_UDPPORT_CTR]++;
	// Here It assumed that above operation always success.
	statCounter [CDSU_NHUDP_SUCCESS_ADD_UDPPORT_CTR]++;
	statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]++;
	printf ("Waiting for Response..............................................\n");
	sleep (2);	
	printf ("!!!!!!!!!!!!!!!!!!!!!!!!Make Sure That Remote Statitistics Test script is not running(To Continye press enter)!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	scanf ("%d", &c);
	char data [] = "Testing";
	
	// Generate Data Indication Error
	sendDataReqParam.init (upKey [BOTHLOCALDESTFIXEDST],
				applnModId, data, sizeof (data),
				0);
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_ST_CTR]++;

	statCounter [CDSU_NHUDP_DATAIND_ERROR_CTR]++;
	
	sendDataReqParam.init (upKey [BOTHLOCALDESTFIXEDEPH],
				applnModId, data, sizeof (data),
				0);
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_EPH_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_ERROR_CTR]++;

	sleep (2);
	printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Make Sure That Remote End is Running In Reply Mode!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	scanf ("%d", &c);
	
	// Send Data on each kind of UDP Port 
	// 1) Local Only Fixed and standard local Port.
	sendDataReqParam.init (upKey [LOCALFIXEDST],
				applnModId, data, sizeof (data), 0,
				true, applEventHlr, DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_ST_CTR]++;
 	// Set DOREPLY 1 in Remote So we will receive data indication
	statCounter [CDSU_NHUDP_DATAIND_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCL_ST_CTR]++;

	// 2) local only Fixed and ephemeral local port.
	sendDataReqParam.init (upKey [LOCALFIXEDEPH],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
				DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_EPH_CTR]++;
	// Data Indication (DOREPLY 1 in remote)
	statCounter [CDSU_NHUDP_DATAIND_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCL_EPH_CTR]++;
	
	// 3) both local and destination fixed and local port is standard.
	sendDataReqParam.init (upKey [BOTHLOCALDESTFIXEDST],
				applnModId, data, sizeof (data), 0, true, applEventHlr);
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_ST_CTR]++;
	// Data Indication(DOREPLAY 1 in remote)
	statCounter [CDSU_NHUDP_DATAIND_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCL_ST_CTR]++;
	
	// 4) both local and destination fixed and local port is ephemeral.
	sendDataReqParam.init (upKey [BOTHLOCALDESTFIXEDEPH],
				applnModId, data, sizeof (data), 0, true, applEventHlr);
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_LCL_EPH_CTR]++;
	// Data Indication (DOREPLAY 1 in reomte)
	statCounter [CDSU_NHUDP_DATAIND_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAIND_FIXED_LCL_EPH_CTR]++;
	
	// 5) destination only fixed.
	sendDataReqParam.init (upKey [DESTFIXED],
				applnModId, data, sizeof (data), 0, true, applEventHlr);
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_DST_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_FIXED_DST_CTR]++;
	
	// 6) both not fixed.
	sendDataReqParam.init (upKey [BOTHNOTFIXED],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
				DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_NOTFIXED_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_NOTFIXED_CTR]++;
	
	// 7) ALways Opened	
	sendDataReqParam.init (upKey [ALWOPENED], applnModId, data, sizeof (data), 0,
				true, applEventHlr, DESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_ALWOP_CTR]++;
	statCounter [CDSU_NHUDP_SUCCESS_DATAREQ_ALWOP_CTR]++;
	
	
	// Send Data on each kind of UDP Port and generate error
	// 1) Local Only Fixed and standard local Port.
	sendDataReqParam.init (upKey [LOCALFIXEDST],
				applnModId, data, sizeof (data), 0,
				true, applEventHlr, CDSU_NHUDP_INV_UDPPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR]++;
 
	// 2) local only Fixed and ephemeral local port.
	sendDataReqParam.init (upKey [LOCALFIXEDEPH],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
				CDSU_NHUDP_INV_UDPPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR]++;
	
	// 3) both local and destination fixed and local port is standard.
	sendDataReqParam.init (upKey [BOTHLOCALDESTFIXEDST],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
			       	EXTRADESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_ST_CTR]++;
	
	// 4) both local and destination fixed and local port is ephemeral.
	sendDataReqParam.init (upKey [BOTHLOCALDESTFIXEDEPH],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
				EXTRADESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr) );
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCLDST_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_LCL_EPH_CTR]++;
	
	// 5) destination only fixed.
	sendDataReqParam.init (upKey [DESTFIXED],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
				EXTRADESTPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_FIXED_DST_CTR]++;
	
	// 6) both not fixed.
	sendDataReqParam.init (upKey [BOTHNOTFIXED],
				applnModId, data, sizeof (data), 0, true, applEventHlr,
				CDSU_NHUDP_INV_UDPPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	statCounter [CDSU_NHUDP_DATAREQ_NOTFIXED_CTR]++;
	
	// 7) Non Existing Port.
	sendDataReqParam.init (upKey [NONEXIST],
				applnModId, data, sizeof (data), 0,
				true, applEventHlr, CDSU_NHUDP_INV_UDPPORT, cdSuNhUdpConvIp2Int (remoteIpAddr));
	cdSuNhUdpUiDataReq (sendDataReqParam);
	statCounter [CDSU_NHUDP_DATAREQ_CTR]++;
	
	// Removing Udp Port with error
	// 1) Non Existing Port.
	remUdpPortReqParam.init (upKey [NONEXIST],
				applnModId, applEventHlr);
	cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
	statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;

	bool failed = false;
	
	sleep (3);
	// Get The Status.
	statusReqParam.nhUdpInstId = nhInstID;
	cdSuNhUdpMiStsReq (statsReqParam);
	// Wait for some time to get stats report
	sleep (3);
	
	printf ("\n............................>Statistics Test Report<.............................\n");
	for (int i = 0; i < CDSU_NHUDP_MAX_CTR; i++)
	{
		if (statCounterOfNh [i] != statCounter [i])
		{
			failed = true;
			printf ("Counter %s Not Updated Properly(Actual = %d, Expected = %d)\n", counterString [i], statCounterOfNh [i], statCounter [i]);
		}
	}
	
	if (!failed)
	{
		// Remove Udp Port
		// 1) Remove Udp Port OF Type Local ONly fixed and Standard Port.
		remUdpPortReqParam.init (upKey [LOCALFIXEDST], applnModId, 
					applEventHlr);
		cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);

		statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_CTR]++;	
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_ST_CTR]++;
		// Here It assumed that above operation always success.
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_CTR]++;	
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_ST_CTR]++;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]--;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]--;
		
		// 2) Remove Udp Port Of Type Local Only Fixed And Ephemeral Port.
		remUdpPortReqParam.init (upKey [LOCALFIXEDEPH], applnModId, applEventHlr);
		cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
		statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_CTR]++;	
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_EPH_CTR]++;
		// Here It assumed that above operation always success.
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_CTR]++;	
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_EPH_CTR]++;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]--;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]--;

		// 3) Rem Udp Port OF Type Destination  Only fixed.
		remUdpPortReqParam.init (upKey [DESTFIXED], applnModId, applEventHlr);
		cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
		statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_DST_CTR]++;	
		// Here It assumed that above operation always success.
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_DST_CTR]++;	
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]--;

		// 4) Rem Udp Port Of Type Local  and Destination Both Fixed And Local Standard Port.
		remUdpPortReqParam.init (upKey [BOTHLOCALDESTFIXEDST], applnModId, applEventHlr);
		cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
		statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCLDST_CTR]++;	
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_ST_CTR]++;
		// Here It assumed that above operation always success.
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCLDST_CTR]++;	
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_ST_CTR]++;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]--;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]--;

		// 5) REM Udp Port Of Type Local  and Destination Both Fixed And Local Ephemeral Port.
		remUdpPortReqParam.init (upKey [BOTHLOCALDESTFIXEDEPH], applnModId, applEventHlr);
		cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
		statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCLDST_CTR]++;	
		statCounter [CDSU_NHUDP_REM_UDPPORT_FIXED_LCL_EPH_CTR]++;
		// Here It assumed that above operation always success.
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCLDST_CTR]++;	
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_FIXED_LCL_EPH_CTR]++;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]--;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_LF_CTR]--;

		// 6) Add Udp Port Of Type Local  and Destination Both Not Fixed.
		remUdpPortReqParam.init (upKey [BOTHNOTFIXED], applnModId, applEventHlr);
		cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
		statCounter [CDSU_NHUDP_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_SUCCESS_REM_UDPPORT_CTR]++;
		statCounter [CDSU_NHUDP_CUR_ACTIVE_UDPPORT_CTR]--;
		// Get The Status.
		statusReqParam.nhUdpInstId = nhInstID;
		cdSuNhUdpMiStsReq (statsReqParam);
		// Wait for some time to get stats report
		sleep (2);
		printf ("\n............................>Statistics Test Report<.............................\n");
		for (int i = 0; i < CDSU_NHUDP_MAX_CTR; i++)
		{
			if (statCounterOfNh [i] != statCounter [i])
			{
				failed = true;
				printf ("Counter %s Not Updated Properly(Actual = %d, Expected = %d)\n", counterString [i], statCounterOfNh [i], statCounter [i]);
			}
		}
	}
	if (failed == false)
	{
		printf ("Statistics Package No Error. All Counter Updated Properly\n");
	} else
	{
		printf ("Statistics Counter Not Updated Propery. Your Test Set Up May be wrong.\n");
		printf ("If you are sure that test setup is fine then there must be bugs in UDP NH\n");
	}
	scanf ("%d", &c);
	cdSuNhUdpMiStsReq (statsReqParam);

} // addUdpPort ()

void statRemoteSetUp ()
{
	addUdpPort ();
	sleep (1);
	printf ("Use remove udp port to remove this port\n");	
}

void displayName ()
{
	printf ("\ntripti\n");
}


//============================================================================
// <End of testfunctions.cpp>
//============================================================================

