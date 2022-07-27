//==============================================================================
// Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: cdsuinthash-test.cpp
// Description	: Tests for CdSuIntHash
// Ref Docs	:
// Author	: Titty Thomas
// Created On   : Thu May  1 13:21:40 IST 2003
// Last Modified:
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "cdsuinthash.h"

CdSuIntHash<int> intHash (CdSuIntHash<int>::TZ_251);

void forCheckingCopyConstructor (CdSuIntHash<int> ih)
{
	
//-----------------------------------------------------------------------------
// NOTE: To do very perfect test of copy constructor Following
// for Loop must be uncommented. To do key () interface must be added to
// iterator. 
//-----------------------------------------------------------------------------
	bool flag = true;
	CdSuIntHash<int>::iterator iter1 = intHash.begin ();
/*	for (;iter1 != intHash.end (); ++iter1)
	{
		int i; 
		if (ih.lookup (iter1.key (), i) != true || *iter1 != i)
		{
			flag = false;
			break;
		}
	}
*/
	CdSuIntHash<int>::iterator iter2 = ih.begin ();
	iter1 = intHash.begin ();
	int i = 0;
	for (; iter2 != ih.end (); ++iter2, ++iter1)
	{
		printf ("Copied Value %d, Old Value %d [%d]\n", *iter2, *iter1, i);
		++i;
	}
	
	ih.insert (111111111, 45);
	int val;
	if (flag == false || intHash.size () != (ih.size () - 1) || intHash.lookup (111111111, val) == true)
	{
		printf ("Copy constructor is not proper\n");
	} else
	{
		printf ("Copy Constructor is proper \n");
	}
} // void forCheckingCopyConstructor () 

int main ()
{
	printf ("Size of inthash is %d\n", sizeof (intHash));

	// Test #1, successful insert
	printf ("Test #1 ");
	if (intHash.insert (10, 1000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	printf ("Test #1 ");
	if (intHash.insert (20, 3000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	printf ("Test #1 ");
	if (intHash.insert (30, 4000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	// Test #2, unsuccessful insert
	printf ("Test #2 ");
	if (intHash.insert (30, 5000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	// Test #3, successful lookup 
	printf ("Test #3 ");
	int val;
	if (intHash.lookup (30, val))
	{
		printf ("lookup success, val %d\n", val);
	} else
	{
		printf ("lookup fail\n");
	}

	// Test #4, unsuccessful lookup 
	printf ("Test #4 ");
	if (intHash.lookup (60, val))
	{
		printf ("lookup success, val %d\n", val);
	} else
	{
		printf ("lookup fail\n");
	}

	// Test #5, successful remove
	printf ("Test #5 ");
	if (intHash.remove (20))
	{
		printf ("remove success\n");
	} else
	{
		printf ("remove fail\n");
	}

	// Test #6, unsuccessful remove
	printf ("Test #6 ");
	if (intHash.remove (20))
	{
		printf ("remove success\n");
	} else
	{
		printf ("remove fail\n");
	}

	// Test #7, iterator
	printf ("Test #7\n");
	CdSuIntHash<int>::iterator iter;
	iter = intHash.begin ();
	for (; iter != intHash.end (); ++iter)
	{
		printf (" Value : %d\n", *iter);
	}

	// Test #8, distribution test
	CdSuIntHash<int> intHash1 (CdSuIntHash<int>::TZ_251);
#define NUM_DTEST 1500
	int key;
	for (int i = 0; i < NUM_DTEST; i++)
	{
		key = rand ();
		if (!intHash1.insert (key, 0))
		{
			printf ("insert error, key : %d\n", key);
		}
	}
	printf ("Test #8\n");
	intHash1.print ();
	
	// Test #7, iterator
	printf ("Test #7\n");
	CdSuIntHash<int>::iterator iter1;
	iter1 = intHash1.begin ();
	for (; iter1 != intHash1.end (); ++iter1)
	{
		printf (" Value : %d\n", *iter1);
	}
	
	// Test #9.1, Copy Constructor when table is not empty
	printf ("Test #9.1 Testing Copy Constructor\n");
	forCheckingCopyConstructor (intHash);
	
	// Test #8, size and clear
	printf ("Test #8\n");
	printf ("Size : %d\n", intHash1.size ());
	intHash1.clear ();
	printf ("Size : %d\n", intHash1.size ());
	CdSuIntHash<int>::iterator iter2;
	iter2 = intHash1.begin ();
	for (; iter2 != intHash1.end (); ++iter2)
	{
		printf (" Value : %d\n", *iter2);
	}
	
	// Test #9.2, Copy Constructor when table is empty
	printf ("Test #9.2 Testing Copy Constructor\n");
	intHash.clear ();
	forCheckingCopyConstructor (intHash);
}
//==============================================================================
// <End of cdsuinthash-test.cpp>
//==============================================================================
