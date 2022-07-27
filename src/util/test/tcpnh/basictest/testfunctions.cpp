//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testfunctions.cpp
// Description  : Test Functions
// Author       : 
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

#elif __sun
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

void setParameterValue ()
{
	if (useValidNhModId)
	{
		initParam.nhActorId = nhModId;
	} else
	{
		//TODO:
	}

	if (useValidSmModId)
	{
		initParam.smActorId = smModId;
	} else
	{
		//TODO:
	}
	
	// Max no of connections.
	if (useMaxNoOfCon)
	{
		initParam.maxNoOfConnections = (MAX_NO_CONN * 2);
	} else
	{
		initParam.maxNoOfConnections = CDSU_NHTCP_DEF_NOCONNS;
	}

	// Max no of listeners.
	if (useMaxNoOfLstn)
	{
		initParam.maxNoOfLstns = MAX_NO_LSTN;
	} else
	{
		initParam.maxNoOfLstns = CDSU_NHTCP_DEF_NOLSTNS;
	}

	// Data select time.
	if (useZeroDataSelTimer)
	{
		initParam.dataSelectTimer = 0;
	} else
	{
		initParam.dataSelectTimer = CDSU_NHTCP_DEF_DATA_SELTMR;
	}
	// Listener connection time.
	if (useZeroLstnSelTimer)
	{
		initParam.listenSelectTimer = 0;
	} else
	{
		initParam.listenSelectTimer = CDSU_NHTCP_DEF_DATA_SELTMR;
	}
	
	// No handler timer.
	if (useZeroNoHlrTimer)
	{
		initParam.noHandleTimer = 0;
	} else
	{
		initParam.noHandleTimer = CDSU_NHTCP_DEF_NOHDL_TMR;
	}

	// Max buffer len.
	if (useValidBufferLen)
	{
		initParam.maxBufferLen = CDSU_NHTCP_DEF_TCPBUFLEN;
	} else
	{
		initParam.maxBufferLen = INVALBUFFERLEN;
	}

	// Listen queue size.
	if (useValidLstnQSz)
	{
		initParam.listenQSize = CDSU_NHTCP_DEF_LSTNQ_SIZE;
	} else
	{
		initParam.listenQSize = INVALLSTNQSZ;
	}

	if (useValidNhInstId)
	{
		termparam.nhTcpInstId = nhInstId;
		delnhparam.nhTcpInstId = nhInstId;
		ctrlReqParam.nhTcpInstId = nhInstId;
		cfgReqParam.nhTcpInstId = nhInstId;
		statusReqParam.nhTcpInstId = nhInstId;
		statsReqParam.nhTcpInstId  = nhInstId;
		
		listenReqParam.nhTcpInstId = nhInstId;
		conReqParam.nhTcpInstId = nhInstId;
		dataReqParam.connKey.nhTcpInstId = nhInstId;
		rmlstnparam.lstnKey.nhTcpInstId = nhInstId;
		clConReqParam.connKey.nhTcpInstId = nhInstId;

	} else
	{
		termparam.nhTcpInstId = INVALNHINSTID;
		delnhparam.nhTcpInstId = INVALNHINSTID;
		ctrlReqParam.nhTcpInstId = INVALNHINSTID;
		cfgReqParam.nhTcpInstId = INVALNHINSTID;
		statusReqParam.nhTcpInstId = INVALNHINSTID;
		statsReqParam.nhTcpInstId  = INVALNHINSTID;
		
		nhInstId = INVALNHINSTID;
	}
	
	if (useValidApplModId)
	{
		listenReqParam.applActorId = applnModId;
		conReqParam.applActorId = applnModId;
		dataReqParam.applActorId = applnModId;
                rmlstnparam.applActorId = applnModId;
		clConReqParam.applActorId = applnModId;
	} else 
	{
		// TODO:
	}
	
	if(!ISUIMSGINTF)
	{
		initParam.smEventHlr = utilSmEventHlr;
		listenReqParam.listenEventHlr = lstEventHlr;
		listenReqParam.conEventHlr = conEventHlr; 
		conReqParam.conEventHlr = conEventHlr;
                rmlstnparam.listenEventHlr = lstEventHlr;
		clConReqParam.conEventHlr = conEventHlr;

	} else
	{
		/*
		initParam.smEventHlr =  (void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	
		listenReqParam.listenEventHlr =(void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	
		listenReqParam.conEventHlr =(void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	
		conReqParam.conEventHlr  = (void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	
		dataReqParam.conEventHlr = (void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	 
		rmlstnparam.listenEventHlr = (void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	 
	        clConReqParam.conEventHlr  =(void (*) (void *)) CDSU_NHTCP_INV_TCPEVENTHLR;	 
		*/
	}
	
}

void setDefaultValueToParameter ()
{
	useValidNhInstId = true; 	// Nh Inst flag.
	useValidNhModId = true;		// Nh ModId flag.
	useValidSmModId = true;		// SM ModId flag.
	useValidApplModId = true;	// Appl ModId flag.

	useMaxNoOfCon = true;		// Max Con flag.
	useMaxNoOfLstn = true;		// Max Lstn flag.

	useZeroLstnSelTimer = false;	// Lstn Sel Timer flag.
	useZeroDataSelTimer = false;	// Data Sel Timer flag.
	useZeroNoHlrTimer = false;	// No Handle Timer flag.

	useValidLstnQSz = true;		// Lstn Q sz flag. 
	useValidDataPtr = true;		// Data Ptr flag.
	useZeroDataLen  = false;	// Data Len flag.
	useValidBufferLen = true;	// Buffer Len flag.
}

//----------------------------------------------------------------------------
// Seting Parameter Functions :
// Each functions sets different type of parameter. Each Parameter has some
// Default value. Using Following functions tester can alterate the value of
// parameters.
//----------------------------------------------------------------------------
// Set Invalid Instance ID.
void setInvNhInstId ()
{
	useValidNhInstId = false;
	// printf ("Invalid NH Instance Id :: Set\n");
} // void setInvNhInstId ()

//Set Invalid SM actor ID.
void setInvSmActorId ()
{
	useValidSmModId	= false;
} //void setInvSmActorId ()

// Set Invalid Application Actro ID.
void setInvApplActorId ()
{
	useValidApplModId = false;
} // void setInvApplActorId ()

// Set Invalid NH Actor ID.
void setInvNhActorId ()
{	
	useValidNhModId = false;
} // void setInvNhActorId ()

// set Lstn Select Timer to Zero
void setLstnSelTimerZero ()
{
	useZeroLstnSelTimer = true;
} // void setLstnSelTimerZero ()

// set Data Select Timer to Zero
void setDataSelTimerZero ()
{
	useZeroDataSelTimer = true;
} // void setSelTimerZero ()


// set No Hlr Timer
void setNoHlrTimerZero ()
{
	useZeroNoHlrTimer = true;
} // void setSelTimerZero ()

// Set Invalid Buffer Len
void setInvBufLen ()
{
	useValidBufferLen = false;
} // void setInvBufLen ()

// Setting Invalid Data Pointer
void setInvDataPtr ()
{
	useValidDataPtr = false;
} // void setInvDataPtr ()

// Setting Data Len to 0
void setDataLenZero ()
{
	useZeroDataLen = true;
} // void setInvDataPtr ()



//----------------------------------------------------------------------------
// Function	: createTcpNh ()
// Description	: It creates TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void createTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();
	
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiInit (initParam, nhInstId);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (4, "Init TCP NH API call :: CDSU_NHTCP_API_RT_SUCCESS.\n");
	} else
	{
		CDSU_TRACE (4, "Init TCP NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

} // createTcpNh ()

//----------------------------------------------------------------------------
// Function	: configTcpNh ()
// Description	: It configures TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void configTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	cfgReqParam.nhTcpInstId = nhInstId;

	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCfgReq (cfgReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Config NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
	} else
	{
		CDSU_TRACE (5, "Config NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();
	
} // configTcpNh ()

//----------------------------------------------------------------------------
// Function	: startTcpNh ()
// Description	: It starts TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void startTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	setParameterValue ();

	ctrlReqParam.nhTcpInstId = nhInstId;
	ctrlReqParam.subOpcode = CDSU_NHTCP_SO_START_REQ;
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCtrlReq (ctrlReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Start NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
	} else
	{
		CDSU_TRACE (5, "Start NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

} // startTcpNh ()

//----------------------------------------------------------------------------
// Function	: stopTcpNh ()
// Description	: It stops TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void stopTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	setParameterValue ();

	ctrlReqParam.init (nhInstId, CDSU_NHTCP_SO_STOP_REQ);
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCtrlReq (ctrlReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Stop NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
	} else
	{
		CDSU_TRACE (5, "Stop NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

} // stopTcpNh ()


//----------------------------------------------------------------------------
// Function	: deleteTcpNh ()
// Description	: It deletes TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void deleteTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	setParameterValue ();

	delnhparam.init (nhInstId);
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiDelNhReq(delnhparam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Delete NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
	} else
	{
		CDSU_TRACE (5, "Delete NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

} // deleteTcpNh ()

//----------------------------------------------------------------------------
// Function	: terminateTcpNh ()
// Description	: It terminates TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void terminateTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	setParameterValue ();

	termparam.init (nhInstId);
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiTerminate(termparam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Terminate NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
	} else
	{
		CDSU_TRACE (5, "Terminate NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

} // terminateTcpNh ()

//----------------------------------------------------------------------------
// Function	: statusReqTcpNh ()
// Description	: It requests for status of TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void statusReqTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	statusReqParam.init (nhInstId);
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiStatusReq (statusReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Status NH API call :: CDSU_NHTCP_API_RT_SUCCESS\n");
	} else
	{
		CDSU_TRACE (5, "Status NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();


} // statusReqTcpNh ()

//----------------------------------------------------------------------------
// Function	: statsReqTcpNh ()
// Description	: It requests for statistics of TCP Network Handler.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void statsReqTcpNh ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	setParameterValue ();
	
	statsReqParam.init (nhInstId);
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiStsReq (statsReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Statistics NH API call :: CDSU_NHTCP_API_RT_SUCCESS\n");
	} else
	{
		CDSU_TRACE (5, "Statistics NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

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

	nhInstId = 0xffff;

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
	
	// Note: SM Hardware Unit Id should be same as it is given in testserver.
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
		} else
		{
			if (subOpcode == CDSU_NHTCP_SO_START_REQ)
			{
				CDSU_TRACE (1 ,"Start TCP NH Fail.\n");
			} else  if (subOpcode == CDSU_NHTCP_SO_STOP_REQ)
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
				CDSU_TRACE2 (1, "%s = %d\n",  counterString [i], stsRspEvnetParam->counters [i]);
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
			CDSU_TRACE (5, "Add Listener failed.\n");
			displayErrorType (((CdSuNhTcpUiAddLstnRspEvent*)evt)->failCode);
		} else 
		{
			CDSU_TRACE (5, "Add Listener succesful.\n");
		}
		//Delete event.
		delete (CdSuNhTcpUiAddLstnRspEvent*) evt;
		evt = NULL;

		}
		break;

	case CDSU_NHTCP_UI_CON_IND:
		{
		printf ("Connection Indication .............\n");
		++noOfConnInd;
		CDSU_TRACE (5, "New Connection on Listener.\n");

		CdSuNhTcpUiConIndEvent* conIndEvent =
					(CdSuNhTcpUiConIndEvent*) evt;
		// printf ("%d\n", noConnInd);
		noConnInd++;
		// Print Remote Ipaddr and Port No.
		CDSU_TRACE1 (1, "Port No from where connection has arrived: %d\n", conIndEvent->dstPortNo);	

		unsigned char ipaddr [4];
		memcpy ((void*) ipaddr, (void*) &conIndEvent->dstIpv4Addr, 4);
		CDSU_TRACE (1, "Ipaddr is :\n");
		for (int i = 0; i < 4; i++)
		{
			CDSU_TRACE1 (1, "%d\n", ipaddr [i]);
		}
		// Extract Connection Key.
		// connKey [0] = conIndEvent->connKey;
		
		// Delete event.
		delete (CdSuNhTcpUiConIndEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_REM_LISTENER_RSP:
		{
		if (((CdSuNhTcpUiRemLstnRspEvent*)evt)->rspCode != CDSU_NHTCP_RSPCODE_SUCCESS)
		{
			CDSU_TRACE (5, "Remove Listener failed.\n"); 
			displayErrorType (((CdSuNhTcpUiRemLstnRspEvent*)evt)->failCode);
		} else 
		{
			CDSU_TRACE (5, "Remove Listener succesful.\n");
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
			printf (" Connect Response Failed\n");
			displayErrorType (((CdSuNhTcpUiConRspEvent*)evt)->failCode);
		} else 
		{
			CDSU_TRACE (5, "Open Connection succesful.");
			printf (" Connect Response Success\n");
			// noConnInd++;
			// printf ("%d \n", noConnInd);
			noOfSuccessConnRsp ++;
		}
		// Delete event.
		delete (CdSuNhTcpUiConRspEvent*) evt;
		evt = NULL;
		}
		break;

		
	case CDSU_NHTCP_UI_DATA_IND:
		{
		++ noOfDataInd;	
		// printf ("Data Indication \n");
		CDSU_TRACE (5, "Data Indication.");
		CdSuNhTcpUiDataIndEvent* dataIndEvent = 
					(CdSuNhTcpUiDataIndEvent*) evt;
		CDSU_TRACE (5, "Data Indication.");
		int len = dataIndEvent->dataLen;
		CDSU_TRACE1 (1, "Length of data is %d\n", len);
		char data [len];
		strncpy (data, dataIndEvent->dataPtr, len);
		CDSU_TRACE1 (1, "Data Received: %s\n", data);

		// Delete event.
		delete (CdSuNhTcpUiDataIndEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_IND:
		{
		CDSU_TRACE (1, "Close Connection Indication received\n");
		// Delete event.
		delete (CdSuNhTcpUiCloseConRspEvent*) evt;
		evt = NULL;
		}
		break;

	case CDSU_NHTCP_UI_DATA_RSP:	
		{
		
		if (time (&endTime) == -1)
		{
			printf (" Getting System Timer Error\n");
			exit (-1);
		}
		if (((CdSuNhTcpUiDataRspEvent*)evt)->rspCode != CDSU_NHTCP_RSPCODE_SUCCESS)
		{
			CDSU_TRACE (5, "Send Data failed.\n");
			displayErrorType (((CdSuNhTcpUiDataRspEvent*)evt)->failCode);
		} else 
		{
			CDSU_TRACE (5, "Send Data succesful.\n");
			++noOfSendDataRspSuccess;
			printf ("Send Data Success Response [%d]\n", noOfSendDataRspSuccess);
			// printf ("%d \n", noConnInd);
		}
		// Delete event.
		delete (CdSuNhTcpUiDataRspEvent*) evt;
		evt = NULL;

		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_RSP:	
		{
		if (((CdSuNhTcpUiCloseConRspEvent*)evt)->rspCode != CDSU_NHTCP_RSPCODE_SUCCESS)
		{
			CDSU_TRACE (5, "Disconnect request failed.\n");
			displayErrorType (((CdSuNhTcpUiCloseConRspEvent*)evt)->failCode);
		} else 
		{
			CDSU_TRACE (5, "Disconnect request successful.\n");
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
	
} // conEventHlr ()

//----------------------------------------------------------------------------
// Function	: addListener ()
// Description	: It adds tcp listener.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void addListener ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	setParameterValue ();
	
	noConnInd = 1;
	tpkt = false;

	char* lsip =  selfIpAddr;
	CdSuIpv4Addr lsipv4 = convIp2Int (lsip); 
	listenReqParam.init (nhInstId,
			     applnModId,
			     lstEventHlr,
			     conEventHlr,
			     lsipv4,
			     9999,
			     tpkt);

	CdSuNhTcpApiRetType retType = cdSuNhTcpUiAddLstnReq (listenReqParam, lstnKey);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Add Listner API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
	} else
	{
		CDSU_TRACE (5, "Add Listner API call :: ERROR\n" );
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

} // addListener ()

//----------------------------------------------------------------------------
// Function	: connectReq ()
// Description	: It establishes the connection to remote end.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void connectReq ()

{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	noConnInd = 1;
	char* dstip =  selfIpAddr;
	CdSuIpv4Addr dstipv4 = convIp2Int (dstip);

	conReqParam.init (nhInstId,
			  applnModId,
			  conEventHlr,
			  dstipv4,
			  9999,
			  tpkt);

	CdSuNhTcpApiRetType retType = cdSuNhTcpUiConReq (conReqParam, connKey [0]);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Connect Req API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
	} else
	{
		CDSU_TRACE (5, "Connect Req API call :: ERROR\n" );
		displayApiRtType (retType);
	}

	setDefaultValueToParameter ();

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

	setParameterValue ();

	noConnInd = 1;
	char* dstip = selfIpAddr;
	CdSuIpv4Addr dstipv4 = convIp2Int (dstip);
	conReqParam.init (nhInstId,
				  applnModId,
				  conEventHlr,
				  dstipv4,
				  9999,
				  tpkt);

	for (int i= 1; i <= MAX_NO_CONN; i++)
	{
		printf (" Connection Request [%d]\n", i);
		CdSuNhTcpApiRetType retType = cdSuNhTcpUiConReq (conReqParam, connKey [i]);
		if (retType == CDSU_NHTCP_API_RT_SUCCESS)
		{
			// CDSU_TRACE (5, "Connect Req API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
			printf (" Connection Request [%d] success\n", i);
			++noOfSuccessConnReq;
		} else
		{
			printf (" Connection Request [%d] failed\n", i);
		}

		if (i % 10 == 0)
		{
			sleep (3);
		}

	}
	printf (" Connection Request Finished ...........................\n");
	sleep (1);
	setDefaultValueToParameter ();

} // connectReq ()

//----------------------------------------------------------------------------
// Function	: sendData ()
// Description	: It sends data on single connections.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void sendData ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	
	setParameterValue ();

	char* dstip =  selfIpAddr;
	char* data = "TEST MSG";
	int len = strlen (data);

	dataReqParam.init (connKey [0], applnModId, conEventHlr, data, len);
	// for (int i=0 ; i < 500; i++)
	CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Send Data API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
	} else
	{
		CDSU_TRACE (5, "Send Data API call :: ERROR\n" );
		displayApiRtType (retType);
	}
	setDefaultValueToParameter ();


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
	setParameterValue ();

	char* dstip = selfIpAddr;
	char* data = "TEST MSG";
	int len = strlen (data);

	if (time (&startTime) == -1)
	{
		printf (" Getting System Timer Error\n");
		exit (-1);
	}
	for (int j = 0; j < MAX_SEND_DATA_ATTEMPT; j ++)
	{
		for (int i=1; i <= MAX_NO_CONN; i++)
		{
			++noOfSendDataReq;
			printf ("Send Data Request [%d]\n", noOfSendDataReq);
			dataReqParam.init (connKey[i], applnModId, conEventHlr, data, len);
			CdSuNhTcpApiRetType retType = cdSuNhTcpUiDataReq (dataReqParam);
			if (retType == CDSU_NHTCP_API_RT_SUCCESS)
			{
				++noOfSendDataReqSuccess;
				printf ("Send Data Request Success\n");
			} else
			{
				printf ("Send Data Request failed\n");
			} // if ()
		}
	}
	
	// gettimeofday (&endTimeStamp, NULL);
	
	setDefaultValueToParameter ();
	
} // sendDataLoadTesting ()

//----------------------------------------------------------------------------
// Function	: remListener ()
// Description	: It removes tcp listener.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void remListener ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);

	setParameterValue ();

	char* lsip =  selfIpAddr;
	CdSuIpv4Addr lsipv4 = convIp2Int (lsip); 

	rmlstnparam.init (lstnKey,
			     applnModId,
			     lstEventHlr);

	 CdSuNhTcpApiRetType retType = cdSuNhTcpUiRemLstnReq (rmlstnparam);
	 if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	 {
		CDSU_TRACE (5, "Remove Listener API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
	 } else
	 {

		CDSU_TRACE (5, "Remove Listner API call :: ERROR\n" );
		displayApiRtType (retType);
	 }
	
	setDefaultValueToParameter ();

} // remListener ()

//----------------------------------------------------------------------------
// Function	: closeConReq ()
// Description	: It closes a connection.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void closeConReq ()
{
	CDSU_TRACE1 (4, "%s \n", __PRETTY_FUNCTION__);
	setParameterValue ();

	clConReqParam.init (connKey [0], applnModId, CDSU_NHTCP_CCR_NORMAL);	
	clConReqParam.conEventHlr = conEventHlr;
	CdSuNhTcpApiRetType retType = cdSuNhTcpUiCloseConReq (clConReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Close Connection API call :: CDSU_NHTCP_API_RT_SUCCESS\n" );
	} else
	{

		CDSU_TRACE (5, "Close Connection API call :: ERROR\n" );
		displayApiRtType (retType);
	}
	setDefaultValueToParameter ();

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
	setParameterValue ();

	for (int i = 1; i <= MAX_NO_CONN; i++)
	{
		clConReqParam.init (connKey[i], applnModId, CDSU_NHTCP_CCR_NORMAL);	
		clConReqParam.conEventHlr = conEventHlr;
		CdSuNhTcpApiRetType retType = cdSuNhTcpUiCloseConReq (clConReqParam);
	}

	setDefaultValueToParameter ();

} // closeConReqLoadTesting ()
//----------------------------------------------------------------------------
// Function	: printStats ()
// Description	: terminates application.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void printStats ()
{
        timeGap = (endTime - startTime); 
        printf ("No Of Success Connection Request %d\n", noOfSuccessConnReq);
        printf ("No Of Success Connection Response %d\n", noOfSuccessConnRsp);
	printf ("No Of Connection Indication %d\n", noOfConnInd);
        printf ("No Of Send Data Request %d\n", noOfSendDataReq);
        printf ("No Of Success Send Data Request%d\n", noOfSendDataReqSuccess);
        printf ("No Of Success Send Data Response %d\n", noOfSendDataRspSuccess);
        printf ("No Of Data Indication %d\n", noOfDataInd);
	printf ("Seconds Spent %ld\n", timeGap );
        printf ("No Of Data Sent Per Seconds %lf\n", (double)noOfSendDataRspSuccess / timeGap);
}

//----------------------------------------------------------------------------
// Function	: cdSuExit ()
// Description	: terminates application.
// Inputs	: None
// Return Value	: None
// Side Effects	: None
// Algorithm	:
//----------------------------------------------------------------------------

void cdSuExit ()
{
	ctrlReqParam.nhTcpInstId = nhInstId;
	ctrlReqParam.subOpcode = CDSU_NHTCP_SO_STOP_REQ;
	CdSuNhTcpApiRetType retType = cdSuNhTcpMiCtrlReq (ctrlReqParam);
	if (retType == CDSU_NHTCP_API_RT_SUCCESS)
	{
		CDSU_TRACE (5, "Stop NH API call :: CDSU_NHTCP_API_RT_SUCCESS \n");
	} else
	{
		CDSU_TRACE (5, "Stop NH API call :: ERROR.\n");
		displayApiRtType (retType);
	}

	CdSuNhTcpMiTerminateParam terminateParam;
	terminateParam.init (nhInstId);
	cdSuNhTcpMiTerminate (terminateParam);
	cdSuMdTerminate ();
	exit (0);
}
//============================================================================
// <End of testfunctions.cpp>
//============================================================================
