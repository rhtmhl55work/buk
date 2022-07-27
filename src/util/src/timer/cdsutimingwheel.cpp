//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsutimingwheel.cpp
// Description  : TimingWheelHash functions.
// Ref Docs     :
// Author       : Indu Mishra
// Contributor  :
// Created On   : 
// Modified	: 27th March, 2003
// Modified By  : Indu Mishra
// Modified	: ~CdSuTimingWheel was deleting one more than actual size of
// 		Hash. So '=' sign removed.
// Next Modified: 07th April, 2003
// Modified By  : Indu Mishra
// Modified     : Negative problem solved of counters.Changed code for
// 		adding node in last, middle and end of double linked list.
// Modified	: 10th April, 2003
// Modified By	: Indu Mishra
// Modified 	: add function changed, constructor and destructor changed
// 		for creation/destruction of timingwheel hash.deleteTnode
// 		function changed, tickDecCount changed.
// Next Modified:Wed Dec 16 15:01:52 IST 2009
// Modified By  :Anish, Arun
//
//============================================================================

//-----------------------------------------------------------------------------
//	Include Files
//-----------------------------------------------------------------------------
#include "cdsutimingwheel.h"

//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel::CdSuTimingWheel ()
// Description  : Constructor.
// Inputs       : NIL.
// Return Value : NIL.
// Side Effects : 
// Algorithm    : Used to create an array of type CdSuTnode and initialise
// it to NULL . 
//---------------------------------------------------------------------------

CdSuTimingWheel::CdSuTimingWheel (CdSuTimerHandler* th, Uint32 hts)
{
	CdSuTimerInfo ti;
	memset (&ti, sizeof (ti), 0);
	
	hashTableSize = hts;

	for (Uint32 i = 0; i < hashTableSize; i++)
	{
		//Pointer is set to NULL initially.	
		hashTable[i] = (CdSuTnode*)NULL;   
	}
	
	timerHandler = th;

	currentPosition = 0;
	
} // CdSuTimingWheel::CdSuTimingWheel ()

//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel::~CdSuTimingWheel ()
// Description  : Destructor. 
// Inputs       : NIL.
// Return Value : NIL.
// Side Effects : 
// Algorithm    : Used to delete all nodes and then finally itself.
//---------------------------------------------------------------------------
CdSuTimingWheel ::~CdSuTimingWheel ()
{
	
	CdSuTnode* tNode = NULL;
	CdSuTnode* p = NULL;

	for (Uint32 i = 0; i < hashTableSize; i++)
	{
		tNode = hashTable [i];

		// Deleting all the elements in the list.
		while (tNode != (CdSuTnode* )NULL)
		{
			p = tNode->next;
			delete tNode;
			tNode = NULL;
			tNode = p;
		}	 
		hashTable [i] = (CdSuTnode*)NULL;

	} // for
	
} // CdSuTimingWheel ::~CdSuTimingWheel ()


//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel::add ()
// Description  : Add a Request to the Hash Bucket according to the time
// 		  desired .
// Inputs       : CdSuTimerInfo  : InfoStructure of the calling timer.
//		  CdSuTimerId    : timerId.
// Return Value : Address of the node added to Hash bucket.
// Side Effects : 
// Algorithm    : 
//
// Total no: of ticks is calculated from timerInfo structure.
// The Ticks calculated are used to hashed to a position in
// CdSuTimingWheel.Then depending upon the position whether its first node
// or last node or middle node it is added to the double linked list.The
// double linked list is scanned using the next and previous pointers till
// the tickCount becomes greater than the previous nodes tickCount.Then the node is inserted 
// tickCount is the % of numTicks and size of CdSuTimingWheel.
//
// The insertion part is handled in four parts:
//              1. Hash Bucket /double linked list empty 
//              2. Add to the beginning of Double linked list.
//              3. Add to the end of Double linked list.
//              4. Add into the middle of Double linked list.
//---------------------------------------------------------------------------

CdSuTnode* CdSuTimingWheel::add (const CdSuTimerInfo& timerInfo, CdSuTimerId& timerId )
{
	//CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	//--------------------------------------------------------------------
        // Do the required calculations.
	//	* Translate the (UnitType, Count) into absolute ticks.
	//	* Find the "offset" from current position.
	//	* Find the "number of loops" to be done in the timing Wheel.
	//--------------------------------------------------------------------
	// Absolute Ticks.
        Uint32 ticks = calcNumTicks (timerInfo.timeUnits, timerInfo.timeCounts);
		
	// Number of loops.
	Uint32 noOfCycles = ticks / hashTableSize; 
	
	// Offset from current position.
	Uint32 offset = hashFunction1(ticks);
	
	//--------------------------------------------------------------------
	// Create a new node which will be inserted into the timing Wheel.
	//--------------------------------------------------------------------
	CdSuTnode* newNode = new CdSuTnode (timerInfo, timerId, noOfCycles);
	//CDSU_TRACE1(4, "New Node%d\n",(Uint32)newNode);
	
	// Set Tnode's tickCount to noOfCycles.
	newNode->tickCount = noOfCycles;
	
	// Assign determined hash position to Tnode offset.
	newNode->offset = offset;
	
	//--------------------------------------------------------------------
	// * Locate the list in the timing wheel and get the list.
	// * Insert the node into the list. The possibilities are:
	//	* List was empty
	//		+ Insert at beginning.
	//	* List was not empty:
	// 		* Locate position in the list and either
	//			+ "Insert at the end" or 
	//			+ "Insert at beginning" or
	//			+ "Insert in the middle"
	//--------------------------------------------------------------------

	// Locate the list in the timing wheel.
	CdSuTnode* scanPos = hashTable[offset];
	
	// Since we are maintaining a "differential queue", we need a counter 
	// to get the absolute number of ticks which are due when we pass 
	// through the list.
        Uint32 ticksTotal = 0;	

	// Check if list is empty.
	if (scanPos == NULL)
	{
		//CDSU_TRACE(4, "ADD : Empty List\n");
		// Timing request list is empty.
		hashTable[ offset ] = newNode;
		newNode->tickCount = noOfCycles;
		newNode->previous = NULL;
		newNode->next = NULL;
		return (newNode);
        } // if List was empty.

	//--------------------------------------------------------------------
	// List was not empty. Locate the position for this node in the 
	// sorted list.
	//--------------------------------------------------------------------
	
	// Will be set to false, once a position is found.
	bool brkCond = false;
	
	while (!brkCond)
        {
                Uint32 tempTotal = ticksTotal + scanPos->tickCount;
                if (tempTotal > noOfCycles)
		{
			scanPos = scanPos->previous;
			brkCond = true;
		} else
		{
			// Increment the ticks total
                	ticksTotal = tempTotal;

			// Check whether this is the last node.
			if (scanPos->next == NULL)
			{
				brkCond = true;
			} else
			{
				// go to the next node
				scanPos = scanPos->next;
			}
		}
        } // while.


	//--------------------------------------------------------------------
	// Position in the list located ("scanPos" contains the pointer to 
	// the node behind which the "new node" is to be inserted). Now to 
	// adjust the tickcount and insert node with it before scanPos. 
	// 
	// Take care of the three possibilities:
	//	Insert at (Begin/End/Middle).
	//--------------------------------------------------------------------
	if (scanPos == NULL)
	{
		//CDSU_TRACE(4, "ADD : Inserting node at Start of list:\n");
		// Inserting node at start of list.
		newNode->next = hashTable[ offset ];
		(newNode->next)->previous = newNode;
        	hashTable[offset] = newNode;
		newNode->previous = NULL; 

		// Update the tick count of the new Node
        	newNode->tickCount = noOfCycles;

		// Update the next nodes tickCount also.
        	newNode->next->tickCount -= noOfCycles;
		
	  	return (newNode);
	}else if (scanPos->next == NULL)
	{
		//CDSU_TRACE(4, "ADD : Inserting node at END of List\n");
		// Add new node to end of list.
	        newNode->tickCount = noOfCycles - ticksTotal;
		scanPos->next = newNode;
                newNode->previous = scanPos;
                newNode->next = NULL;

		return (newNode);

	} else
	{
		
		// Inserting node at the middle.
		//CDSU_TRACE (4, "ADD : Inserting node at Middle of List\n");
		
		// Adjust the pointers the node ahead
        	newNode->next = scanPos->next;
		(newNode->next)->previous = newNode;

		// Adjust the pointers to the node behind
		newNode->previous = scanPos;
        	scanPos->next = newNode;

        	newNode->tickCount = noOfCycles - ticksTotal;
		(newNode->next)->tickCount =(newNode->next)->tickCount
			- (newNode->tickCount);
		return (newNode);
		
	} // if

	// For removing compiler warnings.
	return (newNode);

} // CdSuTimingWheel::add ()


//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel:: del()
// Description  : Cancel Timer specified by the TimerId. 
// Inputs       : Address of the Timer node to be deleted.
// Return Value : bool.
// Side Effects : 
// Algorithm    : Used to delete a node and then finally returns true else
// false.
//---------------------------------------------------------------------------

bool CdSuTimingWheel::del (CdSuTnode* value)
{
	//CDSU_TRACE2 (4, "%s: value = %d\n", __PRETTY_FUNCTION__, value);
	
        deleteTnode (value);
        
	return (true);

} // CdSuTimingWheel::del ()


//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel:: timeoutHandler()
// Description  : On timeout, check the TimerOutReportType.If it is of type 
// message then call CdSuUserTimeoutHlr() to pass TimeOut structure.Else call 
// CallBack function .
// Inputs       : CdSuTnode node.
// Return Value : NIL.
// Algorithm :
//      Get the first node in the double linked list ,extract the fields 
//      (Timerid, pid,userinfo,opcode) and send them to that destinationId.
// --------------------------------------------------------------------------

void CdSuTimingWheel::timeoutHandler (CdSuTnode* node)
{
	// CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuTimeout tempTimeOut;
	
	tempTimeOut.timerId = node->timerId;
	tempTimeOut.destModuleId = node->timeoutDestId;
	tempTimeOut.opcode = node->opcode;

	for (Uint8 i = 0; i < (Uint8) CDSU_TMR_MAX_USERINFO_NUM; ++i)
	{
		tempTimeOut.userInfo [i] = node->userInfo [i];
	}

	//if callback function is required	
	if(node->timeoutReportType == CDSU_TMR_TRT_CALLBACK)
	{
		node->timerCallBackFn (tempTimeOut);
		
	} else if (node->timeoutReportType == CDSU_TMR_TRT_MESSAGE)
	{
		//call user timeout hlr function and pass timeOut fn.
		//cdSuUserTimeoutHlr (tempTimeOut);
	}
	
} // Timeque::timeoutHandler ()

//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel::tickDecCount ()
// Description  : For all CdSuTnodes whose timerCounts are made 0, send msg to the 
//                corresponding destination. Move to the next 
// Inputs       : NIL.
// Return Value : NIL.
// Side Effects : 
// Algorithm    : 
//
//	TODO: Should the increment happen before or after cleaning up the list?
//	Increment the timingWheel's currentPosition.
//	if (timer service list is not empty) 
//      	decrement the number of ticks in the first node;
// 		while (tickCount == 0) 
//                      call the relevant function with its parameters.
//                      remove this node from the list;
//		endwhile
//	endif   
//
// -----------------------------------------------------------------------------

void CdSuTimingWheel::tickDecCount (void)
{
	// CDSU_TRACE(4, "CdSuTimingWheel : tickDecCount\n");
	bool isCyclic = false;
	if (hashTable[ currentPosition ] == NULL)
	{
		currentPosition = (currentPosition + 1) % hashTableSize;
		return;
	}

	// Check for first node timeCount. If 0 delete it. 
	CdSuTnode* p = hashTable[ currentPosition ];
	
	while(p->tickCount == 0)
	{
		timeoutHandler (p);
		CdSuTimerInfo timerInfo;
		CdSuTimerId timerId;
		
		//check for Cyclic Timers
		if(p->timerType == CDSU_TMR_TT_CYCLIC)
		{
			//CDSU_TRACE(4, "CdSuTimingWheel :CYCLIC TIMER\n");
			//create a struct CdSuTimerInfo  
			//copy values of node to it.then delete node
			timerInfo.timeUnits = p->initTimeUnits;
			timerInfo.timeCounts = p->initTimeCounts;
			timerInfo.destModuleId = p->timeoutDestId;
			timerInfo.opcode = p->opcode;
			for (Uint8 i = 0; i < (Uint8) CDSU_TMR_MAX_USERINFO_NUM; ++i)
			{
				timerInfo.userInfo [i] = p->userInfo [i];
			}
			timerInfo.timerType = p->timerType;	
			timerInfo.timeoutReportType = p->timeoutReportType;
			timerInfo.timerCallBackFn = p->timerCallBackFn;

			//copy TimerId
			timerId = p->timerId;
			//set bool variable
			isCyclic = true;

			//check for return value of deletion of node.
			if(timerHandler->timerHash->remove 
					(p->timerId))
			{
				//CDSU_TRACE(4, "1. Success\n");		
			}else
			{
				CDSU_TRACE1(1,"%s: Sorry! 1.Node Could Not Be Deleted\n",__PRETTY_FUNCTION__);
				return;
			}
			//CDSU_TRACE1(4, "Address of p before deleteNode%d\n",p);
			deleteTnode (p);

			//if cyclic then create a new node with those values .
			if(isCyclic)
			{
				//CDSU_TRACE(4, "Creating a New Cyclic Node\n");
				CdSuTnode* node_added = add(timerInfo, timerId);
				//add in Hash also.
				//CDSU_TRACE1(4, "Address of New Node Added%d\n",node_added);
				
				if(timerHandler->timerHash->
					insert (timerId, node_added))
				{
					// CDSU_TRACE(4, "Timer inserted\n");		
				}else	
				{
					CDSU_TRACE(4, "Sorry ,Cyclic Node could not be added in the Hash\n");
					return;
				}
				
			}// if ()

			
		} // if ()
		else {

			// Handles the case of non-cyclic timers.
			// Delete the element and the entry in the hash.
			//check for return value of deletion of node.
			if(timerHandler->timerHash->
					remove (p->timerId))
			{
				//CDSU_TRACE(4, "3. Success\n");
			}else
			{	
				CDSU_TRACE1 (4,"%s: Sorry! 2.Node Could Not Be Deleted\n",
					__PRETTY_FUNCTION__);
				return;
			}
			deleteTnode (p);

		}// else

		// Get next node of List.
		p = hashTable[currentPosition];
					
		// Advance to the next Node
		if (p == NULL)
		{
			//CDSU_TRACE (4, "No More Elements in List\n");
			// No more elements in the list.
			currentPosition = (currentPosition + 1) % hashTableSize;
			return;
		}
		
	}// while ()
	
	// After deleting first node decrement timeCount.
	p->tickCount--;
	
	currentPosition = (currentPosition + 1) % hashTableSize;
		
} // CdSuTimingWheel::tickDecCount ()


//----------------------------------------------------------------------------
// Function     : calcNumTicks
// Description  : To calculate the  number of ticks in units of 10ms.
// Input        : TimeUnitsType 
//                TimeCountType 
// Output       : Uint32 
// Algorithm :
//      Calculate the number of clocks ticks using units, number of units.
//      Since timer is programmed for 10mS, each tick is of value 10mS.
//      Same count is used on pc, though this will give only relative time 
//      sense. MILLISECOND has been removed, since only values in multiples 
//      of 10mS is possible.
//----------------------------------------------------------------------------

Uint32 CdSuTimingWheel::calcNumTicks (Uint32 timeUnits, Uint32 timeCounts)
{
	// Basic Tick is of 10ms.
	Uint32 retval = 0;

        switch (timeUnits)
        {
        case CDSU_TMR_TU_TENMS:
               retval = timeCounts;
               break;

        case CDSU_TMR_TU_HUNDREDMS:
                retval = timeCounts * 10 ;
		break;

        case CDSU_TMR_TU_SECOND:
		retval = (timeCounts * 100);
		break;

        case CDSU_TMR_TU_MINUTE:
                retval = (timeCounts * 6000);
                break;

        case CDSU_TMR_TU_HOUR:
                retval = (timeCounts * 360000);
                break;

        default:
                break;
        }

        return (retval);

} // CdSuTimingWheel::calcNumTicks ()


//----------------------------------------------------------------------------
// Function     : deleteTnode
// Description  : To delete a CdSuTnode of given address from CdSuTimingWheel
// Input        : Address to that Tnode.
// Output       : Nil.
// Algorithm :
//      Node is deleted depending on whether its,
//              1.  null
//              2.  the only node
//              3.  the first node
//              4.  the last node
//              5.  one of the middle nodes.
//     Before deleting update the "tickCount" in the next node, if it exists.
//----------------------------------------------------------------------------

void CdSuTimingWheel::deleteTnode (CdSuTnode* p)
{
	//CDSU_TRACE (4, "CdSuTimingWheel::deleteTnode\n");
	if (p == NULL)
                return;
	
	CdSuTnode* p_ev = p->previous;
        CdSuTnode* n_ev = p->next;
	
	if (p_ev == NULL)
        {
                if (n_ev == NULL)
                {   
			//CDSU_TRACE (4, "Single Node\n");
			
			//Set hashTable to NULL since no node exists then.
			hashTable[p->offset] = NULL;
			
			// Only a single node present
			delete(p);
								
		} else
                {       
			//CDSU_TRACE (4, "Many Events, First Node\n");
			// Many events present. Delete the first.
			// Assign next node to hashTable[offset].
			hashTable[p->offset] = n_ev;
                        n_ev->previous = NULL;
                        n_ev->tickCount += p->tickCount;
			delete(p);
		}// else
        } else
        {
                if (n_ev == NULL)
                {       
			//CDSU_TRACE (4, "Many Events, Last Node\n");
		        // Many events. Delete the last.
			p_ev->next = NULL;
			delete(p);
                } else
                {      
			//CDSU_TRACE (4, "Many Events, Middle Node");
		        // Many events. Delete from middle.
			p_ev->next = n_ev;
                        n_ev->previous = p_ev;
                        n_ev->tickCount += p->tickCount;
			delete(p);
                }// else
        }// else

} // CdSuTimingWheel::deleteTnode ()


//---------------------------------------------------------------------------
// Name		: CdSuTimingWheel ::hashFunction1 ()
// Description  : This is the function implementing the hashing. 
// Inputs       : char*.
// Return Value : int quotient .
// Side Effects : 
// Algorithm    : Using the start position of array we add to it keyVal and
// then divide it by size of Hash to get hashFunction.
//---------------------------------------------------------------------------
 
Uint32 CdSuTimingWheel::hashFunction1 (Uint32 keyValue)
{
	Uint32 hv;

	// CurrentPosition is the position of start pointer.
	hv = (keyValue + currentPosition) % hashTableSize;  
	return (hv);
	
}// hashFunction1 ()

//=============================================================================
// <End of CdSuTimingWheel.cpp>
//=============================================================================

