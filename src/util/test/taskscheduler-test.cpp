//==============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: taskscheduler-test.cpp
// Description	: Tests for taskscheduler.
// Ref Docs	:
// Author	: Seema Chandak.
// Created On   : Thu Jun 19 10:26:09 IST 2003
// Last Modified:
//==============================================================================

#include "cdsutaskscheduler.h"

//----------------------------------------------------------------------------
// class	: TesterTask1
// Description	: Definitions for class TesterTask1 publicly inherited from
// 		  class Task.
// Notes	: 
//----------------------------------------------------------------------------

class TesterTask1 : public CdSuTask
{
public:
	TesterTask1 () {}
	~TesterTask1 () {}

	void handleMsg (CdTaskMessage* msg);

}; // class TesterTask1

//----------------------------------------------------------------------------
// class	: TesterTask2
// Description	: Definitions for class TesterTask2 publicly inherited from
// 		  class Task.
// Notes	: 
//----------------------------------------------------------------------------

class TesterTask2 : public CdSuTask 
{
public:
	TesterTask2 () {}
	~TesterTask2 () {}

	void handleMsg (CdTaskMessage* msg);

}; // class TesterTask2

//---------------------------------------------------------------------------
// Name		: TesterTask1::handleMsg ()
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void TesterTask1::handleMsg (CdTaskMessage* msg)
{
	printf ("Tester Task1 has received msg\n");
	delete msg;

} // TesterTask1::handleMsg ()

//---------------------------------------------------------------------------
// Name		: TesterTask2::handleMsg ()
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------


void TesterTask2::handleMsg (CdTaskMessage* msg)
{
	printf ("Tester Task 2 has received Msg\n");
	delete msg;

} // TesterTask2::handleMsg ()

//---------------------------------------------------------------------------
// Name		: main
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

int main ()
{
	CdSuTrace::setLevel (5);

	CdSuTsQueue <CdTaskMessage*>* msgQueue = new CdSuTsQueue
					<CdTaskMessage*> (20);

	CdSuTsQueue <CdTaskMessage*>* msgQueue2 = new CdSuTsQueue
					<CdTaskMessage*> (20);

	// Create two TaskScheduler objects.
	CdSuTaskScheduler* ts = new CdSuTaskScheduler (msgQueue, 3);
	CdSuTaskScheduler* ts1 = new CdSuTaskScheduler (msgQueue2, 3);
	
	// Create Five Task Objects.

	// These 3 tasks will be added to ts.
	TesterTask1* task1 = new TesterTask1;
	TesterTask2* task2 = new TesterTask2;
	TesterTask1* task3 = new TesterTask1;
	TesterTask2* taskMax = new TesterTask2;
	TesterTask1* nullTask = NULL;

	
	// These 2 tasks will be added to ts1.
	TesterTask1* task4 = new TesterTask1;
	TesterTask2* task5 = new TesterTask2;
	TesterTask1* task6 = new TesterTask1;
	TesterTask2* task7 = new TesterTask2;
	
	// Successful addition of tasks to TaskScheduler ts.
	printf("Test Case#1\n");
	if ((ts->addTask (task1)) == CDSU_TID_INVALID)
		printf ("Add Fail. Test Case Fails.\n");
	if ((ts->addTask (task2)) == CDSU_TID_INVALID)
		printf ("Add Fail. Test Case Fails.\n");
	if ((ts->addTask (task3)) == CDSU_TID_INVALID)
		printf ("Add Fail. Test Case Fails.\n");
	printf("Test Case#2\n");
	if ((ts->addTask (taskMax)) != CDSU_TID_INVALID)
		printf ("Adds tasks more than the max no specified. Test Case Fails.\n");
	if ((ts->addTask (nullTask)) != CDSU_TID_INVALID)
		printf ("Adds NULL Task. Test Case Fails.\n");

	// Successful addition of tasks to TaskScheduler ts1.
	if ((ts1->addTask (task4)) == CDSU_TID_INVALID)
		printf ("Add Fail. Test Case Fails.\n");
	if ((ts1->addTask (task5)) == CDSU_TID_INVALID)
		printf ("Add Fail. Test Case Fails.\n");
	
	// Successful removal of task from TaskScheduler ts1.
	printf("Test Case#5\n");
	if (ts1->removeTask (task4->getTaskId ()) == false)
		printf ("Remove Task Fail. Test Case Fails.\n");

	// Successful removal of task from TaskScheduler ts1.
	if (ts1->removeTask (task5->getTaskId ()) == false)
		printf ("Remove Task Fail. Test Case Fails.\n");
	
	// Should not allow removal of the same task twice.
	printf("Test Case#6\n");
	if (ts1->removeTask (task4->getTaskId ()) == true)
		printf ("Removes same task twice. Test Case Fails.\n");

	printf("Test Case#3\n");
	if ((ts1->setTask (2, task6)) == false)
		printf ("Valid Set Task fails. Test Case Fails.\n");
	else
		printf("Test Case Success\n");

	printf("Test Case#4\n");
	if ((ts1->setTask (2, task6)) == false)
		printf ("Unable to add same task with same taskId twice. Test Case Success\n");
	
	if ((ts1->setTask (2, task7)) == true)
		printf ("Adds Tasks with a TaskId already there. Test Case Fails\n");

	if ((ts1->setTask (2, task7)) == true)
		printf ("Adds Task with TaskId == maxNoOfTasks. Test Case Fails\n");

	if ((ts1->setTask (2, task7)) == true)
		printf ("Adds Task with TaskId > maxNoOfTasks. Test Case Fails\n");

	if ((ts1->setTask (CDSU_TID_INVALID, task7)) == true)
		printf ("Adds Task with invalid TaskId. Test Case Fails\n");

	CdSuTask* nullTsk = NULL;
	if ((ts1->setTask (0, nullTsk)) == true)
		printf ("Sets Null Task. Test Case Fails\n");

	// Deletion of TaskScheduler ts1.
	delete ts1;

	// Creating Msgs to send to TaskScheduler ts dispatcher function.
	
	// NOTE: If you have to use message distributor for sending and 
	// receiving this message as the case will be most of the times,
	// you have to use CDSU_GETMSGBUF, CDSU_FREEMSGBUF macros 
	// (refer: cdsumdapi.h) and initMsg function of CdTaskMessage.
	// Don't use constructors and destructors then.
	CdTaskMessage*	taskMsg1 = new CdTaskMessage;
	taskMsg1->subMid = task1->getTaskId ();
	msgQueue->add (taskMsg1);

	CdTaskMessage* taskMsg2 = new CdTaskMessage;
	taskMsg2->subMid = task2->getTaskId ();
	msgQueue->add (taskMsg2);
	
	CdTaskMessage* taskMsg3 = new CdTaskMessage;
	taskMsg3->subMid = task3->getTaskId ();
	msgQueue->add (taskMsg3);
	
	// Successful removal of Task. Task Scheduler does not despatch
	// msgs to removed tasks.
	Uint32 taskId = task3->getTaskId ();
	if ((ts->removeTask (taskId)) == false)
		printf ("Remove Task Fail. Test Case Fails.\n");

	// Remove a Task with TaskId that does not exist.
	if (ts->removeTask (100) == true)
		printf ("Removes Task exceeding max TaskId. Test Case fails\n");

	if (ts->removeTask (CDSU_TID_INVALID) == true)
		printf ("Removes Task with Invalid taskId. Test Case fails\n");

	// Added for Valgrind check.
/*	delete msgQueue;
	delete msgQueue2;

	delete ts;
	
	delete taskMsg1;
	delete taskMsg2;
	delete taskMsg3;
	delete task1;
	delete task2;
	delete task5;
*/
	// Successful despatch of messages.
	ts->dispatcher ();

} // main ()
