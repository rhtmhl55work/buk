//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: mbhash-test.cpp
// Description	: Tests for CdSuMultiByteHash.
// Ref Docs	:
// Author	: Seema Chandak
// Created On   : 
// Last Modified: Thu Jun 19 18:36:00 IST 2003
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cdsuutiltypes.h"
#include "cdsutrace.h"
#include "cdsulist.h"
#include "cdsumbhash.h"


//----------------------------------------------------------------------------
// Enumerations and Typedefs.
//----------------------------------------------------------------------------
enum
{
	TEST_CALLHLR_ID_LEN = 16
};

//----------------------------------------------------------------------------
// Class Definitions.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Multibyte Hash key: TestBpTrasId.
//----------------------------------------------------------------------------

struct  TestCallHlrId
{
	TestCallHlrId () {}
	TestCallHlrId (char* callId)
	{
		if (callId == NULL)
			return;
		else 
		{
			memcpy (callHlrId, callId, TEST_CALLHLR_ID_LEN);
		}
	}
	void operator = (const TestCallHlrId chId)
	{
		memcpy (callHlrId, chId.callHlrId, TEST_CALLHLR_ID_LEN);
	}
	char callHlrId [TEST_CALLHLR_ID_LEN];

}; // struct TestCallHlrId

struct TestCallHlr 
{
	TestCallHlr () {}
	TestCallHlr (TestCallHlrId chid)
	{
		callHlrId = chid;
		callHlrNo = 0;
	}
	TestCallHlrId callHlrId;
	int callHlrNo;

}; // struct TestCallHlr

struct TestBpTransId
{
	TestBpTransId () {}

	TestBpTransId (unsigned int servHlrId, unsigned int txnSeq)
	{
		serviceHlrId = servHlrId;
		txnSeqNo = txnSeqNo;
	}

	unsigned int serviceHlrId;
	unsigned int txnSeqNo;

}; // struct TestBpTransId

char* stringAllocator ();

CdSuMultiByteHash<int>* multiByteHash =
			new CdSuMultiByteHash <int>
			(CdSuMultiByteHash<int>::TZ_251, sizeof (TestBpTransId));

			
void forCheckingCopyConstructor (CdSuMultiByteHash<int> mbh)
{
//-----------------------------------------------------------------------------
// NOTE: To do very perfect test of copy constructor Following
// for Loop must be uncommented. To do key () interface must be added to
// iterator. 
//-----------------------------------------------------------------------------
	bool flag = true;
	CdSuMultiByteHash<int>::iterator iter1 = multiByteHash->begin ();
/*	for (;iter1 != multiByteHash->end (); ++ iter1)
	{
		int i; 
		if (mbh.lookup (iter1.key (), i) != true || *iter1 != i)
		{
			flag = false;
			break;
		}
	}
*/	
	CdSuMultiByteHash<int>::iterator iter2 = mbh.begin ();
	iter1 = multiByteHash->begin ();
	int i = 0;
	for (; iter2 != mbh.end (); ++iter2, ++iter1)
	{
		printf ("Copied Value %d, Old Value %d [%d]\n", *iter2, *iter1, i);
		++i;
	}
	TestBpTransId tid1 (0x111111, 111111);	
	mbh.insert (&tid1, 45);
	int val;
	if (flag == false || multiByteHash->size () != (mbh.size () - 1) || multiByteHash->lookup (&tid1, val) == true)
	{
		printf ("Copy constructor is not proper\n");
	} else
	{
		printf ("Copy Constructor is proper \n");
	}
	
}

int main ()
{
	CdSuMultiByteHash<int> multiByteHashObj 
			(CdSuMultiByteHash<int>::TZ_251, sizeof (TestBpTransId));

	printf ("Size of CdSuMultiByteHash: %d\n", sizeof (multiByteHashObj));

	TestBpTransId tid1 (0x102030, 1);	
	TestBpTransId tid2 (0x102031, 2);	
	TestBpTransId tid3 (0x102032, 3);	
	TestBpTransId tid4 (0x102033, 4);
	
	//--------------------------------------------------------------------
	// Test #1, successful insert
	//--------------------------------------------------------------------

	printf ("Test #1 ");
	if (multiByteHash->insert (&tid1, 1000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	printf ("Test #1 ");
	if (multiByteHash->insert (&tid2, 2000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	printf ("Test #1 ");
	if (multiByteHash->insert (&tid3, 3000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}
	
	//--------------------------------------------------------------------
	// Test #2, unsuccessful insert
	//--------------------------------------------------------------------
	printf ("Test #2 ");
	if (multiByteHash->insert (&tid1, 5000))
	{
		printf ("insert success\n");
	} else
	{
		printf ("insert fail\n");
	}

	//--------------------------------------------------------------------
	// Test #3, successful lookup 
	//--------------------------------------------------------------------
	printf ("Test #3 ");
	int val;
	if (multiByteHash->lookup (&tid3, val))
	{
		printf ("lookup success, val %d\n", val);
	} else
	{
		printf ("lookup fail\n");
	}

	//--------------------------------------------------------------------
	// Test #4, unsuccessful lookup 
	//--------------------------------------------------------------------
	printf ("Test #4 ");
	if (multiByteHash->lookup (&tid4, val))
	{
		printf ("lookup success, val %d\n", val);
	} else
	{
		printf ("lookup fail\n");
	}

	//--------------------------------------------------------------------
	// Test #5, successful remove
	//--------------------------------------------------------------------
	printf ("Test #5 ");
	if (multiByteHash->remove (&tid2))
	{
		printf ("remove success\n");
	} else
	{
		printf ("remove fail\n");
	}  

	//--------------------------------------------------------------------
	// Test #6, unsuccessful remove
	//--------------------------------------------------------------------
	printf ("Test #6 ");
	if (multiByteHash->remove (&tid2))
	{
		printf ("Remove success\n");
	} else
	{
		printf ("Remove fail\n");
	}
	
	//--------------------------------------------------------------------
	// Test #10.1, Copy Constructor when table is not empty
	//--------------------------------------------------------------------
	forCheckingCopyConstructor (*multiByteHash);
	
	//--------------------------------------------------------------------
	// Test #7, iterator.
	//--------------------------------------------------------------------
	printf ("Test #7 ");
	CdSuMultiByteHash<int>::iterator iter;
	iter = multiByteHash->begin ();
	for ( ; iter != multiByteHash->end (); ++iter)
		printf ("Value: %d\n", *iter);
	
	//--------------------------------------------------------------------
	// Test #10.2, Copy Constructor when table is not empty
	//--------------------------------------------------------------------
	multiByteHash->clear ();
	forCheckingCopyConstructor (*multiByteHash);
	
	delete multiByteHash;
	//char c;
	//printf ("Enter some characte to continue testing\n");
	//scanf ("%c", &c);

	//--------------------------------------------------------------------
	// Test #8, distribution test.
	//--------------------------------------------------------------------
	printf ("Test #8\n");
	CdSuMultiByteHash<int>* multiByteHash1  = new CdSuMultiByteHash<int>
			(CdSuMultiByteHash<int>::TZ_509, sizeof (TestBpTransId));
	TestBpTransId transId;
	transId.serviceHlrId = 0x102030;
	transId.txnSeqNo = 0;
	
	for (int i = 0; i < 6000; ++i)
	{
		transId.txnSeqNo = i;
		multiByteHash1->insert (&transId, i);
	}

	TestBpTransId searchTransId;
	searchTransId.serviceHlrId = 0x102030;
	searchTransId.txnSeqNo = 0;
	
	int searchVal;
	for (int i = 0; i < 6000; ++i)
	{
		searchTransId.txnSeqNo = i;		
		if (multiByteHash1->lookup (&searchTransId, searchVal) == false)
		{
			CDSU_TRACE (1, "Test #8 : Multibyte Hash lookup failed\n");
		}
	}

	multiByteHash1->print ();

	//--------------------------------------------------------------------
	// Test #7 iterator.
	//--------------------------------------------------------------------
	printf ("Test #7");
	CdSuMultiByteHash <int>::iterator iter1;
	iter1 = multiByteHash1->begin ();
	for (; iter1 != multiByteHash1->end (); ++iter1)
	{
		printf (" Value : %d\n", *iter1);
	}
	
	
	//--------------------------------------------------------------------
	// Test #8, size and clear
	//--------------------------------------------------------------------
	printf ("Test #8\n");
	printf ("Size : %d\n", multiByteHash1->size ());
	multiByteHash1->clear ();
	printf ("Size : %d\n", multiByteHash1->size ());
	CdSuMultiByteHash<int>::iterator iter2;
	iter2 = multiByteHash1->begin ();
	for (; iter2 != multiByteHash1->end (); ++iter2)
	{
		printf (" Value : %d\n", *iter2);
	}
	
	delete multiByteHash1;

	//--------------------------------------------------------------------
	// Test #9. Different Hash Key Test.
	// Description: Generate few callId (strings) keys. Create some CallHlr
	//		objects with the keys. Store the keys in a list. Get 
	//		callHlr ids from the list. Lookup for the entry in
	//		the hash table and remove it.
	//--------------------------------------------------------------------
	CdSuMultiByteHash<TestCallHlr*>* callHlrHash =
			new CdSuMultiByteHash <TestCallHlr*>
			(CdSuMultiByteHash<TestCallHlr*>::TZ_251, sizeof (TestCallHlrId));
	CdSuList <TestCallHlrId*> list;	
	for (int i = 0; i < 251; i++)
	{
		char* key = stringAllocator ();
		TestCallHlrId* callHlrId = new TestCallHlrId (key);
		list.push_front (callHlrId);
		TestCallHlr* callHlr = new TestCallHlr (*callHlrId);
		if (callHlrHash->insert (callHlrId, callHlr) == false)
		{
			printf ("Test Case Fail\n");
		} else 
		{
			callHlr->callHlrNo = i;
			printf  ("Inserted CallHlr No: %d\n", callHlr->callHlrNo);
		}
	}
	callHlrHash->print ();
	CDSU_TRACE1 (1, "Size of List is %d\n", list.size ());
	for (int i = 0; i < 251; i++)
	{
		TestCallHlrId* callHlrId = list.back ();		
		list.pop_back ();
		TestCallHlr* callHlr = NULL;
		if (callHlrHash->lookup (callHlrId, callHlr) == false)
		{
			printf ("Test Case Fail\n");
		} else
		{
			printf (" Lookup CallHlr No: %d\n", callHlr->callHlrNo);
			if (callHlrHash->remove (callHlrId) == false)
			{
				printf ("Test Case Fail\n");
				return 0;
			}
			delete callHlr;
		}
		
	}
	
	printf ("Exitting: Size of List is %d\n", list.size ());

} // main ()

char* stringAllocator ()
{
	char* callId = new char [TEST_CALLHLR_ID_LEN];
	
	for (int i = 0; i < TEST_CALLHLR_ID_LEN; ++i)
	{
		callId [i] = '\0';
	}

	for (int i = 0; i < TEST_CALLHLR_ID_LEN; ++i)
	{
		Uint8 range = Uint8 ((char)'z' - (char)'a');
		callId [i] = 'a' + (rand () % range);
	}

	return callId;


}; // char* stringAllocator ()

//==============================================================================
// <End of mbhash-test.cpp>
//==============================================================================
