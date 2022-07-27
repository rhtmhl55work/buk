//==============================================================================
// Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Fqename	: queue-test.cpp
// Description	: Tests for CdSuQueue
// Ref Docs	:
// Author	: Seema Chandak
// Created On   : Fri May 30 10:38:43 IST 2003
// Last Modified:
//==============================================================================

#include <stdio.h>
#include "cdsuqueue.h"

CdSuQueue <int> queue (5);

typedef bool (*PtrFunc) ();

bool popEmptyQueueTest ();
bool checkEmptyQueueTest ();
bool addPopTest ();
bool addFullQueueTest ();
bool checkFullQueueTest ();
bool printQueueElements ();

struct TestCases
{
	char* testName;
	PtrFunc testFn;
};

void forCheckingCopyConstructor (CdSuQueue<int> q)
{
	bool flag = true;
	CdSuQueue<int>::iterator iter1 = queue.begin ();
	CdSuQueue<int>::iterator iter2 = q.begin ();
	for (; iter1 != queue.end () && iter2 != q.end ();
			++iter1, ++iter2)
	{
		if (*iter1 != *iter2 || iter1 == iter2)
		{
			flag = false;
			break;
		}
	}

	if (flag)
	{
		q.add (5);
		if (q.getSize () == queue.getSize () + 1)
		{
			printf ("Copy Constructor is working fine. Size %d\n", queue.getSize ());
		} else
		{
			flag = false;	
		}
	}
	if (flag ==  false)
	{
		printf ("Copy Constructor is not working\n");
	}
}


int main ()
{
	TestCases tests [] =
	{
		{ "Pop Empty QueueTest", popEmptyQueueTest		},
		{ "Check Empty Queue Test", checkEmptyQueueTest		},
		{ "Add and Pop Test", addPopTest },
		{ "Add Full Queue Test", addFullQueueTest		},
		{ "Check Full Queue Test", checkFullQueueTest		},
		{ "Print the Queue Elements", printQueueElements	},
		{ "End Of Test", 0					}
	};

	for (int i = 0; tests [i].testFn != NULL; i++)
	{
		if (tests [i].testFn () == true)
			printf ("test [%d] success\n", i);
		else
			printf ("test [%d] fail\n", i);
	}
	// Test Case 8.2 Copy Constructor when queue is empty
	forCheckingCopyConstructor (queue);
	queue.add (1);
	queue.add (2);
	queue.add (3);
	queue.add (4);
	// Test Case 8.1 Copy Constructor when queue is not empty
	forCheckingCopyConstructor (queue);
}

bool popEmptyQueueTest ()
{
	CdSuQueue <int> queue (5);
	int i;	
	bool retVal = queue.pop (i);
	if (retVal == false)
		return true;
	else 
		return false;
	
}

bool checkEmptyQueueTest ()
{
	CdSuQueue <int> queue (5);
	if (queue.empty () == true)
	{
		bool retVal = queue.add (5);
		if (retVal == true)
		{
			if (queue.empty () == false)
				return true;
			else
				return false;
		} else 
			return false;
	} else 
		return false;
}


bool addPopTest ()
{
	CdSuQueue <int> queue (5);
	if (queue.add (10) == false)
		return false;
	if (queue.add (20) == false)
		return false;
	if (queue.add (30) == false)
		return false;
	if (queue.add (40) == false)
		return false;
	
	int i;
	bool retVal;

	retVal = queue.pop (i);
	if (retVal == false)
		return false;
	if (i != 10)
		return false;
	
	retVal = queue.pop (i);
	if (retVal == false)
		return false;
	if (i != 20)
		return false;
	
	retVal = queue.pop (i);
	if (retVal == false)
		return false;
	if (i != 30)
		return false;

	retVal = queue.pop (i);
	if (retVal == false)
		return false;
	if (i != 40)
		return false;

	return true;
}

bool addFullQueueTest ()
{
	CdSuQueue <int> queue (5);

	for (int i = 0; i < 5; i++)
	{
		if (queue.add (i) == false)
			return false;
	}
	
	if (queue.full () == true)
	{
		if (queue.add (50) == false)
			return true;
		else 
			return false;
	}
	return true;
}

bool checkFullQueueTest ()
{
	CdSuQueue <int> queue (5);
	if (queue.full () == false)
	{
		for (int i = 0; i < 5; i++)
		{
			if (queue.add (i) == false)
				return false;
		}
		if (queue.full () == true)
			return true;
		else
			return false;
	} else
		return false;

	return true;
}

bool printQueueElements ()
{
	CdSuQueue <int> queue (5);
	CdSuQueue<int>::iterator iter;
	int i;
	
	// Printing for an empty queue.
	printf ("Printing Elements of an Empty Queue.\n");
	for (iter = queue.begin (); iter != queue.end (); ++iter )
	{
		printf ("%d\n", *iter);
	}
	// Printing Queue with one element.
	printf ("Printing Elements of an one element queue .\n");
	queue.add (20);
	for (iter = queue.begin (); iter != queue.end (); ++iter )
	{
		printf ("%d\n", *iter);
	}
	queue.pop (i);
	
	// Printing Elements of a full queue.
	for (int i = 0 ; i < 5; i++)
	{
		if (queue.add (i) != true)
			return false;
	}
	printf ("Printing the elements in the full queue\n");	
	for (iter = queue.begin (); iter != queue.end (); ++iter )
	{
		printf ("%d\n", *iter);
	}
	return true;
}

//==============================================================================
// <End of queue-test.cpp
//==============================================================================
