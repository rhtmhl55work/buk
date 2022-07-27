//==============================================================================
// BUK assignment - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	    : bukMsgdistrMentor.cpp
// Description	:
// Ref Docs	    :
// Author	    : Trainee - 2013
// Created On   : 07.11.2013
// Last Modified:
//==============================================================================

#include "bukMentor.h"

//get module id
CdModuleId mentorMid = getApplMidMentor();
CdModuleId adminMid = getApplMidAdmin();
CdModuleId traineeMid = getApplMidTrainee();
CdSuActorId nhMentorMid = getNhMidMentor();

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
			BukUiMtMsg* msg = (BukUiMtMsg *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;

			switch (msg->opcode)
			{
			case BUK_UI_MT_REG:
				{
				CdMessage* sndMsg = (CdMessage *) cdSuGetMsgBuf (sizeof (CdMessage));
				sndMsg->initMsg (mentorMid, nhMentorMid, BUK_UI_MT_REG, sizeof (BUK_UI_MT_REG));
				CDSU_TRACE (1,"NH: Registration Request Message processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH:Sent message\n");
				}
				}
				break;
			case BUK_UI_MT_RPRTRQST:
				{
				BukUiMtRprtRqst* rcvMsg = (BukUiMtRprtRqst *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
				BukMtRprtRqst* sndMsg = (BukMtRprtRqst *) cdSuGetMsgBuf (sizeof (BukMtRprtRqst));
				sndMsg->initMsg (mentorMid, nhMentorMid, rcvMsg->mId);
				CDSU_TRACE (1,"NH:Report Request Message processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH:Sent message\n");
				}
				}
				break;
			case BUK_UI_MT_EXIT:
				{
					exit (0);
				}
				break;
			default:
				CDSU_TRACE (1, "wrong opcode\n");
				break;
			}//switch (msg->opcode)

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
} // applEventHlr ()


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


//---------------------------------------------------------------------------
// applicationStart()
// Description  : First application thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

void* MentorMain::applicationStart(void* dummy1)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiInitParam initParam;

	//--------------------------------------------------------------------
	// First Instance : Management Intf is Callback.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhMentorMid;
	initParam.smActorId = mentorMid;
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
	addUdpPortReqParam.applActorId	= mentorMid;
	addUdpPortReqParam.applEventHlr = applEventHlr;
	addUdpPortReqParam.localPortNo = localPortOfBothFixedMentor;
	addUdpPortReqParam.destPortNo = remotePortOfBothFixedMentor;
	addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);

	// Add Udp Port
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, 	portKey);

	// starting message distributer
	mentorMain->msgDistr();

}// applicationStart

void MentorMain::msgDistr()
{
	CdMessage* rxMsg;		// Any Message that the Application receives.
	while (1)
	{
		if (cdSuMdRecvMsg (&rxMsg, mentorMid) == true)
		{
			CDSU_TRACE (5, "\nMessage Received in MsgDistrMentor application\n");

			if (rxMsg->msgHdr.src == nhMentorMid)
			{
				CDSU_TRACE(5, "Message Source: Mentor Menu\n");
				msgFromMenu(rxMsg);
			}

			if (rxMsg->msgHdr.src == adminMid)
			{
				CDSU_TRACE(5, "Message Source: Admin\n");
				msgFromAdmin(rxMsg);
			}

			if (rxMsg->msgHdr.src == traineeMid)
			{
				CDSU_TRACE(5, "Message Source: Trainee\n");
				msgFromTrainee(rxMsg);
			}

			cdSuFreeMsgBuf (rxMsg);

			// Exits Thread
		}
	} // while loop
} //msgDistr()


void MentorMain::msgFromMenu(CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	case BUK_UI_MT_REG:
		{
		CDSU_TRACE (5, "Message: Registration Request\n");
		if (startState == MENTOR_INIT)
		{
			CDSU_TRACE (5, "Mentor: in INIT state\n");

			int mentorId = ++noOfMentors;
			if (mentorId < MAX_MENTOR)
			{
				mentor[mentorId].handleMsg(msg, MENU, (void *)mentorId);
			} else
			{
				CDSU_TRACE (5, "Sorry! No More Registration\n");
			}
		} else
		{
			CDSU_TRACE1 (5, "Mentor can't send registration request in %d state\n", startState);
		}
		} //case BUK_UI_MT_REG
		break;
	case BUK_UI_MT_RPRTRQST:
		{
		CDSU_TRACE (5, "Message: Report Request\n");

		BukMtRprtRqst* rcvMsg = (BukMtRprtRqst *)msg;

		int mId = rcvMsg->mId;
		CDSU_TRACE1 (5, "Report Request Message from %d\n", mId);

		mentor[mId].handleMsg(msg, MENU, NULL);

		} //case BUK_UI_MT_RPRTRQST
		break;
	default:
		CDSU_TRACE (5, "Wrong Menu Msg Opcode\n");
		break;
	}
} //msgFromMenu()


void MentorMain::msgFromAdmin(CdMessage* msg)
{
	BukAdMtMsgs* rcvMsg = (BukAdMtMsgs *)msg;

	int mId = rcvMsg->mId;
	mentor[mId].handleMsg (msg, ADMIN, NULL);

} //msgFromAdmin()


void MentorMain::msgFromTrainee(CdMessage* msg)
{
	BukTrMtMsgs* rcvMsg = (BukTrMtMsgs *)msg;

	int mId = rcvMsg->mId;

	mentor[mId].handleMsg(msg, TRAINEE, NULL);

} //msgFromTrainee()


void Mentor::startWorkTimer (int i)
{
	int back;
	CdSuTimerInfo timerInfo1;

	timerInfo1.timeCounts = 15;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId = mentorMid;
	timerInfo1.opcode = 12;
	timerInfo1.timerType = CDSU_TMR_TT_TRIGGER_ONCE;
	timerInfo1.timeoutReportType = CDSU_TMR_TRT_CALLBACK;
	timerInfo1.timerCallBackFn = workTimerExpires;
	timerInfo1.userInfo[0] = i;

	back = timerHandler->startTimer(workTimerId[i], timerInfo1);
	if (back == CDSU_TMR_RV_INV_TMRID)
	{
		printf ("Work Timer NOT started\n");
	} else if (back == CDSU_TMR_RV_SUCCESS)
	{
		printf ("Work Timer started\n");
	}
} //startWorkTimer()


void Mentor::startPeriodicReportTimer (int i)
{
	int back;
	CdSuTimerInfo timerInfo1;

	timerInfo1.timeCounts = 20;
	timerInfo1.timeUnits = CDSU_TMR_TU_SECOND;
	timerInfo1.destModuleId = mentorMid;
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
} //startPeriodicReportTimer()


void Mentor::timerReset()
{
	// get mentor id to get report timer id
	int id = getMentorId();

	//stopping the periodic report timer
	int back1 = timerHandler->stopTimer(reportTimerId[id]);

	if (back1 != 0)
	{
		printf("StopTimer Failed\n");
	} else
	{
		printf("Timer stopped\n");
	}

	//starting the timer again
	startPeriodicReportTimer(id);
} //timerReset()

void workTimerExpires(CdSuTimeout& timeout)
{
	int mId = timeout.userInfo[0];
	CDSU_TRACE1 (5, "Mentor %d: Work Completed\n", mId);

	int type = 1;		//type 1 indicates work timer
	mentor[mId].handleTimer(type);
} //workTimerExpires()

void periodicReportTimerExpires(CdSuTimeout& timeout)
{
	int mId = timeout.userInfo[0];
	CDSU_TRACE1 (5, "Mentor %d: Periodic Report Timer Expires\n", mId);

	int type = 2;		//type 2 indicates periodic report timer
	mentor[mId].handleTimer(type);
} //periodicReportTimerExpires()

void Mentor::handleTimer(int type)
{
	int mId = getMentorId();
	int state = getState();

	if (type == 1)
	{
		if (state == MENTOR_WORKING)
		{
			CDSU_TRACE1 (5, "Mentor %d: In WORKING state\n", mId);
			setState(MENTOR_REGISTERED);
			CDSU_TRACE1 (5, "Mentor %d: changed state to REGISTERED\n", mId);
		} else if (state == MENTOR_WORKING_TRAINING)
		{
			CDSU_TRACE1 (5, "Mentor %d: In WORKING_TRAINING state\n", mId);
			if (trCmpltFlag == 0)
			{
				CDSU_TRACE1 (5, "Mentor %d: Training Not Completed\n", mId);
				setState(MENTOR_TRAINING);
				CDSU_TRACE1 (5, "Mentor %d: changed state to TRAINING\n", mId);
			} else if (trCmpltFlag == 1)
			{
				CDSU_TRACE1 (5, "Mentor %d: Training Completed\n", mId);

				BukMtAdTrCmpltAck* sndMsg = (BukMtAdTrCmpltAck *) cdSuGetMsgBuf (sizeof(BukMtAdTrCmpltAck));
				sndMsg->initMsg(adminMid, mentorMid, mId);

				CDSU_TRACE1 (1,"Mentor %d: Training Complete Ack Sending to Admin...\n", mId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE1 (1, "Mentor %d: Error sending message\n", mId);
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}

				setState(MENTOR_TERMINATED);
				CDSU_TRACE1 (5, "Mentor %d: changed state to TERMINATED\n", mId);

				totalAck--;
				if (totalAck == 0)
				{
					exit(0);
				}
			}
		} else
		{
			CDSU_TRACE (5, "Wrong state\n");
			timerHandler->stopTimer(workTimerId[mId]);
		}
	} else if (type == 2)
	{
		if (state == MENTOR_TRAINING || state == MENTOR_WORKING_TRAINING)
		{
			CDSU_TRACE1 (5, "Mentor %d: In TRAINING or WORKING_TRAINING state\n", mId);

			if (count >= MAX_LOST_REPORT)
			{
				BukMtAdTrLost* sndMsg = (BukMtAdTrLost *) cdSuGetMsgBuf (sizeof (BukMtAdTrLost));
				sndMsg->initMsg (adminMid, mentorMid, mId);
				CDSU_TRACE (1,"Trainee Lost Message Sending to Admin...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Mentor: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
				timerHandler->stopTimer(reportTimerId[mId]);
			} else
			{
				count++;
			}
		} else
		{
			CDSU_TRACE (5, "Wrong state\n");
			timerHandler->stopTimer(reportTimerId[mId]);
		}
	}
}


void Mentor::handleMsg (CdMessage* msg, int src, void* arg)
{
	if (src == MENU)
	{
		switch (msg->msgHdr.opcode)
		{
		case BUK_UI_MT_REG:
			{
			CDSU_TRACE (5, "Registration Request Processing...\n");
			//int mentorId = (int)arg;
			int mentorId = (long)arg; // rohitm

			setMentorId(mentorId);
			setState(MENTOR_INIT);

			CDSU_TRACE1 (5, "New Mentor Id = %d\n", mentorId);

			BukMtAdReg* sndMsg = (BukMtAdReg *) cdSuGetMsgBuf (sizeof(BukMtAdReg));
			sndMsg->initMsg(adminMid, mentorMid, mentorId);
			CDSU_TRACE (1,"Registration Request Sending to Admin...\n");
			if (cdSuMdSendMsg (sndMsg) == false)
			{
				CDSU_TRACE (1, "Mentor: Error sending message\n");
				cdSuFreeMsgBuf (sndMsg);
			}else
			{
				CDSU_TRACE (1, "Sent message\n");
			}
			}
			break;
		case BUK_UI_MT_RPRTRQST:
			{
			CDSU_TRACE (5, "Report Request Message Processing...\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_TRAINING)
			{
				CDSU_TRACE1 (5, "Mentor %d in TRAINING state\n", mId);

				int traineeId = getTraineeId();

				BukMtTrRprtRqst* sndMsg = (BukMtTrRprtRqst *) cdSuGetMsgBuf (sizeof(BukMtTrRprtRqst));
				sndMsg->initMsg(traineeMid, mentorMid, traineeId);
				CDSU_TRACE1 (1,"Report Request Message Sending to Trainee %d...\n",traineeId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Mentor: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}

			} else
			{
				if (mId == 0)
				{
					CDSU_TRACE (5, "Mentor not Registered\n");
				} else
				{
					CDSU_TRACE2 (5, "Mentor %d can't sent report request in %d state\n", mId, state);
				}
			}
			}
			break;
		}
	} else if (src == ADMIN)
	{
		switch (msg->msgHdr.opcode)
		{
		case BUK_AD_MT_REGACK:
			{
			CDSU_TRACE (5, "Message: Registration Ack\n");

			int mId = getMentorId();
			int state = getState();
			if (state == MENTOR_INIT)
			{
				CDSU_TRACE1 (5, "Mentor %d: in INIT state\n", mId);
				setState(MENTOR_REGISTERED);
				CDSU_TRACE1 (5, "Mentor %d: changed state to REGISTERED\n", mId);
			} else
			{
				CDSU_TRACE1 (5, "Mentor %d in wrong state\n", mId);
			}
			} //case BUK_AD_MT_REGACK
			break;
		case BUK_AD_MT_REGNACK:
			{
			CDSU_TRACE (5, "Message: Registration Nack\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_INIT)
			{
				CDSU_TRACE1 (5, "Mentor %d: in INIT state\n", mId);
				setState(MENTOR_TERMINATED);
				CDSU_TRACE1 (5, "Mentor %d: changed state to TERMINATED\n", mId);
			} else
			{
				CDSU_TRACE1 (5, "Mentor %d in wrong state\n", mId);
			}
			} //case BUK_AD_MT_REGNACK
			break;
		case BUK_AD_MT_WRK:
			{
			CDSU_TRACE (5, "Message: Work Request\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_REGISTERED)
			{
				CDSU_TRACE1 (5, "Mentor %d: in REGISTERED state\n", mId);

				setState(MENTOR_WORKING);
				CDSU_TRACE1 (5, "Mentor %d: changed state to WORKING\n", mId);

				// start work timer with timer expiry call back function
				// call back function: change states accordingly
				startWorkTimer(mId);

			} else if (state == MENTOR_TRAINING)
			{
				CDSU_TRACE1 (5, "Mentor %d: in TRAINING state\n", mId);

				setState(MENTOR_WORKING_TRAINING);
				CDSU_TRACE1 (5, "Mentor %d: changed state to WORKING_TRAINING\n", mId);

				// start work timer with timer expiry call back function
				// call back function: change states accordingly
				startWorkTimer(mId);
			}
			else
			{
				CDSU_TRACE (5, "Work Request at Wrong state\n");
			}
			} //case BUK_AD_MT_WRK
			break;
		case BUK_AD_MT_TRID:
			{
			CDSU_TRACE (5, "Message: Trainee Id\n");

			BukAdMtTid* rcvMsg = (BukAdMtTid *)msg;
			int tId = rcvMsg->tId;

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_REGISTERED || state == MENTOR_WORKING)
			{

				CDSU_TRACE1 (5, "Mentor %d: in REGISTERED or WORKING state\n", mId);
				totalAck++;

				// no more registration accepted from menu
				startState = MENTOR_REGISTERED;

				//set trainee id
				setTraineeId(tId);

				BukMtTrIntro* sndMsg = (BukMtTrIntro *) cdSuGetMsgBuf (sizeof(BukMtTrIntro));
				sndMsg->initMsg(traineeMid, mentorMid, mId, tId);
				CDSU_TRACE1 (1,"Introduction message Sending to Trainee %d...\n", tId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE1 (1, "Mentor %d: Error sending message\n", mId);
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			}  else
			{
				CDSU_TRACE (5, "Start Training Request in wrong state\n");
			}
			} //case BUK_AD_MT_TRID
			break;
		case BUK_AD_MT_TRCMPLT:
			{
				CDSU_TRACE (5, "Message: Training Complete\n");

				int mId = getMentorId();
				int state = getState();
				int back = timerHandler->stopTimer(reportTimerId[mId]);
				if (back != 0)
				{
					printf("StopTimer Failed for Id :%d\n",reportTimerId[mId]);
				} else
				{
					printf("Timer stopped, id :%d\n",reportTimerId[mId]);
				}

				if (state == MENTOR_TRAINING)
				{
					CDSU_TRACE1 (5, "Mentor %d: In TRAINING state\n", mId);

					BukMtAdTrCmpltAck* sndMsg = (BukMtAdTrCmpltAck *) cdSuGetMsgBuf (sizeof(BukMtAdTrCmpltAck));
					sndMsg->initMsg(adminMid, mentorMid, mId);

					CDSU_TRACE1 (1,"Mentor %d: Training Complete Ack Sending to Admin...\n", mId);
					if (cdSuMdSendMsg (sndMsg) == false)
					{
						CDSU_TRACE1 (1, "Mentor %d: Error sending message\n", mId);
						cdSuFreeMsgBuf (sndMsg);
					} else
					{
						CDSU_TRACE (1, "Sent message\n");
					}

					CDSU_TRACE1 (5, "Mentor %d: in Training state\n", mId);
					setState(MENTOR_TERMINATED);
					CDSU_TRACE1 (5, "Mentor %d: changed state to TERMINATED\n", mId);

					totalAck--;
					if (totalAck == 0)
					{
						exit(0);
					}
				} else if ( state == MENTOR_WORKING_TRAINING)
				{
					CDSU_TRACE1 (5, "Mentor %d: In WORKING_TRAINING state\n", mId);
					trCmpltFlag = 1;
				} else
				{
					CDSU_TRACE (5, "Stop Training Request in wrong state\n");
				}
			} //case BUK_AD_MT_TRCMPLT
			break;
		default:
			CDSU_TRACE (5, "Wrong Mentor Message Opcode\n");
			break;
		}	//switch
	} else if (src == TRAINEE)
	{
		switch (msg->msgHdr.opcode)
		{
		case BUK_TR_MT_INTRORPLY:
			{
			CDSU_TRACE (5, "Message: Introduction Reply\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_REGISTERED || state == MENTOR_WORKING)
			{
				CDSU_TRACE1 (5, "Mentor %d: Receieved Introduction Reply\n", mId);

				if (state == MENTOR_REGISTERED)
				{
					CDSU_TRACE1 (5, "Mentor %d: In REGISTERED state\n", mId);
					setState (MENTOR_TRAINING);
					CDSU_TRACE1 (5, "Mentor %d: changed state to TRAINING\n", mId);
				} else
				{
					CDSU_TRACE1 (5, "Mentor %d: In WORKING state\n", mId);
					setState (MENTOR_WORKING_TRAINING);
					CDSU_TRACE1 (5, "Mentor %d: changed state to WORKING_TRAINING\n", mId);
				}

				//start periodic report timer with callback function
				//on n times expiry of this timer mentor[mId] will send trLost msg to admin
				startPeriodicReportTimer(mId);
			} else
			{
				CDSU_TRACE2 (5, "Mentor %d can't receive intro reply in %d state\n", mId, state);
			}
			} //case BUK_TR_MT_INTROREPLY
			break;
		case BUK_TR_MT_PRDCRPRT:
			{
			CDSU_TRACE (5, "Message: Periodic Report\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_TRAINING || state == MENTOR_WORKING_TRAINING)
			{
				CDSU_TRACE (5, "Mentor in TRAINING or WORKING_TRAINING state\n");

				// periodic report timer reset (stop and start)
				timerReset();
			} else
			{
				CDSU_TRACE2 (5, "Mentor %d can't receive periodic report in %d state\n", mId, state);
			}
			} //case BUK_TR_MT_PRDCRPRT
			break;
		case BUK_TR_MT_RPRT:
			{
			CDSU_TRACE (5, "Message: Report\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_TRAINING || state == MENTOR_WORKING_TRAINING)
			{
				CDSU_TRACE1 (5, "Mentor %d: in TRAINING or WORKING_TRAINING state\n", mId);
			} else
			{
				CDSU_TRACE2 (5, "Mentor %d: can't receive report in %d state\n", mId, state);
			}
			} //case BUK_TR_MT_RPRT
			break;
		case BUK_TR_MT_GDNCRQST:
			{
			CDSU_TRACE (5, "Message: Guidance Request\n");

			int mId = getMentorId();
			int state = getState();

			if (state == MENTOR_TRAINING || state == MENTOR_WORKING_TRAINING)
			{
				int tId = getTraineeId();

				if (state == MENTOR_TRAINING)
				{
					CDSU_TRACE1 (5, "Mentor %d: in TRAINING state\n", mId);
					BukMtTrGdncRply* sndMsg = (BukMtTrGdncRply *) cdSuGetMsgBuf (sizeof(BukMtTrGdncRply));
					sndMsg->initMsg(traineeMid, mentorMid, tId);

					CDSU_TRACE1 (1,"Mentor %d: Guidance Reply Message Sending...\n", mId);

					if (cdSuMdSendMsg (sndMsg) == false)
					{
						CDSU_TRACE1 (1, "Mentor %d: Error sending message\n", mId);
						cdSuFreeMsgBuf (sndMsg);
					} else
					{
						CDSU_TRACE (1, "Sent message\n");
					}
				} else
				{
					CDSU_TRACE1 (5, "Mentor %d: in WORKING_TRAINING state\n", mId);
					BukMtTrGdncRjct* sndMsg = (BukMtTrGdncRjct *) cdSuGetMsgBuf (sizeof(BukMtTrGdncRjct));
					sndMsg->initMsg(traineeMid, mentorMid, tId);

					CDSU_TRACE1 (1,"Mentor %d: Guidance Reject Message Sending...\n", mId);

					if (cdSuMdSendMsg (sndMsg) == false)
					{
						CDSU_TRACE1 (1, "Mentor %d: Error sending message\n", mId);
						cdSuFreeMsgBuf (sndMsg);
					} else
					{
						CDSU_TRACE (1, "Sent message\n");
					}
				}
			} else
			{
				CDSU_TRACE2 (5, "Mentor %d can't receive guidance request message in %d state\n", mId, state);
			}
			} //case BUK_TR_MT_GDNCRQST
			break;
		default:
			CDSU_TRACE (5, "Wrong Trainee Message Opcode\n");
			break;
		}	//switch
	} else
	{
		CDSU_TRACE (5, "Wrong Message Source\n");
	}
} //handleMsg()


int Mentor::getState ()
{
	return state;
}

void Mentor::setState (int rcvdState)
{
	state = rcvdState;
}

int Mentor::getTraineeId ()
{
	return trId;
}

void Mentor::setTraineeId (int tid)
{
	trId = tid;
}

void Mentor::setMentorId (int mid)
{
	mtId = mid;
}

int Mentor::getMentorId ()
{
	return mtId;
}

Mentor::Mentor()
{
}

Mentor::~Mentor()
{
}

//============================================================================
// End of bukMsgdistrMentor.cpp
//============================================================================
