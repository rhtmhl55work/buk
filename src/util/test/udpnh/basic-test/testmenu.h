#ifndef TEST_MENU_H_
#define TEST_MENU_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testmenu.h
// Description  : Test menu header file
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================

#define CDTEST_END_NUM 255
#define CDTEST_SOCK_PORT 54715

#ifdef TEST_INPUT_THRU_SOCK
int cdCreateTestConn ();
int cdGetChoiceFromSock (int testSock);
int cdCloseTestConn (int testSock);
#endif

void cdExecuteTests (int testSock);

int cdGetNumTests ();
void cdPrintTestMenu ();

// test function table
typedef void (*CdPtrFunc) ();
struct CdTestCases 
{
	char* testName;
	CdPtrFunc testFn;
};
//extern CdTestCases cdTests [];

//============================================================================
// <End of testmenu.h>
//============================================================================
#endif // TEST_MENU_H_
