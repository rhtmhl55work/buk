//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: cdsutrace-test.cpp
// Description	: Tests for CdSuTrace.
// Ref Docs	:
// Author	: Seema Chandak, Sumit Bijwe
// Created On   : Wed Nov 12 09:49:02 IST 2003
// Last Modified: 
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>    


#include "cdsutrace.h"

#ifdef __ECOS
externC void init_all_network_interfaces ();
#endif

int main ()
{
	CdSuTrace::setLevel (5);
	sleep (20);
#ifdef __ECOS
	init_all_network_interfaces ();
	sleep (3);
#endif
	char ch = 'a';
	int  hex = 0xff;
	printf ("Entered Main\n");
	CDSU_TRACE (1, "Entered Trace Test\n");
	CDSU_TRACE2 (5, "Testing TRACE with level 5 Ch = %c Hex = %x \n", ch, hex);

}
