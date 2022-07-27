#ifdef CDSU_TRACING

//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsutrace.cpp
// Description  : Trace utility
// Ref Docs     : 
// Author       : Titty 
// Created On   : 
// Last Modified: 1. Mon Aug 16 10:51:41 IST 2004
// 			By : Vijaya Kumar
// 			printf is replaced by diag_printf for ecos.
//		: 2 Tue Apr 18 16:02:32 IST 2006
//			By : Vijaya Kumar
//			Following bugs entered in bugzilla are fixed
//			- 40
//		: 3.Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>

#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>	// For diag_printf : To Fix Bugs bugzilla/40 
#endif

#ifdef __sun
#include <strings.h>
#endif

#ifdef __ECOS
#define CDSU_PRINTF diag_printf 
#else
#define CDSU_PRINTF printf 
#endif

#include "cdsutrace.h"

#define NET_PRINT_IPADDR "192.168.51.215"
#define NET_PRINT_PORTNO 9005

unsigned CdSuTrace::traceLevel = 1;

void CdSuTrace::setLevel (unsigned level)
{
	printf("%s: Changing Trace level (%d -> %d)!! \n",__PRETTY_FUNCTION__, traceLevel, level);
	traceLevel = level;
}
bool CdSuTrace::canTrace (unsigned level)
{
	return level <= traceLevel;
}

// If Netprint is desired.
#ifdef CDSU_NETPRINT
void CdSuTrace::print (const char* fname, const unsigned lineno, unsigned level, char* fmt, ...)
{
	if (canTrace (level)) 
	{
		va_list ap; // argument pointer

		//--------------------------------------------------------------------
		// First open the relevant socket.
		//--------------------------------------------------------------------
		int sockfd = socket (AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0)
		{
			// printf ("Error! could not open socket\n");
			return;
		}
		struct sockaddr_in servaddr;
		bzero (&servaddr, sizeof (servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr (NET_PRINT_IPADDR);
		servaddr.sin_port = htons (NET_PRINT_PORTNO);

		//--------------------------------------------------------------------
		// Now create the print string to be transmitted.
		//--------------------------------------------------------------------
		char* txBuf = new char [1024];
		char* txBufPtr = txBuf;

		int len = sprintf (txBufPtr, "%s (%d) : ", fname, lineno);
		txBufPtr += len;

		va_start (ap, fmt); // ap points to first argument now.
		for (char* p = fmt; *p; p++)
		{
			if (*p != '%')
			{
				//putchar (*p);
				*(txBufPtr++) = *p;
				continue;
			}
			switch (*++p)
			{
			case 'd':
				{
				int ival = va_arg (ap, int); // get an arg and
				int len = sprintf (txBufPtr, "%d", ival);	
				txBufPtr += len;
				}
				break;

			case 'f':
				{
				double dval = va_arg (ap, double);
				int len = sprintf (txBufPtr, "%f", dval);	
				txBufPtr += len;
				}
				break;
				
			case 's':
				{
				char* sval = va_arg (ap, char*);
				strcpy (txBufPtr, sval);
				txBufPtr += strlen (sval);
				}
				break;

			case 'c':
				{
				char ch = (char) va_arg (ap, int);
				int len = sprintf (txBufPtr, "%c", ch);		
				txBufPtr += len;
				}
				break;

			case 'x':
				{
				int ival = va_arg (ap, int); // get an arg and
				int len = sprintf (txBufPtr, "%x", ival);	
				txBufPtr += len;
				}
				break;

			default:
				*(txBufPtr++) = *p;
				break;
			} // switch
		} // for.

		va_end (ap); // clean up
		*txBufPtr = '\0';

#ifdef __sun
	int length;
#else
	socklen_t length;
#endif

	length = sizeof (servaddr);

	if (sendto (sockfd, txBuf, (txBufPtr - txBuf + 1), 0,
			(const struct sockaddr*) &servaddr, length) < 0)
	{
			printf ("Error in sending data to print Server!\n");
	}
		delete [] txBuf;
		close (sockfd);

	}
}

#else
// If Prints on the same terminal is desired.
void CdSuTrace::print (const char* fname, const unsigned lineno, unsigned level, char* fmt, ...)
{
	if (canTrace (level)) 
	{
		va_list ap; // argument pointer
		char* p, *sval, ch; // string value
		int ival, hexval; // interger value
		double dval; // double value

		printf ("%s (%d) : ",fname, lineno);

		va_start (ap, fmt); // ap points to first argument now.

		for (p = fmt; *p; p++)
		{
			if (*p != '%')
			{
				putchar (*p);
				continue;
			}
			switch (*++p)
			{
			case 'd':
				{
				ival = va_arg (ap, int); // get an arg and go to the next one 
				CDSU_PRINTF ("%d", ival);
				}
				break;

			case 'f':
				{
				dval = va_arg (ap, double);
				CDSU_PRINTF ("%f", dval);
				}
				break;

			case 's':
				{
				for (sval = va_arg (ap, char*); *sval; sval++)
					putchar (*sval);
				}
				break;

			case 'c':
				{
				ch = (char) va_arg (ap, int);
				CDSU_PRINTF ("%c", ch);
				}
				break;

		   case 'x':
				{
				hexval = va_arg (ap, int);
				CDSU_PRINTF ("%x", hexval);
				}
				break;

			default:
				{
				putchar (*p);
				}
				break;

			}
		}
		va_end (ap); // clean up
	}
} 
#endif


//============================================================================
// <end of cdsutrace.cpp>
//============================================================================
#endif
