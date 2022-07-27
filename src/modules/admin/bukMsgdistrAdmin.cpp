//==============================================================================
// BUK assignment - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	    : bukMsgdistrAdmin.cpp
// Description	:
// Ref Docs	    :
// Author	    : Trainee - 2013
// Created On   : 07.11.2013
// Last Modified:
//==============================================================================

#include "bukAdmin.h"

//get module id
CdModuleId adminMid = getApplMidAdmin();
CdModuleId mentorMid = getApplMidMentor();
CdModuleId traineeMid = getApplMidTrainee();
CdSuActorId nhAdminMid = getNhMidAdmin();

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

            // message received from admin menu
			BukUiAdMsg* msg = (BukUiAdMsg *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;

			switch (msg->opcode)
			{
			case BUK_UI_AD_STRTRNG:
				{
				CdMessage* sndMsg = (CdMessage *) cdSuGetMsgBuf (sizeof (CdMessage));
				sndMsg->initMsg (adminMid, nhAdminMid, BUK_UI_AD_STRTRNG, sizeof (BUK_UI_AD_STRTRNG));
				CDSU_TRACE (1,"START TRAINING Message processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH: Sent message to MD\n");
				}
				}
				break;
			case BUK_UI_AD_STPTRNG:
				{
				CdMessage* sndMsg = (CdMessage *) cdSuGetMsgBuf (sizeof (CdMessage));
				sndMsg->initMsg (adminMid, nhAdminMid, BUK_UI_AD_STPTRNG, sizeof (BUK_UI_AD_STPTRNG));
				CDSU_TRACE (1,"STOP TRAINING Message  processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH: Sent message to MD\n");
				}
				}
				break;
			case BUK_UI_AD_WRKTOMT:
				{
				BukUiAdWrk* rcvMsg = (BukUiAdWrk *)((CdSuNhUdpUiDataIndEvent*)eventParam)->dataPtr;
				BukAdWrktoMtMsg* sndMsg = (BukAdWrktoMtMsg *) cdSuGetMsgBuf (sizeof(BukAdWrktoMtMsg));
				sndMsg->initMsg (adminMid, nhAdminMid, rcvMsg->mId);

				CDSU_TRACE (1,"WORK TO MENTOR Message  processing...\n");
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "NH: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "NH: Sent message to MD\n");
				}
				}
				break;
			case BUK_UI_AD_EXIT:
				{
				exit (0);
				}
				break;
			default:
				CDSU_TRACE (1, "wrong opcode\n");
				break;
			}	//switch (msg->opcode)
			//}
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

//---------------------------------------------------------------------------
// applicationStart ()
// Description  : Application thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

void* AdminMain::applicationStart (void* dummy1)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhUdpMiInitParam initParam;

	//--------------------------------------------------------------------
	// 1.First Instance : Management Intf is Callback.
	//--------------------------------------------------------------------
	initParam.nhActorId = nhAdminMid;
	initParam.smActorId = adminMid;
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
	addUdpPortReqParam.applActorId	= adminMid;
	addUdpPortReqParam.applEventHlr = applEventHlr;
	addUdpPortReqParam.localPortNo = localPortOfBothFixedAdmin;
	addUdpPortReqParam.destPortNo = remotePortOfBothFixedAdmin;
	addUdpPortReqParam.localAddr =	cdSuNhUdpConvIp2Int (localIpAddr);
	addUdpPortReqParam.destAddr = cdSuNhUdpConvIp2Int (remoteIpAddr);

	// Add Udp Port
	cdSuNhUdpUiAddUdpPortReq (addUdpPortReqParam, 	portKey);

	//start message distributer for receiving all messages
	adminMain->msgDistr();

}// applicationStart

// Handles the messages received from menu
void Admin::msgFromMenu(CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	case BUK_UI_AD_STRTRNG:
		{
		CDSU_TRACE (5, "Start Training Message processing...\n");
		int state = getState();
		if (state == ADMIN_INIT)
		{
			CDSU_TRACE (5, "Admin: in INIT state\n");

			//if the number of mentors is same as number of trainees
			//then map mentor-trainee and send message to start the training
            int NoOfMentors = queueMr.getSize();
			int NoOfTrainees = queueTr.getSize();

			CDSU_TRACE1 (5, "Number of Mentors = %d\n", NoOfMentors);
			CDSU_TRACE1 (5, "Number of Trainees = %d\n", NoOfTrainees);

			if (NoOfTrainees > 0 && NoOfMentors > 0)
			{
				if (NoOfMentors == NoOfTrainees)
				{
					//map mentor-trainee
					mapMtTrFunction();

					//send trId to corresponding mentor and vice versa
					sendMapping();

					totalAck = NoOfMentors + NoOfTrainees;

					setState (ADMIN_MONITOR);
					CDSU_TRACE (5, "Admin: changed state to MONITOR\n");
				} else
				{
					CDSU_TRACE (5, "Training not started\n");
					CDSU_TRACE (5, "Number of mentors are not equal to Number of Trainees\n");
				}
			} else
			{
				CDSU_TRACE (5, "Training not started\n");
				CDSU_TRACE (5, "No Mentor or Trainee\n");
			}
		} else
		{
			CDSU_TRACE1 (5, "Admin can't start training in %d state\n",state);
		}
		} //case BUK_UI_AD_STRTRNG
		break;
	case BUK_UI_AD_STPTRNG:
		{
		CDSU_TRACE (5, "Stop Training Message processing...\n");
		int state = getState();
		if (state == ADMIN_MONITOR)
		{
			CDSU_TRACE (5, "Admin: in MONITOR state\n");

			//go to wait_terminated state
			setState (ADMIN_WAIT_TERMINATED);
			CDSU_TRACE (5, "Admin: changed state to WAIT_TERMINATED\n");

			//send trCmplt msg to all mentors
			for (iter1 = queueMr.begin (); iter1 != queueMr.end (); ++iter1)
			{
				int mid = *iter1;
				CDSU_TRACE1 (5, "Sending Training Complete message to mentor %d \n", mid);
				BukAdMtTrCmplt* sndMsg = (BukAdMtTrCmplt *) cdSuGetMsgBuf (sizeof(BukAdMtTrCmplt));
				sndMsg->initMsg(mentorMid, adminMid, mid);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			}

			//send trCmplt msg to all trainees
			for (iter1 = queueTr.begin (); iter1 != queueTr.end (); ++iter1)
			{
				int tid = *iter1;
				CDSU_TRACE1 (5, "Sending Training Complete message to trainee %d \n", tid);
				BukAdTrTrCmplt* sndMsg2 = (BukAdTrTrCmplt *) cdSuGetMsgBuf (sizeof(BukAdTrTrCmplt));
				sndMsg2->initMsg(traineeMid, adminMid, tid);
				if (cdSuMdSendMsg (sndMsg2) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg2);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			}
		} else
		{
			CDSU_TRACE1 (5, "Admin can't stop training in %d state\n",state);
		}
		} //case BUK_UI_AD_STPTRNG
		break;
	case BUK_UI_AD_WRKTOMT:
		{
		CDSU_TRACE (5, "Message: Work to Mentor Message Arrived\n");
		int state = getState();
		if (state == ADMIN_INIT || state == ADMIN_MONITOR)
		{
			CDSU_TRACE1 (5, "Admin: in %d state\n", state);

			BukAdWrktoMtMsg* rcvMsg = (BukAdWrktoMtMsg *)msg;

			int mentorId = rcvMsg->mId;

			CDSU_TRACE1 (5, "Work to be allocated to Mentor %d\n", mentorId);

			int mentorPresent = 0;

			for (iter1 = queueMr.begin (); iter1 != queueMr.end (); ++iter1 )
			{
				if (*iter1 == mentorId)
				{
					mentorPresent = 1;
				}
			}

			if (mentorPresent == 1)
			{
				//send work msg to mentor
				BukAdMtWrk* sndMsg = (BukAdMtWrk *) cdSuGetMsgBuf (sizeof(BukAdMtWrk));
				sndMsg->initMsg(mentorMid, adminMid, mentorId);
				CDSU_TRACE1 (1, "ALLOCATE WORK Message Sending to Mentor %d\n", mentorId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			} else
			{
				CDSU_TRACE1 (5, "Mentor %d is not registered\n", mentorId);
			}
		} else
		{
			CDSU_TRACE1 (5, "Admin can't allocate work to mentor in %d state\n", state);
		}
		} //case BUK_UI_AD_WRKTOMT
		break;

	default:
		CDSU_TRACE (5, "Wrong Menu Message Opcode\n");
		break;
	}
}

// function to handle messages from mentor
void Admin::msgFromMentor(CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	case BUK_MT_AD_MTID:
		{
		CDSU_TRACE (5, "Message: Registration Request\n");
		int state = getState();
		if (state == ADMIN_INIT)
		{
			CDSU_TRACE (5, "Admin: in INIT state\n");

			BukMtAdReg* rcvMsg = (BukMtAdReg *)msg;
			int mentorId = rcvMsg->mId;

			int regSuccess;

			//add mentor id in queue
			regSuccess = queueMr.add (mentorId);
			if (regSuccess == true)
			{
				BukAdMtRegAck* sndMsg = (BukAdMtRegAck *) cdSuGetMsgBuf (sizeof(BukAdMtRegAck));
				sndMsg->initMsg(mentorMid, adminMid, mentorId);
				CDSU_TRACE1 (1, "Registration Ack Sending to Mentor %d\n", mentorId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			} else
			{
				BukAdMtRegNack* sndMsg = (BukAdMtRegNack *) cdSuGetMsgBuf (sizeof(BukAdMtRegNack));
				sndMsg->initMsg(mentorMid, adminMid, mentorId);
				CDSU_TRACE1 (1,"Registration Nack Sending to Mentor %d...\n", mentorId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			}
		} else
		{
			CDSU_TRACE1 (5, "Admin can't handle registration request in %d state\n",state);
		}
		} //case BUK_MT_AD_MTID
		break;
	case BUK_MT_AD_TRLOST:
		{
		CDSU_TRACE (5, "Message: Trainee Lost\n");
		int state = getState();
		if (state == ADMIN_MONITOR)
		{
			CDSU_TRACE (5, "Admin: in MONITOR state\n");

			BukMtAdTrLost* rcvMsg = (BukMtAdTrLost *)msg;

			int mentorId = rcvMsg->mId;
			int traineeId;

			intHash.lookup (mentorId, traineeId);

			CDSU_TRACE2 (5, "Admin: %d mentor lost contact with %d trainee\n", mentorId, traineeId);
		} else
		{
			CDSU_TRACE (5, "Admin in different state\n");
		}
		} //case BUK_UI_AD_STPTRNG
		break;
	case BUK_MT_AD_TRCMPLTACK:
		{
		CDSU_TRACE (5, "Message: Training Complete Ack\n");
		int state = getState();
		if (state == ADMIN_WAIT_TERMINATED)
		{
			CDSU_TRACE (5, "Admin: in WAIT_TERMINATED state\n");
			BukMtAdTrCmpltAck* rcvMsg = (BukMtAdTrCmpltAck *)msg;

			int mentorId = rcvMsg->mId;
			int traineeId;

			intHash.lookup (mentorId, traineeId);

			CDSU_TRACE1 (5, "Training complete Ack received from Mentor %d\n", mentorId);

			// delete mapping
			if (intHash.remove(mentorId) == false)
			{
				CDSU_TRACE2 (5, "Admin: Mentor%d-Trainee%d mapping deletion - FAILED\n", mentorId, traineeId);
			} else
			{
				CDSU_TRACE2 (5, "Admin: Mentor%d-Trainee%d mapping deletion - SUCCESS\n", mentorId, traineeId);
			}

			totalAck--;

			// all TrCmplt ack received, admin exiting
			if (totalAck == 0)
			{
				setState(ADMIN_TERMINATED);
				CDSU_TRACE (5, "Admin: changed state to TERMINATED\n");
				CDSU_TRACE (5, "All Training Completed\nAdmin Exiting\n");
				::exit(0);
			}
		} else
		{
			CDSU_TRACE (5, "Wrong state\n");
		}
		} //case BUK_MT_AD_TRCMPLT
		break;
	default:
		CDSU_TRACE (5, "Wrong Mentor Message Opcode\n");
		break;
	}	//switch
} //msgFromMentor

//  function to handle all messages from trainee
void Admin::msgFromTrainee(CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	case BUK_TR_AD_TRID:
		{
		CDSU_TRACE (5, "Message: Registration Request\n");
		int state = getState();
		if (state == ADMIN_INIT)
		{
			CDSU_TRACE (5, "Admin: in INIT state\n");

			BukTrAdReg* rcvMsg = (BukTrAdReg *)msg;
			int traineeId = rcvMsg->tId;

			int regSuccess;

			//add mentor id in queue
			regSuccess = queueTr.add (traineeId);
			if (regSuccess == true)
			{
				BukAdTrRegAck* sndMsg = (BukAdTrRegAck *) cdSuGetMsgBuf (sizeof(BukAdTrRegAck));
				sndMsg->initMsg(traineeMid, adminMid, traineeId);
				CDSU_TRACE1 (1, "Registration Ack Sending to Trainee %d...\n", traineeId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				}else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			} else
			{
				BukAdTrRegNack* sndMsg = (BukAdTrRegNack *) cdSuGetMsgBuf (sizeof(BukAdTrRegNack));
				sndMsg->initMsg(traineeMid, adminMid, traineeId);
				CDSU_TRACE1 (1,"Registration Nack Sending to Trainee %d...\n", traineeId);
				if (cdSuMdSendMsg (sndMsg) == false)
				{
					CDSU_TRACE (1, "Admin: Error sending message\n");
					cdSuFreeMsgBuf (sndMsg);
				} else
				{
					CDSU_TRACE (1, "Sent message\n");
				}
			}
		} else
		{
			CDSU_TRACE1 (5, "Admin can't handle registration request in %d state\n",state);
		}
		} //case BUK_TR_AD_TRID
		break;
	case BUK_TR_AD_TRCMPLTACK:
		{
		CDSU_TRACE (5, "Message: Training Complete Ack\n");
		int state = getState();
		if (state == ADMIN_WAIT_TERMINATED)
		{
			CDSU_TRACE (5, "Admin: in WAIT_TERMINATED state\n");

			BukTrAdTrCmpltAck* rcvMsg = (BukTrAdTrCmpltAck *)msg;

			int traineeId = rcvMsg->tId;

			CDSU_TRACE1 (5, "Training complete Ack received from trainee %d\n",traineeId);

			totalAck--;

			//all training complete Ack received, admin exiting
			if (totalAck == 0)
			{
				setState(ADMIN_TERMINATED);
				CDSU_TRACE (5, "Admin: changed state to TERMINATED\n");
				CDSU_TRACE (5, "All Training Completed\nAdmin Exiting\n");
				::exit(0);
			}
		} else
		{
			CDSU_TRACE (5, "Wrong state\n");
		}
		} //case BUK_TR_AD_TRCMPLT
		break;
	default:
		CDSU_TRACE (5, "Wrong Trainee Message Opcode\n");
		break;
	}	//switch
} //msgFromTrainee

// Admin message distributor
void AdminMain::msgDistr()
{
	CdMessage* rxMsg;

	// Any Message that the Application receives.
	while (1)
	{
		if (cdSuMdRecvMsg (&rxMsg, adminMid) == true)
		{
			CDSU_TRACE (5, "\nMessage Arrived in MsgDistrAdmin application\n");
			admin.handleMsg (rxMsg);
			cdSuFreeMsgBuf (rxMsg);
		}
	} // while loop
}

// function to handle different messages from different sources
void Admin::handleMsg(CdMessage* rxMsg)
{
	if (rxMsg->msgHdr.src == nhAdminMid)
	{
		CDSU_TRACE(5, "Message Source: Admin Menu\n");
		msgFromMenu(rxMsg);
	}

	if (rxMsg->msgHdr.src == mentorMid)
	{
		CDSU_TRACE(5, "Message Source: Mentor\n");
		msgFromMentor(rxMsg);
	}

	if (rxMsg->msgHdr.src == traineeMid)
	{
		CDSU_TRACE(5, "Message Source: Trainee\n");
		msgFromTrainee(rxMsg);
	}
}

// to map mentor-trainee
int Admin::mapMtTrFunction()
{
	int retValTr, retValMr, popValTr, popValMr;

	for (iter1 = queueMr.begin(), iter2 = queueTr.begin(); iter1 != queueMr.end() && iter2 != queueTr.end();)
	{
		popValTr = *iter1;
		popValMr = *iter2;
		//printf("\nprinting %d \t %d \t", popValTr, popValMr);
		intHash.insert (popValTr, popValMr);
		++iter1;
		++iter2;
	}

}

// function to send mapped mentorId to trainee and vice versa
void Admin::sendMapping ()
{
	int val;

	for (iter1 = queueMr.begin (); iter1 != queueMr.end (); ++iter1)
	{
		intHash.lookup (*iter1, val);

		int mentorId = *iter1;
		int traineeId = val;

		CDSU_TRACE2 (5, "\nMentor %d assigned Trainee %d\n", mentorId, traineeId);

		BukAdMtTid* sndMsg1 = (BukAdMtTid *) cdSuGetMsgBuf (sizeof(BukAdMtTid));
		sndMsg1->initMsg(mentorMid, adminMid, mentorId, traineeId);
		CDSU_TRACE (1,"Sending Trainee Id to Mentor...\n");
		if (cdSuMdSendMsg (sndMsg1) == false)
		{
			CDSU_TRACE (1, "Admin: Error sending message\n");
			cdSuFreeMsgBuf (sndMsg1);
		}else
		{
			CDSU_TRACE (1, "Sent message\n");
		}

		BukAdTrMid* sndMsg2 = (BukAdTrMid *) cdSuGetMsgBuf (sizeof(BukAdTrMid));
		sndMsg2->initMsg(traineeMid, adminMid, mentorId, traineeId);
		CDSU_TRACE (1,"Sending Mentor Id to Trainee...\n");
		if (cdSuMdSendMsg (sndMsg2) == false)
		{
			CDSU_TRACE (1, "Admin: Error sending message\n");
			cdSuFreeMsgBuf (sndMsg2);
		}else
		{
			CDSU_TRACE (1, "Sent message\n");
		}
	}
}

// to get state of admin
int Admin::getState ()
{
	return state;
}

// to set state of admin
void Admin::setState (int rcvdState)
{
	state = rcvdState;
}

Admin::Admin()
{
	setState (ADMIN_INIT);
}

Admin::~Admin()
{

}
//============================================================================
// End of bukMsgdistrAdmin.cpp
//============================================================================
