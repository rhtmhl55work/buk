//==============================================================================
// Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: cdsulist-test.cpp
// Description	: Tests for CdSuList
// Ref Docs	:
// Author	: Titty Thomas
// Created On   : Wed Apr 30 16:56:12 IST 2003
// Last Modified:
//==============================================================================

#include <stdio.h>
#include "cdsulist.h"

CdSuList<int> intList;
CdSuList<int> list;

void forCheckingCopyConstructor (CdSuList<int> il)
{
//-----------------------------------------------------------------------------
// NOTE: To do very perfect test of copy constructor Following
// for loop must be uncommented. To do key () interface must be added to
// iterator. 
//-----------------------------------------------------------------------------
	
	bool flag = true;
/*	CdSuList<int>::iterator iter1 = list.begin ();
	CdSuList<int>::iterator iter2 = il.begin ();
	for (; iter1 != list.end () && iter2 != il.end ();
			++iter1, ++iter2)
	{
		if (*iter1 != *iter2 || iter1 == iter2)
		{
			flag = false;
			break;
		}
	}
*/
	if (flag)
	{
		il.push_back (5);
		if (il.size () == list.size () + 1)
		{
			printf ("Copy Constructor is working fine. Size %d\n", list.size ());
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

void printList (CdSuList<int> l)
{
	int i = 0;
	for (CdSuList<int>::iterator iter = l.begin ();
			iter != l.end (); iter++)
	{
		printf ("List [%d] = %d\n", i, *iter);
		++i;
	} // for 
} // void printList

int main ()
{
	CdSuList<int>::iterator iter;

	// Test #1, push_front and push_back
	printf ("Test #1\n");
	intList.push_front (20);
	intList.push_front (10);
	intList.push_back (30);
	intList.push_back (40);

	// verify the values
	iter = intList.begin ();
	printf ("Size : %d\n", intList.size ());
	for ( int i = 1; iter != intList.end (); ++iter, ++i)
	{
		printf ("No. %d : val : %d\n", i, *iter);
	}

	// Test #2, pop_front and pop_back
	printf ("Test #2\n");
	intList.pop_front ();
	intList.pop_back ();

	// verify the values
	iter = intList.begin ();
	printf ("Size : %d\n", intList.size ());
	for ( int i = 1; iter != intList.end (); ++iter, ++i)
	{
		printf ("No. %d : val : %d\n", i, *iter);
	}

	// Test #3, front and back
	printf ("Test #3\n");
	intList.push_front (10);
	intList.push_back (40);
	printf ("Front val : %d\n", intList.front ());
	printf ("Back val : %d\n", intList.back ());

	// Test #4, clear and empty
	printf ("Test #4\n");
	if (intList.empty ())
	{
		printf ("List empty\n");
	} else
	{
		printf ("List not empty\n");
	}
	intList.clear ();
	if (intList.empty ())
	{
		printf ("List empty\n");
	} else
	{
		printf ("List not empty\n");
	}

	// Test #5, iterator::operator*
	printf ("Test #5\n");
	intList.push_back (10);
	iter = intList.begin ();
	printf ("Begin val : %d\n", *iter);
	*iter = 20;
	iter = intList.begin ();
	printf ("Begin val : %d\n", *iter);

	// Test #6, iterator::operator++ (prefix and postfix)
	printf ("Test #6\n");
	intList.push_front (10);
	intList.push_back (30);
	intList.push_back (40);
	iter = intList.begin ();
	printf ("Begin val : %d\n", *iter);
	iter++;
	printf ("Next val : %d\n", *iter);
	++iter;
	printf ("Next val : %d\n", *iter);

	// Test #7, iterator::operator== and operator!=
	printf ("Test #7\n");
	CdSuList<int>::iterator iter1 = intList.begin ();
	iter = intList.begin ();
	if (iter == iter1)
	{
		printf ("Iterators are equal\n");
	} else
	{
		printf ("Iterators are not equal\n");
	}

	iter = intList.end ();
	if (iter != iter1)
	{
		printf ("Iterators are not equal\n");
	} else
	{
		printf ("Iterators are equal\n");
	}

	// Test #8, erase
	printf ("Test #8\n");
	iter = intList.begin ();
	iter = intList.erase (iter);
	iter++;
	iter = intList.erase (iter);
	iter = intList.begin ();
	printf ("Size : %d\n", intList.size ());
	for (int i = 1; iter != intList.end (); ++iter, ++i)
	{
		printf ("No. %d : val : %d\n", i, *iter);
	}

	//--------------------------------------------------------------
	// Following Section added to test insert and erase (first, last)
	//--------------------------------------------------------------
	
	CdSuList<int> ::iterator listIter;
	
	//--------------------------------------------------------------
	// Test #9.1 inserting element to empty list, before begin.
	//--------------------------------------------------------------
	printf ("\nTest Case #9.1\n");
	list.insert (list.begin (), 1);
	if (list.size () == 1 && *list.begin () == 1
		&& list.begin () != list.end ()
		&& ++list.begin () == list.end ()
		&& ++list.end () == list.begin ())
	{
		printf ("Test Case #9.1 is passed\n");
	} else
	{
		printf ("Test Case #9.1 is failed\n");
	} // if
	
	//--------------------------------------------------------------
	// Test #9.2 inserting element to empty list, before end .
	//--------------------------------------------------------------
	list.clear (); 
	printf ("\nTest Case #9.2\n");
	list.insert (list.end (), 1);
	if (list.size () == 1 && *list.begin () == 1
		&& list.begin () != list.end ()
		&& ++list.begin () == list.end ()
		&& ++list.end () == list.begin ())
	{
		printf ("Test Case #9.2 is passed\n");
	} else
	{
		printf ("Test Case #9.2 is failed\n");
	} // if
	
	//--------------------------------------------------------------
	// Test #9.3 inserting element when there is one element, 
	// before begin
	//--------------------------------------------------------------
	printf ("\nTest Case #9.3\n"); 
	list.insert (list.begin (), 2);
	if (list.size () == 2 && list.begin () != list.end () 
			&& *list.begin () == 2 
			&& *(++list.begin ()) == 1 
			&& ++list.end () == list.begin ())
	{
		printf ("Test Case #9.3 is passed\n");
	} else
	{
		printf ("Test Case #9.3 is failed\n");
	} // if

	//--------------------------------------------------------------
	// Test Case #9.4 inserting element where there is one element, 
	// before end 
	//--------------------------------------------------------------
	printf ("\nTest Case #9.4\n"); 
	list.clear ();
	list.insert (list.end (), 1);
	list.insert (list.end (), 2);
	if (list.size () == 2 && list.begin () != list.end () 
			&& *list.begin () == 1 
			&& *(++list.begin ()) == 2 
			&& ++list.end () == list.begin ())
	{
		printf ("Test Case #9.4 is passed\n");
	} else
	{
		printf ("Test Case #9.4 is failed\n");
	} // if

	//--------------------------------------------------------------
	// Test Case #9.5 inserting element between two element
	//--------------------------------------------------------------
	printf ("\nTest Case #9.5\n");
	list.insert (++list.begin (), 3);
	if (list.size () == 3 && list.begin () != list.end ()
			&& *list.begin () == 1 
			&& *(++list.begin ()) == 3
			&& *(++++list.begin ()) == 2
			&& ++list.end () == list.begin ())
	{
		printf ("Test Case #9.5 is passed\n");
	} else
	{
		printf ("Test Case #9.5 is failed\n");
	}
	
	//--------------------------------------------------------------
	// Test #10.1 : Deleting from begin to end when list is empty
	//--------------------------------------------------------------
	printf ("\nTest Case #10.1 \n");
	if (list.erase (list.begin (), list.end ()) == list.end () &&
			list.size () == 0)
	{
		printf ("Test Case #10.1 is passed\n");
	} else
	{
		printf ("Test Case #10.1 is failed\n");
	}
	
	//--------------------------------------------------------------
	// Test #10.2 Deleting from begin to begin when there is one element 
	// in list.
	//--------------------------------------------------------------
	printf ("\nTest Case #10.2\n");
	list.insert (list.begin (), 1);
 	// printf ("Before erase\n");
	// printList (list);
	listIter = list.erase (list.begin (), list.begin ());
	// printf ("After erase\n");
	// printList (list);
	if (listIter == list.begin () &&
		list.begin () != list.end () && 
		list.size () == 1 && *list.begin () == 1)
	{
		printf ("Test Case #10.2 is passed\n");
	} else
	{
		printf ("Test Case #10.2 is failed\n");
	} // if 

	//--------------------------------------------------------------
	// Test #10.3 deleting element of list which has single 
	// element
	//--------------------------------------------------------------
	printf ("\nTest Case #10.3\n");
	listIter = list.erase (list.begin (), list.end ());

	if (list.begin () == list.end () && list.size () == 0 &&
			listIter == list.end ())
	{
		printf ("Test Case #10.3 is passed\n");
	} else
	{
		printf ("Test Case #10.3 is failed\n");
	} // if ()
	
	//--------------------------------------------------------------
	// Test #10.4 Erase n element in list with more than n + 1 
	// 	element
	//--------------------------------------------------------------
	printf ("\nTest Case #10.4\n");
	CdSuList<int>::iterator pos1;
	CdSuList<int>::iterator pos2;
	pos2 = list.insert (list.begin (), 5); 
	list.insert (list.begin (), 4); 
	list.insert (list.begin (), 3); 
	pos1 = list.insert (list.begin (), 2); 
	list.insert (list.begin (), 1);
	list.erase (pos1, pos2);
	if (list.size () == 2 &&
		*list.begin () == 1 &&
		*(++list.begin ()) == 5)
	{
		printf ("Test Case #10.4 passed\n");
	} else
	{
		printf ("Test Case #10.4 failed\n");
	} // if ()
	
	//--------------------------------------------------------------
	// Test #11.1  Copy Constructor when list is not empty
	//--------------------------------------------------------------
	printf ("Testing Copy Constructor\n");
	forCheckingCopyConstructor (list);
	
	//--------------------------------------------------------------
	// Test #11.2  Copy Constructor when list is empty
	//--------------------------------------------------------------
	list.clear ();
	forCheckingCopyConstructor (list);
}

//==============================================================================
// <End of cdsulist-test.cpp
//==============================================================================
