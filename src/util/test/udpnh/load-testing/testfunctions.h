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

void resetStats ();
void setRemote ();
void setLocal ();
void setSelf ();
void setDelay ();
void resetDelay ();
void setMsgIntf ();
void setCallbackIntf ();
void setZeroSelTimer ();
void makeNhReady ();
void doLoadTestOne ();
void addAUdpPort ();
void addNUdpPort ();
void sendData ();
void sendDataForNUdpPort ();
void printStats ();
//============================================================================
// <End of testfunctions.h>
//============================================================================
#endif // _TEST_FUNCTIONS_H_

