//=============================================================================
// ASSIGNMENTS - (c) Copyright 2013 by Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename		    : bukAdminMenu.cpp
// Description		: User interface for Admin
// Ref Docs	    	:
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

#include "bukUiAdminMsgs.h"

using namespace std;

//defining server ip address
//#define SERVERIP "192.168.16.129"
#define SERVERIP "192.168.16.40" // rohitm

//using an unused port in the host
#define SERVERPORT 50003

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
		cout<<"\n\t\t ADMIN\n";
		cout<<"\nEnter Choice:";
		cout<<"\n1. Allocate Work to Mentor";
		cout<<"\n2. Start Training";
		cout<<"\n3. Stop Training";
		cout<<"\n4. Exit"<<endl;
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
		clientAddress.sin_port = htons(0);	        			// any free port


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
			cout<<"\nInvalid address\n";
			exit(1);
		}

		switch (choice)
		{
		case 1:
			{
			// Work To Mentor Message Handling

			int mId;
			cout<<"Enter Mentor Id: ";
			cin>>mId;

			BukUiAdWrk bukUiAdWrk;
			bukUiAdWrk.initMsg (mId);

			// send contents of [buffer] to Admin NH
			n = sendto(sockfd, (char *)&bukUiAdWrk, sizeof(bukUiAdWrk), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}

			cout<<"\nSending Work request to mentor "<<mId<<endl;
			}
			break;
		case 2:
			{
			//Start Training Message Handling

			BukUiAdStrTrng bukUiAdStrTrng;
			bukUiAdStrTrng.initMsg ();

			// send contents of [buffer] to Admin NH
			n = sendto(sockfd, (char *)&bukUiAdStrTrng, sizeof(bukUiAdStrTrng), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}

			cout<<"\nSending Start Training instruction to mentors and trainees... "<<endl;
			}
			break;
		case 3:
			{
			//Stop Training Message Handling

			BukUiAdStpTrng bukUiAdStpTrng;
			bukUiAdStpTrng.initMsg ();

			// send contents of [buffer] to Admin NH
			n = sendto(sockfd, (char *)&bukUiAdStpTrng, sizeof(bukUiAdStpTrng), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}

			cout<<"\nSending Stop Training instruction to mentors and trainees... "<<endl;
			}
			break;
		case 4:
			{
			//Exit Message Handling

			BukUiAdExit bukUiAdExit;
			bukUiAdExit.initMsg ();

			// send contents of [buffer] to Admin NH
			n = sendto(sockfd, (char *)&bukUiAdExit, sizeof(bukUiAdExit), 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
			if (n == -1)
			{
				perror("sendto-call");
				exit(1);
			}

			cout<<"\nAdmin Exiting..."<<endl;

			//remove the socket
			n = close(sockfd);
			if(n == -1)
			{
				perror("close-sockfd");
				exit(1);
			}

			//Admin Menu Exit
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
// end of bukAdminMenu.cpp
//============================================================================
