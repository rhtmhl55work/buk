//=============================================================================
// Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : tester.cpp
// Description	: This is a testtub for testing Multiple NH support feature of
// 		: UDP NH.
// Author       : Vijaya Kumar
// Created On   : Tue Mar 23 16:37:50 IST 2004
// Last Modified: 
//============================================================================
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cdsunhudpapi.h"
#include "cdsumdapi.h"
#include "cdsutrace.h"
#include <semaphore.h>

int lPortOfBothFixed = 6500;
int rPortOfBothFixed = 7500;
int lPortOfLocalFixed = 8000;
int rPortOfLocalFixed = 9000;


#ifndef CLIENT
int localPortOfBothFixed = lPortOfBothFixed;
int remotePortOfBothFixed = rPortOfBothFixed;
int localPortOfLocalFixed = lPortOfLocalFixed;
int remotePortOfLocalFixed = rPortOfLocalFixed;
bool isServer = true;
char data [] [ 100 ]   =
{
	"Server->Client : BOth fixed",
	"Server->Client : Local Only Fixed To Local Only Fixed",
	"Server->Client : Destinaton Only fixed to Local Only Fixed",
	"Server->Client : Both Not fixe to local Only Fixed",
} ;
#else

int localPortOfBothFixed = rPortOfBothFixed;
int remotePortOfBothFixed = lPortOfBothFixed;
int localPortOfLocalFixed = rPortOfLocalFixed;
int remotePortOfLocalFixed = lPortOfLocalFixed;
bool isServer = false;
char data [] [ 100 ] = 
{
	"Client->Server : Data To Both Fixed",
	"Client->Server : Local Only Fixed To Local Only Fixed",
	"Client->Server : Destinaton Only fixed to Local Only Fixed",
	"Client->Server : Both Not fixe to local Only Fixed",

};
#endif

sem_t semaphore;

char localIpAddr [4] = { 192, 168, 16, 72 };
char remoteIpAddr [4] = {192, 168, 16, 25 };


void smMgmtEventHlr (void*);
void applEventHlr (void*);
void smMgmtEventHlrForMsgIntf (void*);
void applEventHlrForMsgIntf (void*);

extern void cdCreateManagerReceiverThread (CdModuleId* mId);
extern void cdCreateApplReceiverThread (CdModuleId* mId);

#ifdef __ECOS
char applThreadSpace[PTHREAD_STACK_MIN * 32];
char sysMgmtThreadSpace [PTHREAD_STACK_MIN * 32];
#endif

// Module ID of Different Module
CdSuActorId smModId;		// Sys Mgmt Actor ID.
CdSuActorId nhModId [4];		// nh MOD ID.
CdSuActorId applnModId;		// Application Actor ID.

CdSuNhUdpInstId instID [ 4 ];
CdSuNhUdpPortKey portKey [ 4 ];
char failCodeString  [] [100] =
{
	"",
	"CDSU_NHUDP_RSPFCODE_INVPARAM",
	"CDSU_NHUDP_RSPFCODE_INV_OP",
	"CDSU_NHUDP_RSPFCODE_UDPPORT_OVERFLOW",
	"CDSU_NHUDP_RSPFCODE_BINDERROR",
	"CDSU_NHUDP_RSPFCODE_CONNECTERROR",
	"CDSU_NHUDP_RSPFCODE_UPORTNOTEXIST",
	"CDSU_NHUDP_RSPFCODE_UNKNOWNERROR",
	"CDSU_NHUDP_RSPFCODE_SENDERROR",
	"CDSU_NHUDP_RSPFCODE_OPENERROR",
	"CDSU_NHUDP_RSPFCODE_READERROR",
	"CDSU_NHUDP_RSPFCODE_INVALID"
} ;

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


//----------------------------------------------------------------------------
// Function	: displayErrorType ()
// Description	: This is System Management Thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------


void displayErrorType (CdSuNhUdpRspFailCode failCode)
{
	if (CDSU_NHUDP_RSPFCODE_INVALID > failCode && failCode > -1)
	printf ("Error Type %s\n", failCodeString [failCode]);
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
		smMgmtEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpMiInitRspMsg*)cdMessage)->initRspEvent);		
		}
		break;
	
		case CDSU_NHUDP_MI_DELNH_RSP:
		{
		smMgmtEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpMiDelNhRspMsg*)cdMessage)->delNhRspEvent);	
		}
		
		break;
		case CDSU_NHUDP_MI_CTRL_RSP:
		{
		smMgmtEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpMiCtrlRspMsg*)cdMessage)->ctrlRspEvent);	
		}
		break;
		
		case CDSU_NHUDP_MI_STATUS_RSP:
		{
		smMgmtEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpMiStatusRspMsg*)cdMessage)->statusRspEvent);	
		}
		break;

		case CDSU_NHUDP_MI_STATISTICS_RSP:
		{
		smMgmtEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpMiStsRspMsg*)cdMessage)->stsRspEvent);	
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
		applEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpUiAddUdpPortRspMsg*)cdMessage)->addUdpPortRspEvent);	
		}
		break;
	
		case CDSU_NHUDP_UI_REM_UDPPORT_RSP:
		{
		applEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpUiRemUdpPortRspMsg*)cdMessage)->remUdpPortRspEvent);	
		}
		break;
		
		case CDSU_NHUDP_UI_DATA_RSP:
		{
		applEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpUiDataRspMsg*)cdMessage)->dataRspEvent);	
		}
		break;

		case CDSU_NHUDP_UI_DATA_IND:
		{
		applEventHlrForMsgIntf ( (void*) &( (CdSuNhUdpUiDataIndMsg*)cdMessage)->dataIndEvent);	
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
			//printf ("COUNTER [%s]== %d\n",  counterString [i], stsRspEvnetParam->counters [i]);
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
	delete eventParam;
	eventParam = NULL;
	sem_post (&semaphore);


} // smMgmtEvnetHlr ()

//----------------------------------------------------------------------------
// Function	: smMgmtEventHlrForMsgInf ()
// Description	: 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void smMgmtEventHlrForMsgIntf (void* arg)
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
			if (subOpcode == CDSU_NHUDP_SO_START_REQ)
			{
			printf ("Start UDP NH Failed\n");
			} else  if (subOpcode == CDSU_NHUDP_SO_STOP_REQ)
			{
			printf (" Stop UDP NH Failed\n");
			} else
			{
				printf ("Control Operation Failed\n");
			}
			displayErrorType (((CdSuNhUdpMiCtrlRspEvent*)eventParam)->failCode);

		} else 
		{
			if (subOpcode == CDSU_NHUDP_SO_START_REQ)
			{
			printf ("Start UDP NH Success\n");
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
			//printf ("COUNTER [%s]== %d\n",  counterString [i], stsRspEvnetParam->counters [i]);
			}

		} else 
		{
			printf ("Statistics Nh fail\n");
			displayErrorType (((CdSuNhUdpMiStsRspEvent*)eventParam)->failCode);

		}
		}
		break;

	} // switch
	
	sem_post (&semaphore);

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
			printf ("Add Udp Port Succes\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
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
				// delete [] ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
			}
			
			
		}
		if (((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr != NULL)
		{
			delete [] ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
		}
		}
		break;
	
	default:
		break;
	} // 	switch (eventParam->opcode)
	delete  eventParam;
	 sem_post (&semaphore);

} // applEventHlr ()

//----------------------------------------------------------------------------
// Function	: applEventHlrForMsgIntf()
// Description	: This is Application Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void applEventHlrForMsgIntf (void* arg)
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
			exit (0);
		} else 
		{
			printf ("Add Udp Port Succes\n");
			printf ("Udp Port ID = %d\n", portId);
			printf ("Udp Local Port = %d\n", localPortNo);
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
				delete [] ((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
			}
			
			
		}
		}
		break;
	
	default:
		break;
	} // 	switch (eventParam->opcode)
	 sem_post (&semaphore);

} // applEventHlr ()

void initAllGlobals ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId[0].hwUnitId.rackId = 0;
	nhModId[0].hwUnitId.unitId = 0;
	nhModId[0].hwUnitId.slotId = 0;
	nhModId[0].hwUnitId.procId = 0;

	nhModId[0].srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId[0].srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId[0].srvcHlrId.appSrvcHlrId.appId = 1;
	nhModId[0].srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.
	
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId[1].hwUnitId.rackId = 0;
	nhModId[1].hwUnitId.unitId = 0;
	nhModId[1].hwUnitId.slotId = 0;
	nhModId[1].hwUnitId.procId = 0;

	nhModId[1].srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId[1].srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId[1].srvcHlrId.appSrvcHlrId.appId = 2;
	nhModId[1].srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.

	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId[2].hwUnitId.rackId = 0;
	nhModId[2].hwUnitId.unitId = 0;
	nhModId[2].hwUnitId.slotId = 0;
	nhModId[2].hwUnitId.procId = 0;

	nhModId[2].srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId[2].srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId[2].srvcHlrId.appSrvcHlrId.appId = 3;
	nhModId[2].srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.

	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId[3].hwUnitId.rackId = 0;
	nhModId[3].hwUnitId.unitId = 0;
	nhModId[3].hwUnitId.slotId = 0;
	nhModId[3].hwUnitId.procId = 0;

	nhModId[3].srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId[3].srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId[3].srvcHlrId.appSrvcHlrId.appId = 4;
	nhModId[3].srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.


	
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
	cdSuMdRegisterMid (nhModId[0]);
	cdSuMdRegisterMid (nhModId[1]);
	cdSuMdRegisterMid (nhModId[2]);
	cdSuMdRegisterMid (nhModId[3]);
	cdSuMdRegisterMid (smModId);	
	cdSuMdRegisterMid (applnModId);	

} // initAllGlobals ()


int main ()
{
	sem_init (&semaphore, 0, 0);	
	initAllGlobals ();			
	
	CdSuNhUdpMiInitParam initParam;
	
	// Start Manager Thread to recieve response
	// to manager if callback function is not registered.
	createSMThr ();	

	//--------------------------------------------------------------------
	// 1.First Instance : Management Intf is Callback.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhModId [0];
	initParam.smActorId = smModId;
	initParam.smEventHlr = smMgmtEventHlr;
	
	// Create Instance.
	cdSuNhUdpMiInit (initParam, instID [0]);
	
	// Wait till response comes
	int rv;
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	// Start The UDP Nh
	CdSuNhUdpMiCtrlReqParam ctrlReqParam;
	ctrlReqParam.nhUdpInstId = instID [0];
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	if (cdSuNhUdpMiCtrlReq (ctrlReqParam) == CDSU_NHUDP_API_RT_SUCCESS )
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
	}
	// Wait Till Response Comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	//--------------------------------------------------------------------
	// 2. Second Instance : Management Intf is message base.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhModId [1];
	initParam.smActorId = smModId;
	initParam.smEventHlr = smMgmtEventHlr;
	
	// Create Instance.
	cdSuNhUdpMiInit (initParam, instID [1]);

	// Wati till response comes.
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	// Start NH
	ctrlReqParam.nhUdpInstId = instID [1];
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	if (cdSuNhUdpMiCtrlReq (ctrlReqParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
	}
	// Wait Till response Comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	//--------------------------------------------------------------------
	// 3. 3rd Instance : Management Intf is message Base.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhModId [2];
	initParam.smActorId = smModId;
	initParam.smEventHlr = NULL;
	
	// Create Instance.
	cdSuNhUdpMiInit (initParam, instID [2]);

	// Wait Till response Comes.
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	// Start NH
	ctrlReqParam.nhUdpInstId = instID [2];
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	if (cdSuNhUdpMiCtrlReq (ctrlReqParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
	}
	// Wait Till response Comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	//--------------------------------------------------------------------
	// 4. 4th Instance : Management Intf is message base.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhModId [3];
	initParam.smActorId = smModId;
	initParam.smEventHlr = NULL;
	
	// Create Instance.
	cdSuNhUdpMiInit (initParam, instID [3]);

	// Wait Till Response Comes.
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	// Start NH.
	ctrlReqParam.nhUdpInstId = instID [3];
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	if (cdSuNhUdpMiCtrlReq (ctrlReqParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
	}
	// Wati Till response Comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	// Create Application Thred. This is required if application
	// interface is message base.
	createApplThr ();

	CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;
	
	
	//--------------------------------------------------------------------
	// 1. Udp Port of type Both Fixed. : Callback Function Intf.
	// Instance : 1.
	//--------------------------------------------------------------------
	addUdpPortReqParam.nhUdpInstId	= instID [0];
	addUdpPortReqParam.applActorId	= applnModId;
	addUdpPortReqParam.applEventHlr = applEventHlr;
	addUdpPortReqParam.localPortNo = localPortOfBothFixed;
	addUdpPortReqParam.destPortNo = remotePortOfBothFixed;
	addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
	
	// Add Udp Port
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, 	portKey [ 0 ]);
	// Wait till response comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	

	//--------------------------------------------------------------------
	// 2. Udp Port of type Local Fixed. : Message Intf.
	// Instance : 2.
	//--------------------------------------------------------------------
	addUdpPortReqParam.nhUdpInstId	= instID [1];
	addUdpPortReqParam.applActorId	= applnModId;
	addUdpPortReqParam.applEventHlr = NULL; //applEventHlr;
	addUdpPortReqParam.localPortNo = localPortOfLocalFixed;
	addUdpPortReqParam.destPortNo = CDSU_NHUDP_INV_UDPPORT;
	
	// Add Udp Port
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, portKey [ 1 ]);
	// Wait Till Response Comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	//--------------------------------------------------------------------
	// 3. Udp Port Of Type destination fixed. : Message Intf.
	// Instance : 3.
	//--------------------------------------------------------------------
	addUdpPortReqParam.nhUdpInstId	= instID [2];
	addUdpPortReqParam.applActorId	= applnModId;
	addUdpPortReqParam.applEventHlr = NULL; //applEventHlr;
	addUdpPortReqParam.localPortNo = CDSU_NHUDP_INV_UDPPORT;
	addUdpPortReqParam.destPortNo = remotePortOfLocalFixed;
	addUdpPortReqParam.destAddr =	cdSuNhUdpConvIp2Int (remoteIpAddr);
	
	// Add Udp Port.
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, 	portKey [ 2 ]);
	// Wait Till Response Comes.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	//--------------------------------------------------------------------
	// 3. Udp Port Of Type Both Not Fixed. : Message Intf.
	// Instance 3.
	//--------------------------------------------------------------------
	addUdpPortReqParam.nhUdpInstId	= instID [3];
	addUdpPortReqParam.applActorId	= applnModId;
	addUdpPortReqParam.applEventHlr = NULL; //applEventHlr;
	addUdpPortReqParam.localPortNo = CDSU_NHUDP_INV_UDPPORT;
	addUdpPortReqParam.destPortNo = CDSU_NHUDP_INV_UDPPORT;
	
	// Add Udp Port.
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, portKey [ 3 ]);
	// Wati Till Response Comess.
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	
	char c;
	while (true)
	{
		CdSuNhUdpUiDataReqParam dataReqParam;
		printf ("Press enter to send data, 'q' to quit.\n");
		scanf ("%c", &c);
		if (c =='q') break;
		//------------------------------------------------------------
		// Send Data  
		// Response : Callback Interface.
		// Udp Port : 1(Both Fixed.)
		// Send Data To Destination Both Fixed UDP Port.
		//------------------------------------------------------------
		dataReqParam.applActorId = applnModId;
		dataReqParam.applEventHlr = applEventHlr;
		dataReqParam.udpPortKey = portKey [0];
		dataReqParam.dataPtr = data [0];
		dataReqParam.dataLen = strlen (data [0]) + 1;
		dataReqParam.opaqueData = 0;
		// Send Data.
		cdSuNhUdpUiDataReq (dataReqParam);
		// Wait Till Response Comes.
		do 
		{
			rv = sem_wait(&semaphore);
		} while (rv == -1 && errno == EINTR);
		if (rv != 0)
		{
			CDSU_TRACE (1, "semaphore wait error\n");
			return -1;
		} // if ()

		//------------------------------------------------------------
		// Send Data
		// Response : Message Interface.
		// Udp Port : 2 (Local Only Fixed.)
		// Destination : Local Only Fixed.
		//------------------------------------------------------------
		dataReqParam.applActorId = applnModId;
		dataReqParam.applEventHlr = NULL; //applEventHlr;
		dataReqParam.udpPortKey = portKey [1];
		dataReqParam.dataPtr = data [1];
		dataReqParam.dataLen = strlen (data [1]) + 1;
		dataReqParam.destPortNo = remotePortOfLocalFixed;
	        dataReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
		cdSuNhUdpUiDataReq (dataReqParam);
		do 
		{
			rv = sem_wait(&semaphore);
		} while (rv == -1 && errno == EINTR);
		if (rv != 0)
		{
			CDSU_TRACE (1, "semaphore wait error\n");
			return -1;
		} // if ()

		//------------------------------------------------------------
		// Send Data
		// Response : Message Interface.
		// Udp Port 3 (Destination Only Fixed.).
		// Destination : Local Only Fixed.
		//------------------------------------------------------------
		dataReqParam.applActorId = applnModId;
		dataReqParam.applEventHlr = NULL; //applEventHlr;
		dataReqParam.udpPortKey = portKey [2];
		dataReqParam.dataPtr = data [2];
		dataReqParam.dataLen = strlen (data [2]) + 1;
		dataReqParam.destPortNo = CDSU_NHUDP_INV_UDPPORT;
		cdSuNhUdpUiDataReq (dataReqParam);
		do 
		{
			rv = sem_wait(&semaphore);
		} while (rv == -1 && errno == EINTR);
		if (rv != 0)
		{
			CDSU_TRACE (1, "semaphore wait error\n");
			return -1;
		} // if ()

		//------------------------------------------------------------
		// Send Data.
		// Response : Message Interface.
		// Udp Port : 4 (Both Not Fixed.)
		// Destination : Local Only Fixed.
		//------------------------------------------------------------
		dataReqParam.applActorId = applnModId;
		dataReqParam.applEventHlr = NULL; //applEventHlr;
		dataReqParam.udpPortKey = portKey [3];
		dataReqParam.dataPtr = data [3];
		dataReqParam.dataLen = strlen (data [3]) + 1;
		dataReqParam.destPortNo = remotePortOfLocalFixed;
		dataReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);
		cdSuNhUdpUiDataReq (dataReqParam);
		do 
		{
			rv = sem_wait(&semaphore);
		} while (rv == -1 && errno == EINTR);
		if (rv != 0)
		{
			CDSU_TRACE (1, "semaphore wait error\n");
			return -1;
		} // if ()
	} 

	CdSuNhUdpUiRemUdpPortReqParam remUdpPortReqParam;
	

	//--------------------------------------------------------------------
	// Remove Udp Port 1.
	// Response : Message.
	//--------------------------------------------------------------------
	remUdpPortReqParam.udpPortKey = portKey [0];
	remUdpPortReqParam.applActorId = applnModId;
	remUdpPortReqParam.applEventHlr = NULL; //applEventHlr;
	cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
	do 
	{
		rv = sem_wait(&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	//--------------------------------------------------------------------
	// Remove Udp Port 2.
	// Response : Callback.
	//--------------------------------------------------------------------
	remUdpPortReqParam.udpPortKey = portKey [1];
	remUdpPortReqParam.applEventHlr = applEventHlr;
	cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
	do 
	{
		rv = sem_wait (&semaphore);	
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	
	//--------------------------------------------------------------------
	// Remove Udp Port 3.
	// Response : Callback.
	//--------------------------------------------------------------------
	remUdpPortReqParam.udpPortKey = portKey [2];
	remUdpPortReqParam.applEventHlr = applEventHlr;
	cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
	do 
	{
		rv = sem_wait (&semaphore);	
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	//--------------------------------------------------------------------
	// Remove Udp Port 3.
	// Response : Callback.
	//--------------------------------------------------------------------
	remUdpPortReqParam.udpPortKey = portKey [3];
	remUdpPortReqParam.applEventHlr = applEventHlr;
	cdSuNhUdpUiRemUdpPortReq (remUdpPortReqParam);
	do 
	{
		rv = sem_wait (&semaphore);	
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	
	CdSuNhUdpMiTerminateParam terminateParam;
	CdSuNhUdpMiDelNhParam delNhReqParam;
	
	//--------------------------------------------------------------------
	// Terminate UDP Nh
	// Instance : 1
	//--------------------------------------------------------------------
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_STOP_REQ;
	ctrlReqParam.nhUdpInstId = instID [0];
	
	cdSuNhUdpMiCtrlReq (ctrlReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()

	
	delNhReqParam.nhUdpInstId = instID [0];
	
	cdSuNhUdpMiDelNhReq (delNhReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	terminateParam.nhUdpInstId = instID [0];
	if (cdSuNhUdpMiTerminate (terminateParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("NH Instance ID %d :Success fully terminated\n", instID [0]);
	}
		
	
	//--------------------------------------------------------------------
	// Terminate UDP Nh
	// Instance : 2
	//--------------------------------------------------------------------
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_STOP_REQ;
	ctrlReqParam.nhUdpInstId = instID [1];
	
	cdSuNhUdpMiCtrlReq (ctrlReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	delNhReqParam.nhUdpInstId = instID [1];
	
	cdSuNhUdpMiDelNhReq (delNhReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	terminateParam.nhUdpInstId = instID [1];
	if (cdSuNhUdpMiTerminate (terminateParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("NH Instance ID %d :Success fully terminated\n", instID [1]);
	}

	//--------------------------------------------------------------------
	// Terminate UDP Nh
	// Instance : 3
	//--------------------------------------------------------------------
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_STOP_REQ;
	ctrlReqParam.nhUdpInstId = instID [2];
	
	cdSuNhUdpMiCtrlReq (ctrlReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	delNhReqParam.nhUdpInstId = instID [2];
	
	cdSuNhUdpMiDelNhReq (delNhReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	terminateParam.nhUdpInstId = instID [2];
	if (cdSuNhUdpMiTerminate (terminateParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("NH Instance ID %d :Success fully terminated\n", instID [2]);
	}
	//--------------------------------------------------------------------
	// Terminate UDP Nh
	// Instance : 4
	//--------------------------------------------------------------------
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_STOP_REQ;
	ctrlReqParam.nhUdpInstId = instID [3];
	
	cdSuNhUdpMiCtrlReq (ctrlReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	delNhReqParam.nhUdpInstId = instID [3];
	
	cdSuNhUdpMiDelNhReq (delNhReqParam);
	do 
	{
		rv = sem_wait (&semaphore);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return -1;
	} // if ()
	
	
	terminateParam.nhUdpInstId = instID [3];
	if (cdSuNhUdpMiTerminate (terminateParam) == CDSU_NHUDP_API_RT_SUCCESS)
	{
		printf ("NH Instance ID %d :Success fully terminated\n", instID [3]);
	}
}

