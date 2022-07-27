#ifndef TEST_FUNCTIONS_H_
#define TEST_FUNCTIONS_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testfunctions.h
// Description  : Test Functions
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================

#include <stdio.h>


void cdSetTraceLevel0 ();
void cdSetTraceLevel1 ();
void cdSetTraceLevel2 ();
void cdSetTraceLevel3 ();
void cdSetTraceLevel4 ();
void cdSetTraceLevel5 ();

// TODO: Add test functions here, format void funcName (void)

// Parameter Setting Functions
void initSetup ();
void setInvNhInstId ();
void setInstIdMoreThanMax ();
void setInvSmActorId ();
void setInvApplActorId ();
void setInvNhActorId ();
void setBufLenZero ();
void setBufLenMoreThanMax ();
void setSelTimerZero ();
void setSelTimerMoreThanMax ();
void setNoHlrTimerZero ();
void setNoHlrTimerMoreThanMax ();
void setNoOfUdpPortMoreThanMax ();
void setLocalPortZero ();
void dontSetLocalPort ();
void setDestPortZero ();
void setDestPort ();
void setDestPortDiff ();
void setDestPortSelf ();
void setInvPortId ();
void setInvDataPtr ();
void setDataLenZero ();
void setLocalAddrDif ();
void setLocalAddrToDest ();
void setDestAddrDif ();
void displayParamSetting ();
void displayName();

void initUdpNh ();
void terminateUdpNh ();
void deleteUdpNh ();
void invalidConfig ();
void invCtrlUdpNh ();
void initialConfig ();
void generalConfig ();
void createUdpNh ();
void startUdpNh ();
void stopUdpNh ();
void statusReqUdpNh ();
void statsReqUdpNh ();

void addUdpPort ();
void remUdpPort ();
void setNoAckReq ();
void sendData ();
void sendDataAllOpened ();
void statTest ();
void statRemoteSetUp ();

//============================================================================
// <End of testfunctions.h>
//============================================================================
#endif // TEST_FUNCTIONS_H_


