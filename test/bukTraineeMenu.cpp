//=============================================================================
// ASSIGNMENTS - (c) Copyright 2013 by Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename		    : bukTraineeMenu.cpp
// Description		: User interface for Trainee
// Ref Docs		    :
// Author		    : Trainee - 2013
// Created On		: 11.11.2013
// Last Modified	:
// Last Modified On : 11.11.2013
//============================================================================

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "bukUiTraineeMsgs.h"

using namespace std;

//defining server ip address
//#define SERVERIP "192.168.16.129"
#define SERVERIP "192.168.16.40" // rohitm

//using an unused port in the host
#define SERVERPORT 50008

//define maximum buffersize
#define BUFFERSIZE 512

int main (int argc, char *argv[])
{
	int sockfd;		            		// socket-descriptor
	int n;
	struct sockaddr_in serverAddress;	//to store server address
	struct sockaddr_in clientAddress;	//to store client address
	int choice;


	while (1)
	{
		//display menu
		cout<<"\n\t\t TRAINEE\n";
		cout<<"\nEnter Choice:";
		cout<<"\n1. Registration";
		cout<<"\n2. Guidance Request";
		cout<<"\n3. Exit"<<endl;
		cout<<"\nChoice: ";
		cin>>choice;

		// UDP socket is created
		sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sockfd == -1)
		{
			perror("socket-call");
			exit(1);
		}
		// zero maybe used for IPPROTO_UDP

		// initialise members of the local address
		clientAddress.sin_family = AF_INET;
		clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);		// all interfaces
		clientAddress.sin_port = htons(0);	    			    // any free port

		// Bind the local address with the socket
		// This is needed as client required a reply from the server
		// The server needs to know client address to send a reply
		n = bind(sockfd, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
		if (n == -1)
		{
			perror("bind-call");
			exit(1);
		}

		//initializing the members of remote address
		serverAddress.sin_family = AF_INET;
		//assigning port number after converting host byte order to network byte order
		serverAddress.sin_port = htons(SERVERPORT);

		n = inet_aton(SERVERIP, &(serverAddress.sin_addr));
		if (n == 0)
		{
			cout<<"Invalid address\n";
			exit(1);
		}

		switch (choice)
		{
		case 1:
			{
			//Registration message handling

			BukUiTrReg bukUiTrReg;
			bukUiTrReg.initMsg ();

			// send contents of [buffer] to Trainee NH
			n = sendto(sockfd, (char *)&bukUiTrReg, sizeof(bukUiTrReg), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}
			cout<<"\nSending Registration request to Admin... "<<endl;
			}
			break;
		case 2:
			{
			//Guidance Request Message Handling

			int tid;
			cout<<"Enter your id: ";
			cin>>tid;

			BukUiTrGdncRqst bukUiTrGdncRqst;
			bukUiTrGdncRqst.initMsg (tid);

			// send contents of [buffer] to Trainee NH
			n = sendto(sockfd, (char *)&bukUiTrGdncRqst, sizeof(bukUiTrGdncRqst), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}

			cout<<"\nSending Guidance request to mentor... "<<endl;
			}
			break;
			case 3:
			{
			//Exit Message Handling

			BukUiTrExit bukUiTrExit;
			bukUiTrExit.initMsg ();

			// send contents of [buffer] to Trainee NH
			n = sendto(sockfd, (char *)&bukUiTrExit, sizeof(bukUiTrExit), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}
			cout<<"\nTrainee Exiting..."<<endl;

			//remove the socket
			n = close(sockfd);
			if(n == -1)
			{
				perror("close-sockfd");
				exit(1);
			}

			// trainee menu exit
			exit(0);
			}
			break;
		default:
			{
			cout<<"\nWrong Choice!\n";
			}
			break;
		}

		//remove the socket
		n = close(sockfd);
		if(n == -1)
		{
			perror("close-sockfd");
			exit(1);
		}
	}
		return 0;
}

//============================================================================
// end of bukTraineeMenu.cpp
//============================================================================
