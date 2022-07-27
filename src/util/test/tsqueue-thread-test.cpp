#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "cdsutsqueue.h"

void* threadFunc1 (void*);
void* threadFunc2 (void*);

#ifdef __ECOS
char thread1StackSpace [ PTHREAD_STACK_MIN * 16 ];
char thread2StackSpace [ PTHREAD_STACK_MIN * 16 ];
#endif

int main ()
{
	CdSuTsQueue <int>* queue = new CdSuTsQueue <int> (5);
	// Create Two Threads.
	pthread_t thread1;
	pthread_t thread2;
#ifdef __ECOS
	pthread_attr_t thread1Attr;
	pthread_attr_init (&thread1Attr);
	pthread_attr_setstackaddr (&thread1Attr, 
		(void*)&thread1StackSpace [sizeof (thread1StackSpace)]);
	pthread_attr_setstacksize (&thread1Attr, sizeof (thread1StackSpace));	
	pthread_create (&thread1, 0, threadFunc1, (void*) queue);
#else
	if (pthread_create (&thread1, 0, threadFunc1, (void*) queue) == 0)
		CDSU_TRACE (1, "Thread 1 created successfully\n");
	else
		CDSU_TRACE (1, "Thread 1 creation fail\n");
#endif

#ifdef __ECOS
	pthread_attr_t thread2Attr;
	pthread_attr_init (&thread2Attr);
	pthread_attr_setstackaddr (&thread2Attr, 
		(void*)&thread2StackSpace [sizeof (thread2StackSpace)]);
	pthread_attr_setstacksize (&thread2Attr, sizeof (thread2StackSpace));	
	pthread_create (&thread2, 0, threadFunc2, (void*) queue);
#else
	
	if (pthread_create (&thread2, 0, threadFunc2, (void*) queue) == 0)
		CDSU_TRACE (1, "Thread 2 created successfully\n");
	else
		CDSU_TRACE (1, "Thread 2 creation fail\n");
#endif
	while (1)
	{
		sleep (10);
	}
}

void* threadFunc1 (void* p)
{
	CDSU_TRACE (1, "Entered Thread 1\n");

	CdSuTsQueue <int>* queue = (CdSuTsQueue <int>*)p;
	while (1)
	{
		if (queue->add (5) == true)
			printf ("Add Success\n");
		else
			printf ("Add Fail\n");
		
		struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = 250000000;
		nanosleep (&sleepTime, NULL);

	}

} // threadFunct1 ()

void* threadFunc2 (void* p)
{
	CDSU_TRACE (1, "Entered Thread 2\n");
	CdSuTsQueue <int>* queue = (CdSuTsQueue <int>*)p;
	while (1)
	{
		int x;
		if (queue->popBlocking (x) == true)
		{
			printf ("Pop success Value = %d\n", x);
		}
		else
			printf ("Pop fail\n");
	}

} // threadFunct2 ()

