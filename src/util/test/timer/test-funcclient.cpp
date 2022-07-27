//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : test-client.cpp
// Description  : Client for testing Timer.
// Author       : Indu Mishra.
// Created On   : Mon Apr  7 19:28:31 IST 2003
// Last Modified:
//============================================================================

//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include <pthread.h>
#include <stdio.h>
#include<netdb.h>
#include<string.h>
#include<sys/types.h>
#include<sys/errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdlib.h>

/*
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
*/


#define TEST_SERVER_IPADDR "192.168.16.72"

#define TEST_SOCK_PORT 9500
#define MAX_NUM_OF_TESTS 24


void printMenu ();

// test function table
typedef void (*PtrFunc) ();

struct TestCases 
{
	char* testName;
	PtrFunc testFn;
};

// dummy functions for test matrix
void TT_TIME_COUNT_V () {}
void TT_TIME_COUNT_INV () {}
void TT_TIME_UNIT_V_001 () {}
void TT_TIME_UNIT_V_002 () {}
void TT_TIME_UNIT_V_003 () {}
void TT_TIME_UNIT_V_004 () {}
void TT_TIME_UNIT_V_005 () {}
void TT_TIME_UNIT_INV () {}
void TT_DEST_MODULE_ID_V () {}
void TT_DEST_MODULE_ID_INV () {}
void TT_OPCODE_V () {}
void TT_OPCODE_INV () {}
void TT_TIMER_TYPE_V_001 (){}
void TT_TIMER_TYPE_V_002 () {}
void TT_TIMER_TYPE_INV () {}
void TT_TIME_OUT_REP_TYPE_V_001 () {}
void TT_TIME_OUT_REP_TYPE_V_002 () {}
void TT_TIME_OUT_REP_TYPE_INV() {}
void TT_CALL_BACK_V() {}
void TT_CALL_BACK_INV() {}
void TT_STOP_TIMER_V () {}
void TT_STOP_TIMER_INV () {}
void TT_START_TIMER_V () {}	



// Array of TestCases and their Descriptions.
TestCases tests [] = {
	{"Valid Time Count",TT_TIME_COUNT_V},
	{"Invalid Time Count",TT_TIME_COUNT_INV},
	{"Valid Time Unit 001",TT_TIME_UNIT_V_001},
	{"Valid Time Unit 002",TT_TIME_UNIT_V_002},
	{"Valid Time Unit 003",TT_TIME_UNIT_V_003},
	{"Valid Time Unit 004",TT_TIME_UNIT_V_004},
	{"Valid Time Unit 005",TT_TIME_UNIT_V_005},
	{"Invalid Time Unit",TT_TIME_UNIT_INV},
	{"Valid Destination ModuleId",TT_DEST_MODULE_ID_V},
	{"Invalid Destination ModuleId",TT_DEST_MODULE_ID_INV},
	{"Valid Opcode",TT_OPCODE_V},
	{"Invalid Opcode",TT_OPCODE_INV},
	{"Valid Timer Type 001",TT_TIMER_TYPE_V_001},
	{"Valid Timer Type 002",TT_TIMER_TYPE_V_002},
	{"Invalid Timer Type",TT_TIMER_TYPE_INV},
	{"Valid TimeOut Report Type 001",TT_TIME_OUT_REP_TYPE_V_001},
	{"Valid TimeOut Report Type 002",TT_TIME_OUT_REP_TYPE_V_002},
	{"Invalid TimeOut Report Type",TT_TIME_OUT_REP_TYPE_INV},
	{"Valid CallBack Function",TT_CALL_BACK_V},
	{"Invalid CallBack Function",TT_CALL_BACK_INV},
	{"Valid Stop Timer",TT_STOP_TIMER_V},
	{"Invalid Stop Timer",TT_STOP_TIMER_INV},
	{"Valid Start Timer",TT_START_TIMER_V}	
};

//--------------------------------------------------------------------------
// Function Name  : main ()
// Purpose        : to give input to rtp module
// Input Param    : none
// Return Value   : none
// Side Effects   : 
// Algorithm      : 
//	- connect to RTP server thread
//	- read user input
//	- send input to server
//	- if input is zero, exit
//--------------------------------------------------------------------------

int main()
{

	// create socket and connect to test server
	int sockfd;
	struct sockaddr_in serveraddr;
	
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("socket error\n");
		return -1;
	} 
	
	memset (&serveraddr, 0, sizeof (serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons (TEST_SOCK_PORT);
	serveraddr.sin_addr.s_addr=inet_addr(TEST_SERVER_IPADDR);
/*	
#ifndef __ECOS
	if (inet_pton (AF_INET, argv[1], &serveraddr.sin_addr) < 0)
	{
		printf ("inet_pton error\n");
	}
#else
	if (inet_aton (TEST_SERVER_IPADDR, &serveraddr.sin_addr))
	{
		printf ("Success\n");
	}
#endif
*/
	printf ("Sizeof serveraddr is %d, sockfd is %d\n", sizeof (serveraddr), sockfd);
	if (connect (sockfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr)) == -1)
	{
		printf ("connect error\n");
		return -1;
	} else
	{
		printf ("connection established\n");
		int choice;
		// get the number of tests
		// int numTests = getNumTests ();
		int numTests = MAX_NUM_OF_TESTS;

		bool isOver = false;
		while (!isOver)
		{
			printMenu ();
			printf ("Enter your choice; 0 for menu : ");
			scanf ("%d", &choice);
			if (choice == (numTests - 1))  isOver = true;

			// send thru socket
			choice = htonl (choice);
			if ((write (sockfd, (char*)&choice, 4)) < 0)
			{
				printf ("write error\n");
				return -1;
			} 
		}
	}
	return 0;
	// Close Socket.
	close(sockfd);	
	
}// main ()


//----------------------------------------------------------------------------
// print menu 
//----------------------------------------------------------------------------
void printMenu ()
{
	printf ("========================================================\n");
	int i;
	for (i = 0; tests[i].testFn != 0 ; ++i)
	printf ("(%d) %s\n", i, tests[i].testName);
	printf ("(%d) EXIT!\n",i);
	printf ("---------------------------------------------------------\n");

}// printmenu ()



//============================================================================
// <End of test-funcclient.cpp>
//============================================================================
