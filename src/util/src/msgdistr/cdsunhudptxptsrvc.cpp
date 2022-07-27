//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudptxptsrvc.cpp
// Description	: 
// Ref Docs     : 
// Author       : Sumit Bijwe 
// Created On   : Mon Jul 21 18:15:10 IST 2003
// Last Modified:
// Modified By	: Vijaya Kumar
// Modified On	: Mon Aug  2 17:37:02 IST 2004
// 		: 		Closing Socket
//============================================================================

//----------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include "cdsunhudptxptsrvc.h"
#include "cdsutrace.h"


//---------------------------------------------------------------------------
// Name		: CdSuUdpTxptServices 
// Description  : Constructor
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuUdpTxptServices::CdSuUdpTxptServices ()
{
	listenSockfd = -1;
	// CDSU_TRACE (4, "CdSuUdpTxptServices::CdSuUdpTxptServices ()\n");

	if ((sendSockfd = socket (AF_INET, SOCK_DGRAM, 0 )) < 0)
	{
		CDSU_TRACE (1, "Socket creation error\n");
	}

}//CdSuUdpTxptServices::CdSuUdpTxptServices ()

//-----------------------------------------------------------------------------
// Name		: CdSuUdpTxptServices 
// Description  : Destructor
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//-----------------------------------------------------------------------------
 
CdSuUdpTxptServices::~CdSuUdpTxptServices ()
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	// shutdown (sendSockfd, 2);
	close (sendSockfd);
}// CdSuUdpTxptServices::~CdSuUdpTxptServices ()

//-----------------------------------------------------------------------------
// Name		: CdSuUdpTxptServices::setListener
// Description  : Wait for the message.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//-----------------------------------------------------------------------------

int CdSuUdpTxptServices::setListener (Uint32 ipaddr, Uint16 servPort)
{
	// CDSU_TRACE (4, "CdSuUdpTxptServices::setListener().\n");
	int sid;
	struct sockaddr_in servaddr;
	if ((sid = socket (AF_INET, SOCK_DGRAM, 0 )) < 0)
	{
		// CDSU_TRACE1 (3, "%s: Socket creation error.\n", __PRETTY_FUNCTION__);
		return (-1);
	}
	
	this->listenSockfd = sid;

	memset ((char *) &servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (servPort);
	servaddr.sin_addr.s_addr = ipaddr;
	
	if (bind (sid, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0)
	{
		// CDSU_TRACE2 (3, "%s: bind failure  = %d\n", __PRETTY_FUNCTION__, errno);
		return (-1);
	} 	
	// CDSU_TRACE3 (5, "CdSuUdpTxptServices::setListener(): ipaddr = %d, portNo = %d, sockfd = %d\n", ipaddr, servPort, sid);

	return sid;
	
}//int CdSuUdpTxptServices::setListener ()

//-----------------------------------------------------------------------------
// Name		: CdSuUdpTxptServices::sendMsg
// Description  : Send the message to  peer.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//-----------------------------------------------------------------------------

bool CdSuUdpTxptServices::sendMsg (char* buffer, Uint16 length, Uint32 ipAddr, 
	Uint16 portNo)
{
	struct sockaddr_in servaddr;
	int noSent;

	memset ((char *) &servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (portNo);
	servaddr.sin_addr.s_addr = ipAddr;

	noSent = sendto (sendSockfd, buffer, length, 0, (SA*) &servaddr,
				sizeof(servaddr));

	if (noSent > 0)
	{
		 // CDSU_TRACE2 (5, "Message sent to peer.\nlength = %d,\nnoSent = %d\n", length, noSent);
		
	} else
	{
		CDSU_TRACE (1, "sendto() failure \n");
		return false;
	}
	return true;

}//bool CdSuUdpTxptServices::sendMsg ()

//-----------------------------------------------------------------------------
// Name		: CdSuUdpTxptServices::recvMsg
// Description  : Receive the messages from Network.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//-----------------------------------------------------------------------------

bool CdSuUdpTxptServices::recvMsg (char* buf, int& length, Uint32& ipAddr, 
		Uint16& portNo)
{
#ifdef sun
	int len;
#else 	
	socklen_t len = sizeof (sockaddr_in);
#endif	
	struct sockaddr_in cliaddr;	
	fd_set fdlist;
	FD_ZERO (&fdlist);
	FD_SET (listenSockfd, &fdlist);
	int fd = 0;

	// For select call.
	if (fd <= listenSockfd)
		fd = listenSockfd + 1;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	int retVal = select (fd, &fdlist, NULL, NULL, NULL);

	if (retVal > 0)
	{
		if (FD_ISSET (listenSockfd, &fdlist))
		{
			length = recvfrom (listenSockfd, buf, 
			 CDSU_MSGDISTR_READ_BUF_LEN, 0, (struct sockaddr*) &cliaddr, &len);

			if (length <= 0)	
			{
				CDSU_TRACE (1, "Socket Receive Error\n");
				return false;
			}

			// CDSU_TRACE1 (5, "CdSuUdpTxptServices::recvMsg (): len = %d\n", length);

			struct sockaddr_in saddr = (struct sockaddr_in ) cliaddr;
			portNo = ntohs (saddr.sin_port);
			ipAddr = ntohl(saddr.sin_addr.s_addr);	
			
			// FIXME: 
			char* x = inet_ntoa (saddr.sin_addr);
			ipAddr = inet_addr (x);
			// CDSU_TRACE1 (5, "IP Address = %s\n", x);

			return true;
		}
	}
	return false;
} //bool CdSuUdpTxptServices::recvMsg ()

//-----------------------------------------------------------------------------
// Name		: CdSuUdpTxptServices::stopListener
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//-----------------------------------------------------------------------------

void CdSuUdpTxptServices::stopListener ()
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	// Nothing.
	close (listenSockfd);
}//void CdSuUdpTxptServices::stopListener ()

//============================================================================
// End of cdsunhudptxptsrvc.cpp
//============================================================================

