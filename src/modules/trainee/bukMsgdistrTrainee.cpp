//==============================================================================
// BUK assignment - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	    : bukMsgdistrTrainee.cpp
// Description	:
// Ref Docs 	:
// Author	    : Trainee - 2013
// Created On   : 07.11.2013
// Last Modified: 26.07.2022 by rohitm
//==============================================================================

#include "bukTrainee.h"

// Retrieving module ids to send and receive mesaages
CdModuleId mentorMid = getApplMidMentor();		    		// Mentor Module ID
CdModuleId adminMid = getApplMidAdmin();			    	// Admin Module ID
CdModuleId traineeMid = getApplMidTrainee();				// Trainee Module ID (Self)
CdModuleId myTraineeMenuMid = getApplMidMyTraineeMenu();	// MyTraineeMenu Module ID
CdSuActorId nhTraineeMid = getNhMidTrainee();				// Network Handler Module ID (Self)

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
	//sem_post (&semaphore);
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
			printf ("Remote PortNo %d\n",((CdSuNhUdpUiDataIndEvent*)eventParam)->destPortNo);
			printf ("Dest Addr = %s\n", cdSuNhUdpConvInt2Addr (((CdSuNhUdpUiDataIndEvent*)eventParam)->destAddr));




			BukUiTrMsg* msg = (BukUiTrMsg *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;

			switch (msg->opcode)
			{
			case BUK_UI_TR_REG:
				{
				CdMessage* sndMsg = (CdMessage *) cdSuGetMsgBuf (sizeof (CdMessage));
				sndMsg->initMsg (traineeMid, nhTraineeMid, BUK_UI_TR_REG, sizeof (BUK_UI_TR_REG));
				CDSU_TRACE (1,"Registration Request processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH: Sent message\n");
				}
				}
				break;
			case BUK_UI_TR_GDNCRQST:
				{
				BukUiTrGdncRqst* rcvMsg = (BukUiTrGdncRqst *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
				BukTrGdncRqst* sndMsg = (BukTrGdncRqst *) cdSuGetMsgBuf (sizeof (BukTrGdncRqst));
				sndMsg->initMsg (traineeMid, nhTraineeMid, rcvMsg->tId);
				CDSU_TRACE (1,"Guidance request Message processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH: Sent message.\n");
				}
				}
				break;
			case BUK_UI_TR_EXIT:
				{
					exit (0);
				}
				break;
			default:
				CDSU_TRACE (1, "wrong opcode\n");
				break;
			}	//switch (msg->opcode)




//			BukMyTraineeMenuTrMsg* msg = (BukMyTraineeMenuTrMsg *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
//
//			switch (msg->opcode)
//			{
//			case BUK_MYTRAINEEMENU_TR_REQ:
//				{
//				CdMessage* sndMsg = (CdMessage *) cdSuGetMsgBuf (sizeof (CdMessage));
//				sndMsg->initMsg (traineeMid, nhTraineeMid, BUK_MYTRAINEEMENU_TR_REG, sizeof (BUK_MYTRAINEEMENU_TR_REG));
//				CDSU_TRACE (1,"Registration Request processing...\n");
//				if (cdSuMdSendMsg (sndMsg) == false)
//				{
//					CDSU_TRACE (1, "NH: Error sending message\n");
//					cdSuFreeMsgBuf (sndMsg);
//				}else
//				{
//					CDSU_TRACE (1, "NH: Sent message\n");
//				}
//				}
//				break;
//			case BUK_MYTRAINEEMENU_TR_GDNCRQST:
//				{
//				BukMyTraineeMenuTrGdncRqst* rcvMsg = (BukMyTraineeMenuTrGdncRqst *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
//				BukTrGdncRqst* sndMsg = (BukTrGdncRqst *) cdSuGetMsgBuf (sizeof (BukTrGdncRqst));
//				sndMsg->initMsg (traineeMid, nhTraineeMid, rcvMsg->tId);
//				CDSU_TRACE (1,"Guidance request Message processing...\n");
//				if (cdSuMdSendMsg (sndMsg) == false)
//				{
//					CDSU_TRACE (1, "NH: Error sending message\n");
//					cdSuFreeMsgBuf (sndMsg);
//				}else
//				{
//					CDSU_TRACE (1, "NH: Sent message.\n");
//				}
//				}
//				break;
//			case BUK_MYTRAINEEMENU_TR_EXIT:
//				{
//					exit (0);
//				}
//				break;
//			default:
//				CDSU_TRACE (1, "wrong opcode\n");
//				break;
//			}	//switch (msg->opcode)




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
	 //sem_post (&semaphore);

} // applEventHlr ()



//---------------------------------------------------------------------------
// applicationStart1 ()
// Description  : First application thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------
//thread id = testThread1 : traineeMid
void* TraineeMain::applicationStart (void* dummy1)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiInitParam initParam;

	//--------------------------------------------------------------------
	// 1.First Instance : Management Intf is Callback.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhTraineeMid;
	initParam.smActorId = traineeMid;
	initParam.smEventHlr = smMgmtEventHlr;

	// Create Instance.
	cdSuNhUdpMiInit (initParam, instID);

	// Start The UDP Nh
	CdSuNhUdpMiCtrlReqParam ctrlReqParam;
	ctrlReqParam.nhUdpInstId = instID;
	ctrlReqParam.subOpcode = CDSU_NHUDP_SO_START_REQ;
	if (cdSuNhUdpMiCtrlReq (ctrlReqParam) == CDSU_NHUDP_API_RT_SUCCESS )
	{
		printf ("Start NH Req : Successful\n");
	} else
	{
		printf ("Start NH Req : fail\n");
	}

	CdSuNhUdpUiAddUdpPortReqParam addUdpPortReqParam;

	//--------------------------------------------------------------------
	// 1. Udp Port of type Both Fixed. : Callback Function Intf.
	// Instance : 1.
	//--------------------------------------------------------------------
	addUdpPortReqParam.nhUdpInstId	= instID;
	addUdpPortReqParam.applActorId	= traineeMid;
	addUdpPortReqParam.applEventHlr = applEventHlr;
	addUdpPortReqParam.localPortNo = localPortOfBothFixedTrainee;
	addUdpPortReqParam.destPortNo = remotePortOfBothFixedTrainee;
	addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);

	// Add Udp Port
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, portKey);

	traineeMain->msgDistr();

}// applicationStart1


void TraineeMain::msgDistr()
{
	CdMessage* rxMsg;

	// Any Message that the Application receives.
	while (1)
	{
		if (cdSuMdRecvMsg (&rxMsg, traineeMid) == true)
		{
			CDSU_TRACE (5, "\nMessage received in MsgDistrTrainee\n");

			if (rxMsg->msgHdr.src == nhTraineeMid)
			{
				CDSU_TRACE(5, "Message Source: Trainee Menu\n");
				msgFromMenu(rxMsg);
//				CDSU_TRACE(5, "Message Source: MyTraineeMenu\n");	// rohitm
//				msgFromMyTraineeMenu(rxMsg);	// rohitm
			}

			if (rxMsg->msgHdr.src == mentorMid)
			{
				CDSU_TRACE(5, "Message Source: Mentor\n");
				msgFromMentor(rxMsg);
			}

			if (rxMsg->msgHdr.src == adminMid)
			{
				CDSU_TRACE(5, "Message Source: Admin\n");
				msgFromAdmin(rxMsg);
			}

			if (rxMsg->msgHdr.src == myTraineeMenuMid)
			{
				CDSU_TRACE(5, "Message Source: MyTraineeMenu\n");
				msgFromMyTraineeMenu(rxMsg);
			}

			cdSuFreeMsgBuf (rxMsg);

			// Exits Thread1
			//pthread_exit (&applThread1);
		}
		// exit (0);
	} // while loop

} //msgDistr()




void TraineeMain::msgFromMenu(CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	case BUK_UI_TR_REG:
		{
		CDSU_TRACE (5, "Request for Registration Message Arrived\n");

		if (startState == TRAINEE_INIT)
		{
			CDSU_TRACE (5, "Trainee: in INIT state\n");
			int tId = ++noOfTrainee;
			if (tId < MAX_TRAINEE)
			{
				trainee[tId].handleMsg(msg, MENU, (void *)tId);

			} else
			{
				CDSU_TRACE (5, "No More Registration -- Maximum number of trainees exceeding\n");
			}

		} else
		{
			CDSU_TRACE (5, "No more Registration -- Training Started\n");
		}
		} //case BUK_UI_TR_REG
		break;
	case BUK_UI_TR_GDNCRQST:
		{
		CDSU_TRACE (5, "Guidance Request Message Arrived\n");

		BukTrGdncRqst* rcvMsg = (BukTrGdncRqst *)msg;

		int tId = rcvMsg->tId;
		trainee[tId].handleMsg(msg, MENU, NULL);
		} //case BUK_UI_TR_GDNCRQST
		break;
	default:
		CDSU_TRACE (5, "Wrong Menu Message Opcode\n");
		break;
	}
} // msgFromMenu()

//void TraineeMain::msgFromMyTraineeMenu(CdMessage* msg)
//{
//	switch (msg->msgHdr.opcode)
//	{
//	case BUK_MYTRAINEEMENU_TR_REQ:
//		{
//		CDSU_TRACE (5, "Some Request Message from MyTraineeMenu Arrived\n");
//		BukMyTraineeMenuTrMsgs* rcvMsg = (BukMyTraineeMenuTrMsgs *)msg;
//		int reqId = rcvMsg->reqId;
//		traineeObj.handleMsg(msg, MYTRAINEEMENU, (void *)reqId);
//		} //case BUK_MYTRAINEEMENU_TR_REG
//		break;
//	default:
//		CDSU_TRACE (5, "Wrong Menu Message Opcode\n");
//		break;
//	}
//} // msgFromMyTraineeMenu()

void TraineeMain::msgFromMyTraineeMenu(CdMessage* msg)
{
	BukMyTraineeMenuTrMsgs* rcvMsg = (BukMyTraineeMenuTrMsgs *)msg;
	int reqId = rcvMsg->reqId;
	traineeObj.handleMsg(msg, MYTRAINEEMENU, (void *)reqId);
} // msgFromMyTraineeMenu()


void TraineeMain::msgFromAdmin(CdMessage* msg)
{
	BukAdTrMsgs* rcvMsg = (BukAdTrMsgs *)msg;

	int tId = rcvMsg->tId;

	trainee[tId].handleMsg (msg, ADMIN, NULL);

} //msgFromAdmin

void TraineeMain::msgFromMentor(CdMessage* msg)
{
	BukMtTrMsgs* rcvMsg = (BukMtTrMsgs *)msg;

	int tId = rcvMsg->tId;

	trainee[tId].handleMsg(msg, MENTOR, NULL);

} //msgFromMentor


void Trainee::startPeriodicReportTimer (int i)
{
	CDSU_TRACE (1,"start Periodic Report Timer\n");
	int back;
	CdSuTimerInfo timerInfo1;

	timerInfo1.timeCounts = 18;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId = traineeMid;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_CYCLIC;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	timerInfo1.timerCallBackFn = periodicReportTimerExpires;
	timerInfo1.userInfo[0] = i;

	back = timerHandler->startTimer(reportTimerId[i], timerInfo1);
	if (back == CDSU_TMR_RV_INV_TMRID)
	{
		printf ("Periodic Report Timer NOT started\n");
	} else if (back == CDSU_TMR_RV_SUCCESS)
	{
		printf ("Periodic Report Timer started\n");
	}
}

void periodicReportTimerExpires(CdSuTimeout& timeout)
{
	int tId = timeout.userInfo[0];
	CDSU_TRACE1 (5, "Trainee %d: Sending Periodic Report to Mentor\n", tId);
	trainee[tId].handleTimer();
}

void Trainee::handleTimer()
{
	int tId = getTraineeId();
	int state = getState();

	if (state == TRAINEE_TRAINING)
	{
		BukTrMtPrdcRprt* sndMsg = (BukTrMtPrdcRprt *) cdSuGetMsgBuf (sizeof (BukTrMtPrdcRprt));
		sndMsg->initMsg (mentorMid, traineeMid, tId);
		if (cdSuMdSendMsg (sndMsg) == false)
		{
			CDSU_TRACE1 (1, "Trainee %d: Error sending message\n", tId);
			cdSuFreeMsgBuf (sndMsg);
		}else
		{
			CDSU_TRACE (1, "Sent message\n");
		}
	} else
	{
		CDSU_TRACE (5, "Wrong state\n");
	}
}

void Trainee::handleMsg(CdMessage *msg, int src, void* arg)
{
	if (src == MENU)
	{
		switch (msg->msgHdr.opcode)
		{
		case BUK_UI_TR_REG:
			{
			CDSU_TRACE (5, "Request for Registration Msg processing...\n");

			//int tId = (int) arg;
			int tId = (long) arg; // rohitm

			setTraineeId(tId);
			setState(TRAINEE_INIT);

			CDSU_TRACE1 (5, "Trainee Id = %d\n", tId);
			CDSU_TRACE1 (5, "Trainee %d state: INIT\n", tId);

			BukTrAdReg* sndMsg = (BukTrAdReg *) cdSuGetMsgBuf (sizeof(BukTrAdReg));
			sndMsg->initMsg(adminMid, traineeMid, tId);
			CDSU_TRACE (1,"Registration request Sending to Admin..\n");
			if (cdSuMdSendMsg (sndMsg) == false)
			{
				CDSU_TRACE (1, "Trainee: Error sending message\n");
				cdSuFreeMsgBuf (sndMsg);
			}else
			{
				CDSU_TRACE (1, "Sent message\n");
			}
			} //case BUK_UI_TR_REG
			break;

		case BUK_UI_TR_GDNCRQST:
			{
			CDSU_TRACE (5, "Guidance Request Message processing...\n");

			int tId = getTraineeId();
			if (tId != 0)
			{
				CDSU_TRACE1 (5, "Guidance Request Message from Trainee %d\n", tId);
			}

			int state = getState();

			if (state == TRAINEE_TRAINING)
			{
				CDSU_TRACE (5, "Trainee %d: in TRAINING state\n");

				int mId = getMentorId();

				BukTrMtGdncRqst* sndMsg = (BukTrMtGdncRqst *) cdSuGetMsgBuf (sizeof(BukTrMtGdncRqst));
				sndMsg->initMsg(mentorMid, traineeMid, mId);

				CDSU_TRACE2 (1,"Trainee %d: Sending Guidance Request Message to Mentor %d\n", tId, mId);

				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Trainee: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message.\n");
				}
			}
			else
			{
				if (tId == 0)
				{
					CDSU_TRACE (5, "Trainee not Registered\n");
				} else
				{
					CDSU_TRACE1 (5, "Trainee can't send guidance request in %d state\n", state);
				}
			}
			} //case BUK_UI_TR_GDNCRQST
			break;

//		case BUK_MYTRAINEEMENU_TR_GDNCRQST:
//			{
//			CDSU_TRACE (5, "Guidance Request Message processing...\n");
//
//			int tId = getTraineeId();
//			if (tId != 0)
//			{
//				CDSU_TRACE1 (5, "Guidance Request Message from Trainee %d\n", tId);
//			}
//
//			int state = getState();
//
//			if (state == TRAINEE_TRAINING)
//			{
//				CDSU_TRACE (5, "Trainee %d: in TRAINING state\n");
//
//				int mId = getMentorId();
//
//				BukTrMtGdncRqst* sndMsg = (BukTrMtGdncRqst *) cdSuGetMsgBuf (sizeof(BukTrMtGdncRqst));
//				sndMsg->initMsg(mentorMid, traineeMid, mId);
//
//				CDSU_TRACE2 (1,"Trainee %d: Sending Guidance Request Message to Mentor %d\n", tId, mId);
//
//				if (cdSuMdSendMsg (sndMsg) == false)
//				{
//					CDSU_TRACE (1, "Trainee: Error sending message\n");
//					cdSuFreeMsgBuf (sndMsg);
//				}else
//				{
//					CDSU_TRACE (1, "Sent message.\n");
//				}
//			}
//			else
//			{
//				if (tId == 0)
//				{
//					CDSU_TRACE (5, "Trainee not Registered\n");
//				} else
//				{
//					CDSU_TRACE1 (5, "Trainee can't send guidance request in %d state\n", state);
//				}
//			}
//			} //case BUK_MYTRAINEEMENU_TR_GDNCRQST
//			break;

		default:
			CDSU_TRACE (5, "Wrong Menu Msg Opcode\n");
			break;
		} // switch
	} else if (src == ADMIN)
	{
		switch (msg->msgHdr.opcode)
		{
		case BUK_AD_TR_REGACK:
			{
			CDSU_TRACE (5, "Message: Registration Ack\n");

			int tId = getTraineeId();
			int state = getState();

			if (state == TRAINEE_INIT)
			{
				CDSU_TRACE1 (5, "Trainee %d: in INIT state\n", tId);
				setState(TRAINEE_REGISTERED);
				CDSU_TRACE1 (5, "Trainee %d: state changed to REGISTERED\n", tId);
			} else
			{
				CDSU_TRACE1 (5, "Trainee %d: in wrong state\n", tId);
			}
			} //case BUK_AD_TR_REGACK
			break;
		case BUK_AD_TR_REGNACK:
			{
			CDSU_TRACE (5, "Message: Registration Nack\n");

			int tId = getTraineeId();
			int state = getState();

			if (state == TRAINEE_INIT)
			{
				CDSU_TRACE1 (5, "Trainee %d: in INIT state\n", tId);
				setState(TRAINEE_TERMINATED);
				CDSU_TRACE1 (5, "Trainee %d: changed state to TERMINATED\n", tId);
			} else
			{
				CDSU_TRACE1 (5, "Trainee %d: in wrong state\n", tId);
			}
			} //case BUK_AD_TR_REGNACK
			break;
		case BUK_AD_TR_MTID:
			{
			CDSU_TRACE (5, "Message: Mentor Id\n");

			BukAdTrMid* rcvMsg = (BukAdTrMid *)msg;

			int mId = rcvMsg->mId;

			int tId =  getTraineeId();
			int state = getState();

			totalAck++;
			if (state == TRAINEE_REGISTERED)
			{
				CDSU_TRACE1 (5, "Trainee %d: in REGISTERED state\n", tId);

				// no more registration accepted from menu
				startState = TRAINEE_REGISTERED;

				setMentorId(mId);
			}  else
			{
				CDSU_TRACE (5, "Start Training Request in wrong state\n");
			}
			} //case BUK_AD_TR_MTID
			break;
		case BUK_AD_TR_TRCMPLT:
			{
			CDSU_TRACE (5, "Message: Training Complete\n");

			int tId = getTraineeId();
			int state = getState();

			if (state == TRAINEE_TRAINING)
			{

				int back1 = timerHandler->stopTimer(reportTimerId[tId]);

				CDSU_TRACE1 (5, "Trainee %d: in TRAINING state\n", tId);

				BukTrAdTrCmpltAck* sndMsg = (BukTrAdTrCmpltAck *) cdSuGetMsgBuf (sizeof(BukTrAdTrCmpltAck));
				sndMsg->initMsg(adminMid, traineeMid, tId);

				CDSU_TRACE1 (1,"Trainee %d: Training Complete Ack Sending...\n", tId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Trainee: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}

				setState(TRAINEE_TERMINATED);
				CDSU_TRACE1 (5, "Trainee %d: changed state to TERMINATED\n", tId);

				totalAck--;
				if (totalAck == 0)
				{
					exit (0);
				}
			} else
			{
				CDSU_TRACE (5, "Stop Training Request in wrong state\n");
			}
			} //case BUK_AD_TR_TRCMPLT
			break;
		default:
			CDSU_TRACE (5, "Wrong Mentor Msg Opcode\n");
			break;
		}	//switch

	} else if (src == MENTOR)
	{
		switch (msg->msgHdr.opcode)
		{
		case BUK_MT_TR_INTRO:
			{
			CDSU_TRACE (5, "Message: Introduction\n");

			BukMtTrIntro* rcvMsg = (BukMtTrIntro *)msg;

			int mId = rcvMsg->mId;
			int tId = getTraineeId();

			int state = getState();

			if (state == TRAINEE_REGISTERED)
			{
				CDSU_TRACE1 (5, "Trainee %d: in REGISTERED state\n", tId);

				CDSU_TRACE2 (5, "Trainee %d: Receieved Introduction from Mentor %d\n", tId, mId);

				setState (TRAINEE_TRAINING);
				CDSU_TRACE1 (5, "Trainee %d: changed state to TRAINING\n", tId);

				BukTrMtIntroRply* sndMsg = (BukTrMtIntroRply *) cdSuGetMsgBuf (sizeof(BukTrMtIntroRply));
				sndMsg->initMsg(mentorMid, traineeMid, mId);

				CDSU_TRACE1 (1,"Trainee %d: Sending Intro Reply Message\n", tId);

				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE1 (1, "Trainee %d: Error sending message\n", tId);
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}

				//start periodic report timer with callback function
				//on expiry of this timer trainee[tId] will send periodice report msg to mentor
				startPeriodicReportTimer(tId);
			} else
			{
				CDSU_TRACE2 (5, "Trainee %d can't receive intro reply in %d state\n", mId, state);
			}
			} //case BUK_MT_TR_INTRO
			break;
		case BUK_MT_TR_RPRTRQST:
			{
			CDSU_TRACE (5, "Message: Report Request\n");

			int tId = getTraineeId();
			int state = getState();

			if (state == TRAINEE_TRAINING)
			{
				CDSU_TRACE1 (5, "Trainee %d: in TRAINING state\n", tId);

				int mId = getMentorId();

				BukTrMtRprt* sndMsg = (BukTrMtRprt *) cdSuGetMsgBuf (sizeof(BukTrMtRprt));
				sndMsg->initMsg(mentorMid, traineeMid, mId);

				CDSU_TRACE1 (1,"Trainee %d: Sending Report...\n", tId);

				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE1 (1, "Trainee %d: Error sending message\n", tId);
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			} else
			{
				CDSU_TRACE2 (5, "Trainee %d can't receive report request in %d state\n", tId, state);
			}
			} //case BUK_MT_TR_RPRTRQST
			break;
		case BUK_MT_TR_GDNCRPLY:
			{
			CDSU_TRACE (5, "Message: Guidance Reply\n");

			int tId = getTraineeId();
			int state = getState();

			if (state == TRAINEE_TRAINING)
			{
				CDSU_TRACE1 (5, "Trainee %d: in TRAINING state\n", tId);
			} else
			{
				CDSU_TRACE2 (5, "Trainee %d can't receive guidance msg in %d state\n", tId, state);
			}
			} //case BUK_MT_TR_GDNCRPLY
			break;
		case BUK_MT_TR_GDNCRJCT:
			{
			CDSU_TRACE (5, "Message: Guidance Reject\n");

			int tId = getTraineeId();
			int state = getState();

			if (state == TRAINEE_TRAINING)
			{
				CDSU_TRACE1 (5, "Trainee %d: in TRAINING state\n", tId);
			} else
			{
				CDSU_TRACE2 (5, "Trainee %d can't receive guidance reject message in %d state\n", tId, state);
			}
			} //case BUK_MT_TR_GDNCRJCT
			break;
		default:
			CDSU_TRACE (5, "Wrong Mentor Msg Opcode\n");
			break;
		}	//switch

	} else if (src == MYTRAINEEMENU)
	{
//		CDSU_TRACE (5, "Some Request Msg from MyTraineeMenu processing...\n");
//		int reqId = (long) arg; // rohitm
//		int resId = reqId + 1;
//		CDSU_TRACE1 (5, "req Id = %d\n", reqId);
//		BukTrMyTraineeMenuRes* sndMsg = (BukTrMyTraineeMenuRes *) cdSuGetMsgBuf (sizeof(BukTrMyTraineeMenuRes));
//		sndMsg->initMsg(myTraineeMenuMid, traineeMid, resId);
//		CDSU_TRACE (1,"Response Msg Sending to MyTraineeMenu...\n");
//		if (cdSuMdSendMsg (sndMsg) == false)
//		{
//			CDSU_TRACE (1, "Trainee: Error sending message\n");
//			cdSuFreeMsgBuf (sndMsg);
//		} else
//		{
//			CDSU_TRACE (1, "Sent message\n");
//		}

		switch (msg->msgHdr.opcode)
		{
		case BUK_MYTRAINEEMENU_TR_REQ:
			{
			CDSU_TRACE (5, "Some Request Msg from MyTraineeMenu processing...\n");

			//int tId = (int) arg;
			int reqId = (long) arg; // rohitm
			int resId = reqId + 1;
			CDSU_TRACE1 (5, "req Id = %d\n", reqId);
			BukTrMyTraineeMenuRes* sndMsg = (BukTrMyTraineeMenuRes *) cdSuGetMsgBuf (sizeof(BukTrMyTraineeMenuRes));
			sndMsg->initMsg(myTraineeMenuMid, traineeMid, resId);
			CDSU_TRACE (1,"Response Msg Sending to MyTraineeMenu...\n");
			if (cdSuMdSendMsg (sndMsg) == false)
			{
				CDSU_TRACE (1, "Trainee: Error sending message\n");
				cdSuFreeMsgBuf (sndMsg);
			}else
			{
				CDSU_TRACE (1, "Sent message\n");
			}
			} //case BUK_MYTRAINEEMENU_TR_REQ
			break;
		default:
			CDSU_TRACE (5, "Wrong Mentor Msg Opcode\n");
			break;
		}	//switch
	}
	else
	{
		CDSU_TRACE (5, "Wrong Source\n");
	}
}

int Trainee::getState ()
{
	return state;
}


void Trainee::setState (int rcvdState)
{
	state = rcvdState;
}


int Trainee::getTraineeId ()
{
	return trId;
}

void Trainee::setTraineeId (int tid)
{
	trId = tid;
}

void Trainee::setMentorId (int mid)
{
	mtId = mid;
}

int Trainee::getMentorId ()
{
	return mtId;
}

Trainee::Trainee()
{
}

Trainee::~Trainee()
{
}


void* timerThreadEntry (void*)
{
    // NOTE: timer create/start and sigwait should be in the same thread in linux

	// start timer
	itimerval itval, otval;
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_usec = 10 * 1000;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_usec = 10 * 1000;

	if (setitimer (ITIMER_REAL, &itval, &otval) < 0)
	{
		CDSU_TRACE (1, "setitimer error\n");
	}

	while (1)
	{
		sigset_t set;
		// wait for SIGALRM
		sigemptyset (&set);
		sigaddset (&set, SIGALRM);
		int signo;

		int retVal;
		do
		{
			retVal = sigwait (&set, &signo);
			if (retVal == 0)
			{
				sigalrmHandler (signo);
			}

		} while (retVal == -1 &&  errno == EINTR);

	}
	return NULL;	// placate compiler

} // timerThreadEntry ()

void sigalrmHandler (int xx)
{
	// Provides System Ticks to Timer Handler.
	timerHandler->timerTickHandler();
} // sigalrmHandler ()


//============================================================================
// End of bukMsgdistrTrainee.cpp
//============================================================================
