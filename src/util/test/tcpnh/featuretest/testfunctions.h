#ifndef TEST_FUNCTIONS_H_
#define TEST_FUNCTIONS_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testfunctions.h
// Description  : Test Functions
// Author       : Sumit Bijwe
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
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
bool createTcpNh (bool);
bool configTcpNh (bool);
bool startTcpNh (bool);
bool stopTcpNh (bool);
bool deleteTcpNh (bool);
bool terminateTcpNh (bool);
bool statusReqTcpNh (bool);
bool statsReqTcpNh (bool);
void* applThrdStart (void*);
void* sysMgmtStart (void*);
void initAllGlobals ();
bool addListener(bool);
bool connectReq (bool);
void connectReqLoadTesting (bool);
bool sendData (bool);
void sendDataLoadTesting (bool);
bool remListener(bool);
bool closeConReq (bool);
void closeConReqLoadTesting (bool);

//----------------------------------------------------------------------------
// Test Case functions. 
//----------------------------------------------------------------------------
void initNhValid ();
void initNhInValid ();
void genCfgNhValid ();
void genCfgNhInValid ();
void startNhValid ();
void startNhInValid ();
void stopNhValid ();
void stopNhInValid ();
void deleteNhValid ();
void deleteNhInValid ();
void terminateNhValid ();
void terminateNhInValid ();
void statusReqNhValid ();
void statusReqNhInValid ();
void statsReqNhValid ();
void statsReqNhInValid ();
void startMiTesting ();
void startUiTesting ();


void addListnValid ();
void addListnInValid ();
void remListnValid ();
void remListnInValid ();
void conReqValid ();
void conReqInValid ();
void dataReqValid ();
void dataReqInValid ();
void clConReqValid ();
void clConReqInValid ();




//============================================================================
// <End of testfunctions.h>
//============================================================================
#endif // TEST_FUNCTIONS_H_

