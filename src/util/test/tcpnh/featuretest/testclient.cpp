//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : testclient.cpp
// Description  : Test client
// Author       : Titty Thomas
// Created On   : Mon Apr  7 19:28:31 IST 2003
// Last Modified:
//============================================================================

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "testmenu.h"

#define CDTEST_SERVER_IPADDR "192.168.16.72"

int main ()
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
	serveraddr.sin_port = htons (CDTEST_SOCK_PORT);
	if (inet_pton (AF_INET, CDTEST_SERVER_IPADDR, &serveraddr.sin_addr) < 0)
	{
		printf ("inet_pton error\n");
	}

	printf ("Connecting to %s:%d\n", CDTEST_SERVER_IPADDR, CDTEST_SOCK_PORT);
	if (connect (sockfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr)) == -1)
	{
		printf ("connect error\n");
		return -1;
	} else
	{
		printf ("Connection established\n");
		int choice;

		bool isOver = false;
		while (!isOver)
		{
			cdPrintTestMenu ();
			printf ("Enter your choice; 0 for menu; 255 to exit : ");
			scanf ("%d", &choice);

			if (choice == CDTEST_END_NUM)
			{
				isOver = true;
			}

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

//============================================================================
// <End of testclient.cpp>
//============================================================================
