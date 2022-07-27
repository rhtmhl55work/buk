//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : test-listclient.cpp
// Description  : Client for testing Timer.
// Author       : Indu Mishra.
// Created On   : 29th April, 2003
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
#include <stdio.h>	// Commented on 05/09/2003
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> // struct sockaddr 
#include <arpa/inet.h> // inet_addr() 
#include <unistd.h>
#include "cdsutrace.h"
#include <netinet/in.h> // sockaddr_in 
*/

#define TEST_SERVER_IPADDR "192.168.16.72"

#define TEST_SOCK_PORT 54715
#define MAX_NUM_OF_TESTS 11


void printMenu ();

// test function table
typedef void (*PtrFunc) ();

struct TestCases 
{
	char* testName;
	PtrFunc testFn;
};
// Function declaration.
void getUserInput(void );

int main()
{
	// create socket and connect to test server
	int sockfd;
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("socket error\n");
		return -1;
	} 

	struct sockaddr_in serveraddr;
	memset (&serveraddr, 0, sizeof (serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons (TEST_SOCK_PORT);
	serveraddr.sin_addr.s_addr=inet_addr(TEST_SERVER_IPADDR);
	
/*#ifndef __ECOS
	if (inet_pton (AF_INET, argv[1], &serveraddr.sin_addr) < 0)
	{
		printf ("inet_pton error\n");
	}
#else 
	if( inet_aton (TEST_SERVER_IPADDR, &serveraddr.sin_addr))
	{
		printf ("Success\n");
	}
#endif
*/
	if (connect (sockfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr)) == -1)
	{
	//	printf ("connect error\n");
		perror ("Connect fail!\n");	
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
			getUserInput ();
			printf ("Enter your choice : ");
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
}

//-----------------------------------------------------------------------------
// Function getUserInput()
//-----------------------------------------------------------------------------
void getUserInput(void )
{
	printf("Function getUserInput\n");
	printf("*************************************\n");
	printf("CHOICE:\n");
	
	printf("ADD_NO_NODE : 1\n");
	printf("ADD_FIRST_NODE : 2\n");
	printf("ADD_LAST_NODE : 3\n");
	printf("ADD_MIDDLE_NODE : 4\n");
	printf("TO_FIRST_NODE : 5\n");
	printf("STOP_FIRST_NODE : 6\n");
	printf("STOP_LAST_NODE : 7\n");
	printf("STOP_MIDDLE_NODE : 8\n");
	printf("CYCLIC_TIMER : 9\n");
	printf("*************************************\n");
	
}

//============================================================================
// <End of test-client.cpp>
//============================================================================
