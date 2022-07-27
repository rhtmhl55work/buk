//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: thread-test.cpp
// Description	: Tests for CdSuThread.
// Ref Docs	:
// Author	: Seema Jain
// Created On   : 
// Last Modified: Mon Feb 16 18:02:07 IST 2004
//============================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include <stdio.h>
#include <cdsuthread.h>

//----------------------------------------------------------------------------
// Class Definition.
//----------------------------------------------------------------------------
class TestThread:public CdSuThread
{
public:
	static void* threadEntry (void*);	

private:

};

void* TestThread :: threadEntry (void*)
{
	while (1)
	{
		printf ("Hi I am Test Thread!\n");
	}
}

//----------------------------------------------------------------------------
// Main Program.
//----------------------------------------------------------------------------
int main ()
{
	TestThread* testThread = new TestThread;
	testThread->spawn (TestThread::threadEntry);
	while (1)
	{
		printf ("Hi I am main thread!\n");
	}
}



