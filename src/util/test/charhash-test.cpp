//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: charhash-test.cpp
// Description	: Tests for CdSuCharHash.
// Ref Docs	:
// Author	: Seema Chandak
// Created On   : 
// Last Modified: Thu Jun 19 18:36:00 IST 2003
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "cdsucharhash.h"

CdSuCharHash<int>* charHash = NULL;


void forCheckingCopyConstructor (CdSuCharHash<int> ch)
{
//-----------------------------------------------------------------------------
// NOTE: To do very perfect test of copy constructor Following
// for loop must be uncommented. To do key () size () interface must be added to
// iterator. 
//-----------------------------------------------------------------------------
	
	bool flag = true;
	CdSuCharHash<int>::iterator iter1 = charHash->begin ();
/*	for (;iter1 != charHash->end (); ++ iter1)
	{
		int i; 
		if (ch.lookup (iter1.key (), iter1.size (), i) != true || *iter1 != i)
		{
			flag = false;
			break;
		}
	}
*/	
	CdSuCharHash<int>::iterator iter2 = ch.begin ();
	iter1 = charHash->begin ();
	int i = 0;
	for (; iter2 != ch.end (); ++iter2, ++iter1)
	{
		printf ("Copied Value %d, Old Value %d [%d]\n", *iter2, *iter1, i);
		++i;
	}
	
	ch.insert ("vijay", 5, 45);
	int val;
	if (flag == false || charHash->size () != (ch.size () - 1) || charHash->lookup ("vijay", 5, val) == true)
	{
		printf ("Copy constructor is not proper\n");
	} else
	{
		printf ("Copy Constructor is proper \n");
	}
	
}

int main ()
{
	charHash = new CdSuCharHash <int> (CdSuCharHash<int>::TZ_251);
	CdSuCharHash<int> charHashObj (CdSuCharHash<int>::TZ_251);
	
	printf ("Size of charHash is %d\n", sizeof (charHashObj));

	// Test #1, successful insert
	printf ("Test #1 ");
	if (charHash->insert ("seema", 5, 1000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	printf ("Test #1 ");
	if (charHash->insert ("chandak", 7, 3000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	printf ("Test #1 ");
	if (charHash->insert ("seemachandak", 12, 4000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	// Test #2, unsuccessful insert
	printf ("Test #2 ");
	if (charHash->insert ("seema", 5, 5000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail - Test case SUCCESS\n");
	}

	// Test #3, successful lookup 
	printf ("Test #3 ");
	int val;
	if (charHash->lookup ("seemachandak", 12, val))
	{
		printf ("lookup success, val %d\n", val);
	} else
	{
		printf ("lookup fail\n");
	}

	// Test #4, unsuccessful lookup 
	printf ("Test #4 ");
	if (charHash->lookup ("meghna", 6, val))
	{
		printf ("lookup success, val %d\n", val);
	} else
	{
		printf ("lookup fail - Test case SUCCESS\n");
	}

	// Test #5, successful remove
	printf ("Test #5 ");
	if (charHash->remove ("chandak", 7))
	{
		printf ("remove success\n");
	} else
	{
		printf ("remove fail\n");
	}  

	// Test #6, unsuccessful remove
	printf ("Test #6 ");
	if (charHash->remove ("chandak", 7))
	{
		printf ("Remove success\n");
	} else
	{
		printf ("Remove fail - Test case SUCCESS\n");
	}
	
	// Test #7, iterator.
	printf ("Test #7 ");
	CdSuCharHash<int>::iterator iter;
	iter = charHash->begin ();
	for ( ; iter != charHash->end (); ++iter)
		printf ("Value: %d\n", *iter);

	delete charHash;
	
	// Test #8, distribution test.
	printf ("Test #8\n");
	CdSuCharHash<int>* charHash1  = new CdSuCharHash<int>
			(CdSuCharHash<int>::TZ_509);
	char key [64];
	int counter = 0;
	FILE* fp = fopen ("samplewords.txt", "r");

	while (fgets (key, 64, fp) != NULL)
	{
		if (!charHash1->insert (key, strlen (key), counter++))
		{
			printf ("insert error, key : %s\n", key);
		}
	}
	fclose (fp);	
	charHash1->print ();

	
	
	// Test #7 iterator.
	printf ("Test #7");
	CdSuCharHash <int>::iterator iter1;
	iter1 = charHash1->begin ();
	for (; iter1 != charHash1->end (); ++iter1)
	{
		printf (" Value : %d\n", *iter1);
	}

	
	// Test #9.1, Copy Constructor, When table is not empty.
	// Testing Copy Constructor: If you remove the 
	// copy constructor then if you call this function by
	// value definitely Tes#8 will generate segmentation fault.
	printf ("Testing Copy Constructor\n");
	forCheckingCopyConstructor (*charHash);
	
	// Test #8, size and clear
	printf ("Test #8\n");
	printf ("Size : %d\n", charHash1->size ());
	charHash1->clear ();
	printf ("Size : %d\n", charHash1->size ());
	CdSuCharHash<int>::iterator iter2;
	iter2 = charHash1->begin ();
	for (; iter2 != charHash1->end (); ++iter2)
	{
		printf (" Value : %d\n", *iter2);
	}
	
	// Test #9.2 Copy Constructor when table is empty
	printf ("Testing Copy Constructor\n");
	charHash->clear ();
	forCheckingCopyConstructor (*charHash);

	delete charHash1;
}

//==============================================================================
// <End of charhash-test.cpp>
//==============================================================================
