//=============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     :  mp3StreamerClient.cpp
// Description  : Streaming Mp3 Data To Server.
// Author       : Vijaya Kumar
// Created On   : Fri Apr  1 16:13:01 GMT 2004
// Last Modified:
//============================================================================

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------

#include <string.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <stdlib.h>
#include <unistd.h> 

#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>

#ifdef __sun
#include <strings.h>
#endif


//----------------------------------------------------------------------------
// Maximum data sent in each send.
//----------------------------------------------------------------------------

#define MAXDATA 1024

//----------------------------------------------------------------------------
// Main Function
//----------------------------------------------------------------------------
int main (int argc, char* arg [])
{
	// Check wether user provided all requied commandline input 
	if (argc < 5)
	{
	 	printf ("<program> <dst> <port> <song.mp3> <mount>\n");
		exit (1);
	}
	// Open socket to stream mp3 to server.
	int socketID;
	struct sockaddr_in server_addr;
	if((socketID=socket(AF_INET,SOCK_STREAM,0))<0)
  	{
   		perror("couldn't open socket");
   		exit(1);
  	}
 	// build server address
	bzero((char *)&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(arg [1]);
	server_addr.sin_port= htons (atoi (arg [2]));
 	// Connect to Server
 	if(connect(socketID,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
  	{
   		perror("couldn't connect to server");
   		exit(1);
  	}
  
	// Tell Server Channel Wanted
	write (socketID, arg [4], strlen (arg[4]) + 1);
	// TODO: Wait For Ack:(If +ve Ack Wanted Channel available otherwise not)
	char ack [3];
	read (socketID, ack, 3);
	if (strcmp (ack, "OK") != 0)
	{
		printf ("Channel Already In Use\n");
		close (socketID);
		exit (1);
	} else
	{
		printf ("Ack Received\n");
	}
	char mp3[MAXDATA];
  	int n;

#ifdef __sun
	int len;
#else
	socklen_t len;
#endif

	// Open Mp3 file
	int fd = open (arg [3], O_RDONLY);
	if (fd < 0) 
	{
		// Mp3 File Opening Error.
		printf ("File Open error\n");
		close (socketID);
		exit (1);
		
	} else
	{
		while(1)
		{
			printf ("Streamer: reading........... \n");
			// Read Mp3 From File
			int rbytes = read (fd, mp3, MAXDATA);
			if (rbytes <= 0)
			{
				// MP3 File read Error
				printf ("Mp3 file read error\n");
				close (socketID);
				close (fd);
				exit (1);
			}	
			// Write Read Mp3 to Socket
			n = write (socketID, mp3, rbytes);
			printf("Streamer: written #%d\n", n);
			if( n < 0)
			{
				// Write To Socket Error.
				printf("Socket write error\n");
				close (socketID);
				close (fd);
				exit(1);
			}
			// Sleep for a while
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = 16384000; 
			nanosleep (&sleepTime, NULL);
		}
		close (socketID);
		close (fd);
	}
} // main ()	


//============================================================================
// <End Of File mp3StreamerClient.cpp>
//============================================================================
