#ifndef CDSU_MSG_DISTR_TEST_H
#define CDSU_MSG_DISTR_TEST_H
//============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : msgdistr-test.h
// Description	: 
// Ref Docs	:
// Author       : Seema Chandak 
// Created On   : 
// Last Modified: Tue Jul 22 10:42:56 IST 2003
//============================================================================

#include "cdsuutiltypes.h"

struct CdSuIntraCardMsg : public CdMessage
{
	char buffer[100];
	int length;

	void initMsg (CdModuleId dest, CdModuleId src, char buf[], int len)
	{
		msgHdr.dst = dest;
		msgHdr.src = src;
		msgHdr.msgLen = sizeof (CdSuIntraCardMsg);
		length = len;
		//buffer = buf;
		strcpy(buffer,buf);
	}
}; // struct CdSuIntraCardMsg

struct MyTestMsg : CdMessage
{
	unsigned int a;
	unsigned int b;
	unsigned int c;
	unsigned int buflen;
	char data [256];

	void initMsg (CdModuleId dest, CdModuleId src, 
		unsigned short opcode, char* buf, unsigned int datalen)
	{
		msgHdr.dst = dest;
		msgHdr.src = src;
		msgHdr.msgLen = sizeof (MyTestMsg);
		msgHdr.opcode = opcode;

		a = 0x12345678;
		b = 0x22334455;
		c = 0x10203040;
		if (buf != NULL)
		{
			buflen = datalen;
			memcpy (data, buf, datalen);
			data[ datalen ] = '\0';
		} else
		{
			buflen = 0;
			data[ buflen ] = '\0';
		}
	}

	void hton ()
	{
		a = htonl (a);
		b = htonl (b);
		c = htonl (c);
		buflen = htonl (buflen);
	}
	void ntoh ()
	{
		a = ntohl (a);
		b = ntohl (b);
		c = ntohl (c);
		buflen = ntohl (buflen);
	}
};



#endif


