//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testfunctions.cpp
// Description  : Test Functions
// Author       : Sumit Bijwe
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cdsutrace.h"
#include "testfunctions.h"
#include "cdsunhtcpapi.h"
#include "cdsumdapi.h"
#include "parameter.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


Uint32 changeIpaddr (char ipaddr []);
void lstEventHlr (void* arg);
void conEventHlr (void* arg);
void utilSmEventHlr (void* arg);

#ifdef __ECOS
char nhtcpApplnThreadSpace[PTHREAD_STACK_MIN * 32];
char sysMgmtThreadSpace [PTHREAD_STACK_MIN * 32];
#endif


#define MAX_NO_CONN 500 

// TODO Remove
int noConnInd;
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

// TODO: Add test functions here, format void funcName (void)
void displayErrorType (CdSuNhTcpFailCode failCode)
{
	if (CDSU_NHTCP_FCODE_INVALID > failCode && failCode > -1)
	CDSU_TRACE1 (1, "Error Type %s\n", failCodeString [failCode]);
}

void displayApiRtType (CdSuNhTcpApiRetType apiRtType)
{
	CDSU_TRACE1 (1, "API Return Type %s\n", apiRtTypeString [apiRtType]);
}


//----------------------------------------------------------------------------
// Function	: createULThr ()
// Description	: It creates User Layer (application) thread.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void createULThr ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	initAllGlobals ();
	//----------------------------------------------------------------------------
	// Creating the application thread.
	//----------------------------------------------------------------------------
	pthread_t nhtcpApplnThrd;
#ifdef __ECOS
	pthread_attr_t nhtcpApplnThrdAttr;
	pthread_attr_init (&nhtcpApplnThrdAttr);
	pthread_attr_setstackaddr (&nhtcpApplnThrdAttr,
	(void*)&nhtcpApplnThreadSpace [sizeof (nhtcpApplnThreadSpace)]);
	pthread_attr_setstacksize (&nhtcpApplnThrdAttr, sizeof (nhtcpApplnThreadSpace));
	
	pthread_create (&nhtcpApplnThrd, &nhtcpApplnThrdAttr, applThrdStart, 0);

#elif sun

	pthread_attr_t nhtcpApplnThrdAttr;
	pthread_attr_init (&nhtcpApplnThrdAttr);
	pthread_attr_setscope (&nhtcpApplnThrdAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&nhtcpApplnThrd, &nhtcpApplnThrdAttr, applThrdStart, 0);
#else
	
	pthread_create (&nhtcpApplnThrd, 0, applThrdStart, 0);
	
#endif

} // createULThr ()

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
// Function	: createTcpNh ()
// Description	: It creates TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool createTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	if (flag)
	{
		initParam.nhActorId = (CdSuActorId) nhModId;
		initParam.smActorId = (CdSuActorId) smModId;
		initParam.smEventHlr = utilSmEventHlr;
		initParam.maxNoOfConnections = 2000;	// Max no of connections.
		initParam.maxNoOfLstns =  1000; 	// Max no of listeners.
		initParam.maxBufferLen = 2048;		// Max buffer len.
		initParam.listenSelectTimer = 250;	// Listener connection time.
		initParam.dataSelectTimer = 250;	// Data select time.
		initParam.noHandleTimer = 250;		// No handler timer.
		initParam.listenQSize =	5;		// Listen queue size.
	} else
	{
		// Right now no invalid parameter.
		initParam.nhActorId = (CdSuActorId) nhModId;
		initParam.smActorId = (CdSuActorId) smModId;
		initParam.smEventHlr = utilSmEventHlr;
		initParam.maxNoOfConnections = 2000;	// Max no of connections.
		initParam.maxNoOfLstns =  1000; 	// Max no of listeners.
		initParam.maxBufferLen = 2048;		// Max buffer len.
		initParam.listenSelectTimer = 250;	// Listener connection time.
		initParam.dataSelectTimer = 250;	// Data select time.
		initParam.noHandleTimer = 250;		// No handler timer.
		initParam.listenQSize =	5;		// Listen queue size.

	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpMiInit (initParam, nhInstId);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (4, "Init TCP NH API call :: CDSU_NHTCP_API_RT_SUCCESS.\n");
		return (true);
	} else
	{
		CDSU_TRACE (4, "Init TCP NH API call :: ERROR.\n");
		return (false);
	}

} // createTcpNh ()

//----------------------------------------------------------------------------
// Function	: configTcpNh ()
// Description	: It configures TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool configTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	if (flag)
	{
		cfgReqParam.nhTcpInstId = nhInstId;
	} else
	{
		cfgReqParam.nhTcpInstId = INVALNHINSTID;

	}
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCfgReq (cfgReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Config NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
		return (true);
	} else
	{
		CDSU_TRACE (5, "Config NH API call :: ERROR.\n");
		return (false);
	}

} // configTcpNh ()

//----------------------------------------------------------------------------
// Function	: startTcpNh ()
// Description	: It starts TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool startTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	if (flag)
	{
		ctrlReqParam.nhTcpInstId = nhInstId;
		ctrlReqParam.subOpcode = CDSU_NHTCP_SO_START_REQ;
	} else
	{
		ctrlReqParam.nhTcpInstId = INVALNHINSTID;
		ctrlReqParam.subOpcode = CDSU_NHTCP_SO_START_REQ;

	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCtrlReq (ctrlReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Start NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
		return (true);
	} else
	{
		CDSU_TRACE (5, "Start NH API call :: ERROR.\n");
		return (false);
	}

} // startTcpNh ()

//----------------------------------------------------------------------------
// Function	: stopTcpNh ()
// Description	: It stops TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool stopTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	if (flag)
	{
		ctrlReqParam.nhTcpInstId = nhInstId;
		ctrlReqParam.subOpcode = CDSU_NHTCP_SO_STOP_REQ;
	} else
	{
		ctrlReqParam.nhTcpInstId = INVALNHINSTID;
		ctrlReqParam.subOpcode = CDSU_NHTCP_SO_STOP_REQ;

	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCtrlReq (ctrlReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Stop NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
		return (true);
	} else
	{
		CDSU_TRACE (5, "Stop NH API call :: ERROR.\n");
		return (false);
	}

} // stopTcpNh ()


//----------------------------------------------------------------------------
// Function	: deleteTcpNh ()
// Description	: It deletes TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool deleteTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	if (flag)
	{
		delnhparam.init (nhInstId);
	}
	else 
	{
		delnhparam.init (INVALNHINSTID);
	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpMiDelNhReq(delnhparam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Delete NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
		return (true);

	} else
	{
		CDSU_TRACE (5, "Delete NH API call :: ERROR.\n");
		return (false);
	}

} // deleteTcpNh ()

//----------------------------------------------------------------------------
// Function	: terminateTcpNh ()
// Description	: It terminates TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool terminateTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	if (flag)
	{
		termparam.init (nhInstId);
	} else 
	{
		termparam.init (INVALNHINSTID);
	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpMiTerminate(termparam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Terminate NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
		return (true);
	} else
	{
		CDSU_TRACE (5, "Terminate NH API call :: ERROR.\n");
		return (false);
	}

} // terminateTcpNh ()

//----------------------------------------------------------------------------
// Function	: statusReqTcpNh ()
// Description	: It requests for status of TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool statusReqTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	if (flag)
	{
		statusReqParam.init (nhInstId);
	}
	else 
	{
		statusReqParam.init (INVALNHINSTID);
	}


	CdSuNhTcpApiRetType retType = cdSuNhTcpMiStatusReq (statusReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Status NH API call :: CDSU_NHTCP_API_RT_SUCCESS\n");
		return (true);
	} else
	{
		CDSU_TRACE (5, "Status NH API call :: ERROR.\n");
		return (false);
	}

} // statusReqTcpNh ()

//----------------------------------------------------------------------------
// Function	: statsReqTcpNh ()
// Description	: It requests for statistics of TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool statsReqTcpNh (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	if (flag)
	{
		statsReqParam.init (nhInstId);
	}
	else 
	{
		statsReqParam.init (INVALNHINSTID);
	}


	CdSuNhTcpApiRetType retType = cdSuNhTcpMiStsReq (statsReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Statistics NH API call :: CDSU_NHTCP_API_RT_SUCCESS\n");
		return (true);
	} else
	{
		CDSU_TRACE (5, "Statistics NH API call :: ERROR.\n");
		return (false);
	}

} // statsReqTcpNh ()

//----------------------------------------------------------------------------
// Function	: applThrdStart ()
// Description	: It is application thread start function. 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void* applThrdStart (void*)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

} // applThrdStart ()

//----------------------------------------------------------------------------
// Function	: sysMgmtStart ()
// Description	: It is system management thread start function. 
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void* sysMgmtStart (void*)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	CdMessage* rcvMsg;
	if (cdSuMdRecvMsg (&rcvMsg, smModId) != false)
	{
		switch (rcvMsg->msgHdr.opcode)
		{
		case CDSU_NHTCP_MI_CTRL_RSP:
			{
			CdSuNhTcpMiCtrlRspMsg* rspMsg = (CdSuNhTcpMiCtrlRspMsg*) rcvMsg;
			CDSU_FREEMSGBUF (rspMsg);
			}
			break;

		default:
			delete rcvMsg;
			break;

		} // switch.
	
	} // if


} // sysMgmtStart ()

//----------------------------------------------------------------------------
// Function	: initAllGlobals()
// Description	: It initialize all globals.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void initAllGlobals ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Network handler Module ID).
	nhModId.hwUnitId.rackId = 0;
	nhModId.hwUnitId.unitId = 0;
	nhModId.hwUnitId.slotId = 1;
	nhModId.hwUnitId.procId = 0;

	nhModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	nhModId.srvcHlrId.appSrvcHlrId.appType = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appId = 1;
	nhModId.srvcHlrId.appSrvcHlrId.appModuleId = 1;	// To be changed later.
	
	// Note: SM Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (System Management Module ID).
	smModId.hwUnitId.rackId = 0;
	smModId.hwUnitId.unitId = 0;
	smModId.hwUnitId.slotId = 1;
	smModId.hwUnitId.procId = 0;

	smModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	smModId.srvcHlrId.appSrvcHlrId.appType = 2;
	smModId.srvcHlrId.appSrvcHlrId.appId = 2;
	smModId.srvcHlrId.appSrvcHlrId.appModuleId = 2;	// To be changed later.

	// Note: Appl Hardware Unit Id should be same as given in testserver.
	// Allocate ActorIds (Appl Module ID).
	applnModId.hwUnitId.rackId = 0;
	applnModId.hwUnitId.unitId = 0;
	applnModId.hwUnitId.slotId = 1;
	applnModId.hwUnitId.procId = 0;

	applnModId.srvcHlrId.srvcType = SM_SERVICE_TYPE_APPLICATION_SERVICE;
	applnModId.srvcHlrId.appSrvcHlrId.appType = 2;
	applnModId.srvcHlrId.appSrvcHlrId.appId = 2;
	applnModId.srvcHlrId.appSrvcHlrId.appModuleId = 3;	// To be changed later.


	cdSuMdRegisterMid (nhModId);	
	cdSuMdRegisterMid (smModId);	
	cdSuMdRegisterMid (applnModId);	

} // initAllGlobals ()

//----------------------------------------------------------------------------
// Function    : convIp2Int
// Description : Change ip address from char [] to Uint32 
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
// Function	: utilSmEventHlr ()
// Description	: This is System Management Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void utilSmEventHlr (void* arg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpMiEvent* evt = (CdSuNhTcpMiEvent*) arg;
	switch (evt->opcode)
	{
	case CDSU_NHTCP_MI_INIT_RSP:
		{
		CdSuNhTcpRspCode rspCode = ((CdSuNhTcpMiInitRspEvent*)evt)->rspCode;
		if (rspCode != CDSU_NHTCP_RSPCODE_FAIL)
		{
			CDSU_TRACE (1, "TCP NH Init Successful.\n");
		} else
		{
			CDSU_TRACE (1, "TCP NH Init Fail.\n");
			displayErrorType (((CdSuNhTcpMiInitRspEvent*)evt)->failCode);
		}
		// Delete event.
		delete (CdSuNhTcpMiInitRspEvent*) evt;
		evt = NULL;

		}
		break;

	case CDSU_NHTCP_MI_DELNH_RSP:
		{
		CdSuNhTcpRspCode rspCode = ((CdSuNhTcpMiDelNhRspEvent*)evt)->rspCode;
		if (rspCode != CDSU_NHTCP_RSPCODE_FAIL)
		{
			CDSU_TRACE (1, "DEL TCP NH Successful.\n");
		} else
		{
			CDSU_TRACE (1, "DEL TCP NH Fail.\n");
			displayErrorType (((CdSuNhTcpMiDelNhRspEvent*)evt)->failCode);
		}
		// Delete event.
		delete (CdSuNhTcpMiDelNhRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_CTRL_RSP:
		{
		CdSuNhTcpRspCode rspCode = ((CdSuNhTcpMiCtrlRspEvent*)evt)->rspCode;
		CdSuNhTcpSubOpcode subOpcode = ((CdSuNhTcpMiCtrlRspEvent*)evt)->subOpcode;

		CDSU_TRACE (1, "Control Response received.\n");
		if (rspCode != CDSU_NHTCP_RSPCODE_FAIL)
		{
			CDSU_TRACE (1, "Control Request for TCP NH Successful.\n");
			if (subOpcode == CDSU_NHTCP_SO_START_RSP)
			{
				CDSU_TRACE (1 ,"Start TCP NH Successful.\n");
			} else  if (subOpcode == CDSU_NHTCP_SO_STOP_RSP)
			{
				CDSU_TRACE (1, " Stop TCP NH Successful.\n");
			}
		} else
		{
			if (subOpcode == CDSU_NHTCP_SO_START_RSP)
			{
				CDSU_TRACE (1 ,"Start TCP NH Fail.\n");
			} else  if (subOpcode == CDSU_NHTCP_SO_STOP_RSP)
			{
				CDSU_TRACE (1, " Stop TCP NH Fail.\n");
			} else
			{
				CDSU_TRACE (1, "Control Request for TCP NH Fail.\n");
			}
			displayErrorType (((CdSuNhTcpMiCtrlRspEvent*)evt)->failCode);
		}
		// Delete Event.
		delete (CdSuNhTcpMiCtrlRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_CFG_RSP:
		{
		CDSU_TRACE (1, "Config Response received\n");
		delete (CdSuNhTcpMiCfgRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_STATUS_RSP:
		{
		CdSuNhTcpRspCode rspCode = ((CdSuNhTcpMiStatusRspEvent*)evt)->rspCode;
		CdSuNhTcpState nhState = ((CdSuNhTcpMiStatusRspEvent*)evt)->nhState;
		if (rspCode != CDSU_NHTCP_RSPCODE_FAIL)
		{
			CDSU_TRACE (1, "Status Nh Success\n");
			if (nhState == CDSU_NHTCP_ST_CREATED)
			{
				CDSU_TRACE (4, "State Of NH = CREATED\n");

			} else if (nhState == CDSU_NHTCP_ST_RUNNING)
			{
				CDSU_TRACE (4, "State Of NH = RUNNING\n");
			}
		} else 
		{
			CDSU_TRACE (1, "Status Nh Fail.\n");
			displayErrorType (((CdSuNhTcpMiStatusRspEvent*)evt)->failCode);

		}
		delete (CdSuNhTcpMiStatusRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_STATISTICS_RSP:
		{
		CdSuNhTcpMiStsRspEvent* stsRspEvnetParam = 
					((CdSuNhTcpMiStsRspEvent*)evt);
		CdSuNhTcpRspCode rspCode = stsRspEvnetParam->rspCode;
		
		if (rspCode != CDSU_NHTCP_RSPCODE_FAIL)
		{

			CDSU_TRACE (1, "Statistics Response Successful.\n");
			if (stsRspEvnetParam->counters != NULL)
			for (int i = 0 ; i < CDSU_NHTCP_MAX_CTR; ++i)
			{
				CDSU_TRACE2 (1, "COUNTER [%s]== %d\n",  counterString [i], stsRspEvnetParam->counters [i]);
			}

		} else 
		{
			CDSU_TRACE (1, "Statistics Response Fail.\n");
			displayErrorType (((CdSuNhTcpMiStsRspEvent*)evt)->failCode);
		}

		delete (CdSuNhTcpMiStsRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_MI_USTATUS_IND:
		{
		CDSU_TRACE (1, "Unsolicited Status Indication received\n");
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


	
} // utilSmEventHlr ()


//----------------------------------------------------------------------------
// Function	: lstEventHlr()
// Description	: This is Listener Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void lstEventHlr (void* arg)
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
			CDSU_TRACE (5, "Add Listener failed.");
			printf ( "TCP/MI/RUN/ADD_LISTNER (V)		: Fail\n");
		} else 
		{
			CDSU_TRACE (5, "Add Listener succesful.");
			printf ( "TCP/MI/RUN/ADD_LISTNER (V)		: Pass\n");
		}
		//Delete event.
		delete (CdSuNhTcpUiAddLstnRspEvent*) evt;
		evt = NULL;

		}
		break;

	case CDSU_NHTCP_UI_CON_IND:
		{
		CDSU_TRACE (5, "New Connection on Listener.");

		CdSuNhTcpUiConIndEvent* conIndEvent =
					(CdSuNhTcpUiConIndEvent*) evt;
		// printf ("%d\n", noConnInd);
		noConnInd++;
		// Print Remote Ipaddr and Port No.
		CDSU_TRACE1 (1, "Port No from where connection has arrived: %d\n",
					conIndEvent->dstPortNo);	

		unsigned char ipaddr [4];
		memcpy ((void*) ipaddr, (void*) &conIndEvent->dstIpv4Addr, 4);
		CDSU_TRACE (1, "Ipaddr is :\n");
		for (int i = 0; i < 4; i++)
		{
			CDSU_TRACE1 (1, "%d\n", ipaddr [i]);
		}
		// Extract Connection Key.
		connKey [0] = conIndEvent->connKey;
		
		printf ( "TCP/MI/RUN/ACCEPT_CONN (V)		: Pass\n");
		// Delete event.
		delete (CdSuNhTcpUiConIndEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_REM_LISTENER_RSP:
		{
		if (((CdSuNhTcpUiRemLstnRspEvent*)evt)->rspCode != CDSU_NHTCP_RSPCODE_SUCCESS)
		{
			CDSU_TRACE (5, "Remove Listener failed.");
			printf ( "TCP/MI/RUN/DEL_LISTNER (V)		: Fail\n");
		} else 
		{
			CDSU_TRACE (5, "Add Listener succesful.");
			printf ( "TCP/MI/RUN/DEL_LISTNER (V)		: Pass\n");
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


	
} // lstEventHlr ()

//----------------------------------------------------------------------------
// Function	: conEventHlr ()
// Description	: This is Connection Event Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void conEventHlr (void* arg)
{	
	CdSuNhTcpUiConnEvent* evt = (CdSuNhTcpUiConnEvent*) arg;

	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	switch (evt->opcode)
	{
	case CDSU_NHTCP_UI_CONNECT_RSP:
		{	
		if (((CdSuNhTcpUiConRspEvent*)evt)->rspCode != CDSU_NHTCP_RSPCODE_SUCCESS)
		{
			CDSU_TRACE (5, "Open Connection failed.");
			printf ( "TCP/MI/RUN/CONN_REMT (V)		: Fail\n");
		} else 
		{
			CDSU_TRACE (5, "Open Connection succesful.");
			printf ( "TCP/MI/RUN/CONN_REMT (V)		: Pass\n");
			noConnInd++;
			// printf ("%d \n", noConnInd);


		}
		// Delete event.
		delete (CdSuNhTcpUiConRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_DATA_RSP:	
		{
		printf ( "TCP/MI/RUN/SEND_DATA_TPKT (V)		: Pass\n");
		}
		break;

	case CDSU_NHTCP_UI_DATA_IND:
		{
		CDSU_TRACE (5, "Data Indication.");
		CdSuNhTcpUiDataIndEvent* dataIndEvent = 
					(CdSuNhTcpUiDataIndEvent*) evt;
		CDSU_TRACE (5, "Data Indication.");
		int len = dataIndEvent->dataLen;
		CDSU_TRACE1 (1, "Length of data is %d\n", len);
		char data [len];
		strncpy (data, dataIndEvent->dataPtr, len);
		CDSU_TRACE1 (1, "Data Received: %s\n", dataIndEvent->dataPtr);

		printf ( "TCP/MI/RUN/RECV_DATA (V)		: Pass\n");
		// Delete event.
		delete (CdSuNhTcpUiDataIndEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_IND:
		{
		CDSU_TRACE (1, "Close Connection Indication received\n");
		printf ( "TCP/MI/RUN/RM_DISCONNECT (V)		: Pass\n");
		// Delete event.
		delete (CdSuNhTcpUiCloseConRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_RSP:	
		{
		printf ( "TCP/MI/RUN/UL_DISCONNECT (V)		: Pass\n");
		}
		break;
	default:
		{
		delete evt;
		evt = NULL;
		}
		break;
	} // switch
	
} // conEventHlr ()

//----------------------------------------------------------------------------
// Function	: addListener ()
// Description	: It adds tcp listener.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool addListener (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	noConnInd = 1;
	tpkt = false;

	char* lsip =  selfIpAddr;
	CdSuIpv4Addr lsipv4 = convIp2Int (lsip); 
	if (flag)
	{
		listenReqParam.init (nhInstId,
				     applnModId,
				     lstEventHlr,
				     conEventHlr,
				     lsipv4,
				     9999,
				     tpkt);
	} else
	{
		listenReqParam.init (INVALNHINSTID,
				     applnModId,
				     lstEventHlr,
				     conEventHlr,
				     lsipv4,
				     9999,
				     tpkt);

	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpUiAddLstnReq (listenReqParam, lstnKey);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Add Listner API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
		return (true);
	} else
	{

		CDSU_TRACE (5, "Add Listner API call :: ERROR\n" );
		return (false);
	}

} // addListener ()

//----------------------------------------------------------------------------
// Function	: connectReq ()
// Description	: It establishes the connection to remote end.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool connectReq (bool flag)

{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	noConnInd = 1;
	char* dstip =  remoteIpAddr;
	CdSuIpv4Addr dstipv4 = convIp2Int (dstip);
	if (flag)
	{
		conReqParam.init (nhInstId,
			  applnModId,
			  conEventHlr,
			  dstipv4,
			  9999,
			  tpkt);
	} else
	{
		conReqParam.init (INVALNHINSTID,
			  applnModId,
			  conEventHlr,
			  dstipv4,
			  9999,
			  tpkt);


	}

	CdSuNhTcpApiRetType retType = cdSuNhTcpUiConReq (conReqParam, connKey [0]);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Connect Req API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
		return (true);
	} else
	{

		CDSU_TRACE (5, "Connect Req API call :: ERROR\n" );
		return (false);
	}

} // connectReq ()


//----------------------------------------------------------------------------
// Function	: connectReqLoadTesting ()
// Description	: It establishes several connections to the Remote. It tests
// 		  the system under load.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void connectReqLoadTesting ()

{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	noConnInd = 1;
	char* dstip = remoteIpAddr;
	CdSuIpv4Addr dstipv4 = convIp2Int (dstip);
	conReqParam.init (nhInstId,
				  applnModId,
				  conEventHlr,
				  dstipv4,
				  9999,
				  tpkt);

	for (int i= 1; i <= MAX_NO_CONN; i++)
	{
		CdSuNhTcpApiRetType retType = cdSuNhTcpUiConReq (conReqParam, connKey [i]);
		if (retType == CDSU_NHTCP_API_RT_SUCCESS)
		{
			// CDSU_TRACE (5, "Connect Req API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
		}

		if (i % 50 == 0)
		{
			sleep (3);
		}

	}

} // connectReq ()

//----------------------------------------------------------------------------
// Function	: sendData ()
// Description	: It sends data on single connections.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool sendData (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	char* dstip =  remoteIpAddr;
	char* data = "TEST MSG";
	int len = strlen (data);

	if (flag)
	{
		dataReqParam.init (connKey [0], applnModId, conEventHlr, data, len);
	} else
	{
		CdSuNhTcpConnKey invConKey;
		invConKey.init (CDSU_NHTCP_INV_CONNID, INVALNHINSTID);
		dataReqParam.init (invConKey, applnModId, conEventHlr, data, len);
	}
	// for (int i=0 ; i < 500; i++)
	CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Send Data API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
		return (true);
	} else
	{
		CDSU_TRACE (5, "Send Data API call :: ERROR\n" );
		return (false);
	}

} // void sendData ()


//----------------------------------------------------------------------------
// Function	: sendDataLoadTesting ()
// Description	: It sends data on several connections.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void sendDataLoadTesting ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	char* dstip = remoteIpAddr;
	char* data = "TEST MSG";
	int len = strlen (data);

	for (int i=1; i <= MAX_NO_CONN; i++)
	{
		dataReqParam.init (connKey[i], applnModId, conEventHlr, data, len);
		CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
	}

} // sendDataLoadTesting ()

//----------------------------------------------------------------------------
// Function	: remListener ()
// Description	: It removes tcp listener.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool remListener (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	if (flag)
	{
		rmlstnparam.init (lstnKey,
			     applnModId,
			     lstEventHlr);
	} else
	{
		CdSuNhTcpLstnKey invKey;
		invKey.init(CDSU_NHTCP_INV_LSTNID, INVALNHINSTID);
		rmlstnparam.init (invKey,
			     applnModId,
			     lstEventHlr);
	}

	 CdSuNhTcpApiRetType retType = cdSuNhTcpUiRemLstnReq (rmlstnparam);
	 if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	 {
		CDSU_TRACE (5, "Remove Listener API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
		return (true);	
	 } else
	 {
		CDSU_TRACE (5, "Remove Listner API call :: ERROR\n" );
		return (false);	
	 }

} // remListener ()

//----------------------------------------------------------------------------
// Function	: closeConReq ()
// Description	: It closes a connection.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

bool closeConReq (bool flag)
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	if (flag)	
	{
		clConReqParam.init (connKey [0], applnModId, CDSU_NHTCP_CCR_NORMAL);	
	} else
	{
		CdSuNhTcpConnKey invKey;
		invKey.init(CDSU_NHTCP_INV_LSTNID, INVALNHINSTID);
		clConReqParam.init (invKey, applnModId, CDSU_NHTCP_CCR_NORMAL);	
	}

	clConReqParam.conEventHlr = conEventHlr;
	CdSuNhTcpApiRetType retType = cdSuNhTcpUiCloseConReq (clConReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Close Connection API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
		return (true);
	} else
	{

		CDSU_TRACE (5, "Close Connection API call :: ERROR\n" );
		return (false);
	}

} // closeConReq ()


//----------------------------------------------------------------------------
// Function	: closeConReqLoadTesting ()
// Description	: It closes many connections.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void closeConReqLoadTesting ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	for (int i = 0; i < MAX_NO_CONN; i++)
	{
		clConReqParam.init (connKey[i], applnModId, CDSU_NHTCP_CCR_NORMAL);	
		clConReqParam.conEventHlr = conEventHlr;
		cdSuNhTcpUiCloseConReq (clConReqParam);
	}

} // closeConReqLoadTesting ()

//----------------------------------------------------------------------------
// Function	: initNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void initNhValid ()
{
	printf ( "----------------------------------------------------------\n");
	printf ( "                       MI Test Cases Report \n");
	printf ( "----------------------------------------------------------\n");

	bool retval = createTcpNh (true);
	if (retval != false)
		printf ("TCP/MI/IDLE/INIT_NH (V)		: Pass\n");
	else
		printf ("TCP/MI/IDLE/INIT_NH (V)		: Fail\n");
}

//----------------------------------------------------------------------------
// Function	: initNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void initNhInValid ()
{
	bool retval = createTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/INIT_NH (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/INIT_NH (I)		: Fail\n");
}

//----------------------------------------------------------------------------
// Function	: genCfgNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void genCfgNhValid ()
{
	// Pre Test Case
	createTcpNh (true);

	// Actual Test Case
	bool retval = configTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/GEN_CFG_NH (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/GEN_CFG_NH (V)		: Fail\n");


	// Post Test Case
	deleteTcpNh (true);
	terminateTcpNh (true);
}

//----------------------------------------------------------------------------
// Function	: genCfgNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void genCfgNhInValid ()
{	
	// Pre Test Case
	createTcpNh (true);

	// Actual Test Case
	bool retval = configTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/GEN_CFG_NH (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/GEN_CFG_NH (I)		: Fail\n");


	// Post Test Case
	deleteTcpNh (true);
	terminateTcpNh (true);

}

//----------------------------------------------------------------------------
// Function	: startNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void startNhValid ()
{
	// Pre Test Case
	createTcpNh (true);

	// Actual Test Case
	bool retval = startTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/STRT_NH (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STRT_NH (V)		: Fail\n");


	// Post Test Case
	stopTcpNh (true);
	deleteTcpNh (true);
	terminateTcpNh (true);


}

//----------------------------------------------------------------------------
// Function	: startNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void startNhInValid ()
{
	// Pre Test Case
	createTcpNh (true);

	// Actual Test Case
	bool retval = startTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/STRT_NH (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STRT_NH (I)		: Fail\n");


	// Post Test Case
	stopTcpNh (true);
	deleteTcpNh (true);
	terminateTcpNh (true);

}

//----------------------------------------------------------------------------
// Function	: stopNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void stopNhValid ()
{
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);

	// Actual Test Case
	bool retval = stopTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/STOP_NH (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STRT_NH (V)		: Fail\n");


	// Post Test Case
	deleteTcpNh (true);
	terminateTcpNh (true);


}

//----------------------------------------------------------------------------
// Function	: stopNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void stopNhInValid ()
{
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);

	// Actual Test Case
	bool retval = stopTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/STOP_NH (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STRT_NH (I)		: Fail\n");

	
	// Post Test Case
	stopTcpNh (true);
	deleteTcpNh (true);
	terminateTcpNh (true);


}

//----------------------------------------------------------------------------
// Function	: deleteNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void deleteNhValid ()
{
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);
	stopTcpNh (true);

	// Actual Test Case
	bool retval = deleteTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/DEL_NH (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/DEL_NH (V)		: Fail\n");

	// Post Test Case
	terminateTcpNh (true);


}

//----------------------------------------------------------------------------
// Function	: deleteNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void deleteNhInValid ()
{
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);
	stopTcpNh (true);

	// Actual Test Case
	// Actual Test Case
	bool retval = deleteTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/DEL_NH (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/DEL_NH (I)		: Fail\n");


	// Post Test Case
	deleteTcpNh (true);
	terminateTcpNh (true);
}

//----------------------------------------------------------------------------
// Function	: terminateNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void terminateNhValid ()
{
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);
	stopTcpNh (true);
	sleep (2);
	deleteTcpNh (true);

	// Actual Test Case
	bool retval = terminateTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/TERMINATE_NH (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/TERMINATE_NH (V)		: Fail\n");

}

//----------------------------------------------------------------------------
// Function	: terminateNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void terminateNhInValid ()
{
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);
	stopTcpNh (true);
	deleteTcpNh (true);

	// Actual Test Case
	bool retval = terminateTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/TERMINATE_NH (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/TERMINATE_NH (I)		: Fail\n");

	// Post Test Case.
	terminateTcpNh (true);

}

//----------------------------------------------------------------------------
// Function	: statusReqNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void statusReqNhValid ()
{	
	// Pre Test Case
	createTcpNh (true);
	startTcpNh (true);

	// Actual Test Case
	bool retval = statusReqTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/STATUS_REQ (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STATUS_REQ (V)		: Fail\n");


}

//----------------------------------------------------------------------------
// Function	: statusReqNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void statusReqNhInValid ()
{
	// Actual Test Case
	bool retval = statusReqTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/STATUS_REQ (I)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STATUS_REQ (I)		: Fail\n");



}

//----------------------------------------------------------------------------
// Function	: statsReqNhValid ()
// Description	: 
//----------------------------------------------------------------------------

void statsReqNhValid ()
{
	// Actual Test Case
	bool retval = statsReqTcpNh (true);
	if (retval != false)
		printf ( "TCP/MI/IDLE/STATS_REQ (V)		: Pass\n");
	else
		printf ( "TCP/MI/IDLE/STATS_REQ (V)		: Fail\n");


}

//----------------------------------------------------------------------------
// Function	: statsReqNhInValid ()
// Description	: 
//----------------------------------------------------------------------------

void statsReqNhInValid ()
{
	bool retval = statsReqTcpNh (false);
	if (retval != true)
		printf ( "TCP/MI/IDLE/STATS_REQ (I)       : Pass\n");
	else
		printf ( "TCP/MI/IDLE/STATS_REQ (I)       : Fail\n");


}

//----------------------------------------------------------------------------
// Function	: addListnValid ()
// Description	: 
//----------------------------------------------------------------------------

void addListnValid ()
{
	printf ( "----------------------------------------------------------\n");
	printf ( "                       UI Test Cases Report \n");
	printf ( "----------------------------------------------------------\n");
	// Actual Test Case
	bool retval = addListener (true);
	if (retval != true)
		printf ( "TCP/MI/RUN/ADD_LISTNER (V)		: Fail\n");


}

//----------------------------------------------------------------------------
// Function	: addListnInValid ()
// Description	: 
//----------------------------------------------------------------------------

void addListnInValid ()
{
	// Actual Test Case
	bool retval = addListener (false);
	if (retval != true)
		printf ( "TCP/MI/RUN/ADD_LISTNER (I)		: Pass\n");
	else
		printf ( "TCP/MI/RUN/ADD_LISTNER (I)		: Fail\n");

}

//----------------------------------------------------------------------------
// Function	: remListnValid ()
// Description	: 
//----------------------------------------------------------------------------

void remListnValid ()
{
	// Actual Test Case
	bool retval = remListener (true);
	if (retval != true)
		printf ( "TCP/MI/RUN/DEL_LISTNER (V)		: Fail\n");


}

//----------------------------------------------------------------------------
// Function	: remListnInValid ()
// Description	: 
//----------------------------------------------------------------------------

void remListnInValid ()
{
	// Actual Test Case
	bool retval = remListener (false);
	if (retval != true)
		printf ( "TCP/MI/RUN/DEL_LISTNER (I)		: Pass\n");
	else
		printf ( "TCP/MI/RUN/DEL_LISTNER (I)		: Fail\n");

}

//----------------------------------------------------------------------------
// Function	: conReqValid ()
// Description	: 
//----------------------------------------------------------------------------

void conReqValid ()
{
	// Actual Test Case
	bool retval = connectReq (true);
	if (retval != true)
		printf ( "TCP/MI/RUN/CON_REMT (V)		: Fail\n");

}

//----------------------------------------------------------------------------
// Function	: conReqInValid ()
// Description	: 
//----------------------------------------------------------------------------

void conReqInValid ()
{
	// Actual Test Case
	bool retval = connectReq (false);
	if (retval != true)
		printf ( "TCP/MI/RUN/CON_REMT (I)		: Pass \n");
	else
		printf ( "TCP/MI/RUN/CON_REMT (I)		: Fail\n");


}
//----------------------------------------------------------------------------
// Function	: dataReqValid ()
// Description	: 
//----------------------------------------------------------------------------

void dataReqValid ()
{
	// Actual Test Case
	bool retval = sendData (true);
	if (retval != true)
		printf ( "TCP/MI/RUN/CON_REMT (V)		: Fail\n");

}

//----------------------------------------------------------------------------
// Function	: dataReqInValid ()
// Description	: 
//----------------------------------------------------------------------------

void dataReqInValid ()
{
	// Actual Test Case
	bool retval = sendData (false);
	if (retval != true)
		printf ( "TCP/MI/RUN/CON_REMT (I)		: Pass \n");
	else
		printf ( "TCP/MI/RUN/CON_REMT (I)		: Fail\n");


}

//----------------------------------------------------------------------------
// Function	: clConReqValid ()
// Description	: 
//----------------------------------------------------------------------------

void clConReqValid ()
{
	// Actual Test Case
	bool retval = closeConReq  (true);
	if (retval != true)
		printf ( "TCP/MI/RUN/CON_REMT (V)		: Fail\n");

}

//----------------------------------------------------------------------------
// Function	: clConReqInValid ()
// Description	: 
//----------------------------------------------------------------------------

void clConReqInValid ()
{
	// Actual Test Case
	bool retval = closeConReq (false);
	if (retval != false)
		printf ( "TCP/MI/RUN/UL_DISCONNECT (V)		: Fail\n");


}

void startMiTesting ()
{
	createULThr ();	   
	createSMThr();      
	initNhValid ();	   
	sleep (1);
	initNhInValid ();	   
	sleep (1);
	genCfgNhValid ();	   
	sleep (1);
	genCfgNhInValid	 ();   
	sleep (1);
	startNhValid ();	   
	sleep (1);
	startNhInValid ();	   
	sleep (1);
	stopNhValid ();  
	sleep (1);
	stopNhInValid (); 
	sleep (1);
	deleteNhValid ();	   
	sleep (1);
	deleteNhInValid	();   
	sleep (2);
	terminateNhInValid (); 
	terminateNhValid ();  
	sleep (1);
	statusReqNhValid (); 
	statusReqNhInValid (); 
	statsReqNhValid  (); 
	statsReqNhInValid (); 

	// Clean Up.
	/*stopTcpNh(true);
	sleep (1);
	deleteTcpNh(true);
	sleep (1);
	terminateTcpNh (true);
	sleep (1);
	*/

}

void startUiTesting ()
{
	// Start Up.
	/*createULThr ();	   
	createSMThr();      
	sleep (1);
	createTcpNh(true);
	sleep (1);
	startTcpNh(true);
	sleep (1);
	*/

	addListnValid ();
	addListnInValid ();
	conReqValid ();
	conReqInValid ();
	dataReqValid ();
	dataReqInValid ();
	// Note: This sleep gives segmentation fault in NHTCP FD_ISET() in
	// reactor. 
	sleep (1);
	clConReqInValid ();
	clConReqValid ();
	remListnInValid ();
	remListnValid ();
	sleep (1);

	// Clean Up.
	stopTcpNh(true);
	deleteTcpNh(true);
	sleep (1);
	terminateTcpNh (true);
	sleep (1);
}

//============================================================================
// <End of testfunctions.cpp>
//============================================================================
