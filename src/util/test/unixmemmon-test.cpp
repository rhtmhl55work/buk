//==============================================================================
// Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: unixmemmon-test.cpp
// Description	: test for cdsuunixmemmon.h
// Ref Docs	:
// Author	: Vijaya Kumar
// Created On   : Thu Apr 20 13:37:29 IST 2006
// Last Modified:
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "cdsuunixmemmon.h"

int main ()
{
	CdSuUnixMemoryMonitor memMon;
	printf ("Memory Used Initially %d\n", memMon.getHeapMemoryUsed ());
	char *memArray [1024];
	for (int i = 1; i <= 1024; i++)
	{
		memArray [i-1] = new char [1024];
		printf ("Memory Used after iteration %d, is %ld and callculated one is %d\n",i, memMon.getHeapMemoryUsed (), 1024 * i);
	}
	for (int i = 1; i <= 1024; i++)
	{
		delete [] memArray [i-1];
		printf ("Memory Used after cleaning iteration %d, is %ld and callculated one is %d\n",i, memMon.getHeapMemoryUsed (), 1024 * 1024 - 1024 * i);
	}
}
//==============================================================================
// <End of cdsuinthash-test.cpp>
//==============================================================================
