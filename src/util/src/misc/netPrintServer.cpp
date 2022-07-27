//=============================================================================
// Internet Telephony Software = (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: netPrintServer.cpp
// Description	: This program can be used to display CDSU_TRACE network output
// Ref Docs	:
// Author	: Meghna Doshi
// Contributors	:
// Created On	:
// Last Modified:
// Compiling	: g++ netPrintServer.cpp
//=============================================================================

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>


enum
{
	PRINT_SERVER_PORT_NO = 9060,
	MAX_RXBUF_LEN = 1024
};

//----------------------------------------------------------------------------
// class	: NetPrintServer
//----------------------------------------------------------------------------

class NetPrintServer
{
public:
	NetPrintServer (char* ipaddr, unsigned short portNo);
	~NetPrintServer ();

	void run ();

private:
	char* ipaddr;
	unsigned short portNo;
};

NetPrintServer::NetPrintServer (char* ip, unsigned short port)
{
	this->ipaddr = new char [32];	
	strcpy (ipaddr, ip);
	portNo = port; 
}

void NetPrintServer::run ()
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	
	sockfd = socket (AF_INET, SOCK_DGRAM, 0);
	
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	// servaddr. sin_port = htons (PRINT_SERVER_PORT_NO);
	servaddr. sin_port = htons (portNo);

	if (bind (sockfd, (struct sockaddr*) &servaddr, sizeof (servaddr)) < 0)
        {
                printf ("NetPrintServer::Bind failure.\n");
        } else
	{
                printf ("NetPrintServer::Bind success.\n");
	}

	//--------------------------------------------------------------------
	//	
	//--------------------------------------------------------------------
	while (1)
	{
		//----------------------------------------------------------------------------
		// Socket for reading UDP packet on timeout.
		//----------------------------------------------------------------------------
		char* rxBuf[1024];
		socklen_t len;
		int n;
		struct sockaddr_in cliaddr;
		
		if ((n = recvfrom (sockfd, rxBuf, MAX_RXBUF_LEN, 0, 
			(struct sockaddr*) &cliaddr, &len)) == -1 )
		{
			printf ("NetPrintServer::socket read error.\n");
			return;
		}
		else if (n == 0)	// end of file
		{
			// do nothing.
		} else // Some data present.
		{
			printf ("%s", rxBuf);
		}
	}  // while

}

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		printf ("Usage : nps <UDP port no>\n");
		exit (1);
	}
	int portNo = atoi (argv [1]);
	NetPrintServer* nps = new NetPrintServer ("192.168.51.204", portNo);
	nps->run ();
}

