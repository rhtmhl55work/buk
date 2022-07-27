#ifndef __CDSU_UNIXMEMMON_H__
#define __CDSU_UNIXMEMMON_H__
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsuunixmemmon.cpp
// Description  : Gets information on Heap memory used by a program.
// Author       : Vijay Kumar
// Created On   : Tue Jun 14 11:17:18 IST 2005
// Last Modified: 
// Added to Utility Kit from SS7AMF on Wed Apr 19 18:19:26 IST 2006
//============================================================================

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <malloc.h>
#include <time.h>
#include <stdio.h>

class CdSuUnixMemoryMonitor
{
public:       
	CdSuUnixMemoryMonitor ()
   	{
		startHeapMemoryAddr = (long) sbrk (0);       
	}

	unsigned long getHeapMemoryUsed ()
   	{
       		return ((unsigned long)sbrk (0) - startHeapMemoryAddr);
   	}

private:
   	unsigned long startHeapMemoryAddr;

}; // CdSuUnixMemoryMonitor

//============================================================================
// <end of file>
//============================================================================
#endif // __CDSU_UNIXMEMMON_H__
