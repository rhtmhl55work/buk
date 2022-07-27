//=============================================================================
// ASSIGNMENTS - (c) Copyright 2013 by Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename		: udpclient.cpp
// Description		: client will send a "Hello World" over UDP to server
// 			  and will receive acknowledgement message from server
// 			  usage-> program-name server-address server-port
// Ref Docs		: 
// Author		: Prama Jati
// Created On		: 21.10.2013
// Last Modified	: 
// Last Modified On  	: 21.10.2013	
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

using namespace std;

//defining server ip address
//#define ETH0 "192.168.16.129"

//define maximum buffersize
#define BUFFERSIZE 512

//using an unused port in the host
#define SERVERPORT 50003

// to use [recvfrom()] call in full form 
#define   RECVFROM_FULL

// to use stripped [recvfrom()].   
//#define   RECVFROM_STRIPPED

// to use [recv()] call
//#define   RECV

// to use [read()] call
//#define   READ
 
int main (int argc, char *argv[])
{
	int sockfd;							// to be used as socket-descriptor 
	int n;
	ssize_t i;
	char buffer[BUFFERSIZE];					//buffer to read and write data
	struct sockaddr_in serverAddress;				//to store server address
	struct sockaddr_in clientAddress;				//to store client address
	
	#ifdef RECVFROM_FULL
    		
	// following  variables are used with [ recvfrom ] call in full form
    	struct      sockaddr_in   remoteAddress;
    	socklen_t                 remoteAddressLength;
    	unsigned    long          nbo;  
    	unsigned    long          hbo;  
    	char                      *hostAddress;
  
	#endif
	
	if (argc != 3)
	{
		cout<<"Usage: "<<argv[0]<<" server-address server-port\n";
		exit(1);
	}
	cout<<"UDP client starting...\n";

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
	clientAddress.sin_port = htons(0);				// any free port

	
	// Bind the local address with the socket
	// This is needed as client required a reply from the server
	// The server needs to know client address to send a reply
	n = bind(sockfd, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
  	if (n == -1)
	{
		perror("bind-call");
		exit(1);
	}

	memset(buffer, '\0', BUFFERSIZE);

  	// take input from user
	//write(STDOUT_FILENO,"Enter a string: ", 16);
    	//i = read(STDIN_FILENO, buffer, BUFFERSIZE);

	cout<<"Sending \"Hello World\" to server ... "<<endl;
	strcpy(buffer, "Hello world");
		
	//initializing the members of remote address
	serverAddress.sin_family = AF_INET;
	//assigning port number after converting host byte order to network byte order
  	serverAddress.sin_port = htons(atoi(argv[2]));
	n = inet_aton(argv[1], &(serverAddress.sin_addr));
	if (n == 0)
	{
		cout<<"Invalid address\n";
		exit(1);
	}
  	
	// send contents of [buffer] to the remote address
  	n = sendto(sockfd, buffer, 11, 0, ( struct sockaddr *) &serverAddress, sizeof( serverAddress));
  	if (n == -1)
	{
		perror("sendto-call");
		exit(1);
	}

	cout<<"client: Sent "<<n<<" bytes to server \n";

	n = 0;
	
	// clear the buffer, before receiving from server 
	memset(buffer, '\0', BUFFERSIZE);
/*
	#ifdef RECVFROM_FULL
     		
		cout<<"client: Using recvfrom() call in full form \n";
     		remoteAddressLength = sizeof(remoteAddress);
     		n = recvfrom(sockfd, buffer, BUFFERSIZE, MSG_PEEK,
                   (struct sockaddr *) &remoteAddress,
                   &remoteAddressLength );
     		if (n == -1)
		{
			perror("recvfrom-call-full");
			exit(1);
		}

    		// Print the value of the symbolic constant
     		//cout<<"AF_INET = "<<AF_INET<<endl;

     		// [recvfrom()] system call has filled in members of the variable [remoteAddress]
     		// The values of the members are now retrieved

     		cout<<"rf-full: remote address family = "<<remoteAddress.sin_family<<endl;

     		nbo = remoteAddress.sin_addr.s_addr;
     		cout<<"rf-full: remote address in network byte order = ";
     		cout<<nbo<<endl;

     		hbo = ntohl(remoteAddress.sin_addr.s_addr);
     		cout<<"rf-full: remote address in host byte order = "<<hbo<<endl;

     		hostAddress = inet_ntoa(remoteAddress.sin_addr);

     		cout<<"rf-full: remote address in dotted decimal quad = "<<hostAddress<<endl;

     		cout<<"rf-full: remote port in network byte order = "<<remoteAddress.sin_port<<endl;

     		cout<<"rf-full: remote port in host byte order = "<<ntohs(remoteAddress.sin_port)<<endl;

     		cout<<"rf-full: remote address length = "<<remoteAddressLength<<endl;                          
  	#endif


  	#ifdef RECVFROM_STRIPPED
     		cout<<"client: Using recvfrom() call in stripped form\n";
     		// not saving remote address
     		n = recvfrom(sockfd, buffer, BUFFERSIZE, 0, NULL, 0);
     		if (n == -1)
		{
			perror("recvfrom-call-stripped");
			exit(1);
		}
  	#endif

  	#ifdef RECV
     		cout<<"client: Using recv() call \n";
     		// not storing remote address but using [flag] argument
     		n = recv(sockfd, buffer, BUFFERSIZE, MSG_PEEK);
     		if (n == -1)
		{
			perror("recvfrom-call-stripped");
			exit(1);
		}
  	#endif


  	#ifdef READ
     		cout<<"client: Using read() call \n";
     		// not storing remote address and not using [flag] argument
     		n = read(sockfd, buffer, BUFFERSIZE);
     		if (n == -1)
		{
			perror("read-call");
			exit(1);
		}
  	#endif
	
 
	write(STDOUT_FILENO, "Received from server->", 22);
	i = write(STDOUT_FILENO, buffer, n);
	if( i == -1 ) { perror("write-2"); exit(1); }
  
	cout<<endl;
*/
	//remove the socket
	n = close(sockfd);
	if(n == -1)
	{
		perror("close-sockfd");
		exit(1);
	}

	return 0;
}

//============================================================================
// end of udpclient.cpp
//============================================================================

