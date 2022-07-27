#ifndef TEST_FUNCTIONS_H_
#define TEST_FUNCTIONS_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testfunctions.h
// Description  : Test Functions
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified: By Sumit Bijwe
//============================================================================
void cdSetTraceLevel0 ();
void cdSetTraceLevel1 ();
void cdSetTraceLevel2 ();
void cdSetTraceLevel3 ();
void cdSetTraceLevel4 ();
void cdSetTraceLevel5 ();

// TODO: Add test functions here, format void funcName (void)

void createULThr ();
void createSMThr ();
void createTcpNh ();
void configTcpNh ();
void startTcpNh ();
void stopTcpNh ();
void deleteTcpNh ();
void terminateTcpNh ();
void statusReqTcpNh ();
void statsReqTcpNh ();
void* applThrdStart (void*);
void* sysMgmtStart (void*);
void initAllGlobals ();
void addListener();
void connectReq ();
void connectReqLoadTesting ();
void sendData ();
void sendDataLoadTesting ();
void remListener();
void closeConReq ();
void closeConReqLoadTesting ();
void setInvNhInstId();
void printStats ();
void cdSuExit ();
//============================================================================
// <End of testfunctions.h>
//============================================================================
#endif // TEST_FUNCTIONS_H_

