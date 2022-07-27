//==============================================================================
// Application Software Group - (c) Copyright 2013, Application Software Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Fqename	    : bukAdmin.cpp
// Description	: admin functions
// Ref Docs	    :
// Author	    : Trainee 2013
// Created On   : 14.11.2013
// Last Modified:
//==============================================================================

#include "bukAdmin.h"

CdSuIntHash<int> intHash (CdSuIntHash<int>::TZ_251);
CdSuQueue <int> queueTr (20);
CdSuQueue <int> queueMr (20);
CdSuQueue<int>::iterator iter1;
CdSuQueue<int>::iterator iter2;

AdminMain* adminMain;
Admin admin;

int totalAck;

int main ()
{
	CdSuTrace::setLevel (5);

	// create and register message distributor, network handler, application modules
	basicinfraAdmin();

	// create application for admin
	createApplications ();

	while (1)
	{
		CdSuTsQueue <int> queue (5);
		int i = 0;
		queue.popBlocking (i);
	}
	while (1)
	{
		; // Intentional
	}

} // main ()

//---------------------------------------------------------------------------
// createApplications ()
// Description  : Creation of application Thread.
// Inputs       : None.
// Return Value : None.
// Side Effects : None.
// Algorithm    :
//---------------------------------------------------------------------------

void createApplications ()
{
	// Create Application Thread
	adminMain = new AdminMain;
	adminMain->spawn (AdminMain::applicationStart);
} // createApplication ()

//==============================================================================
// <End of bukAdmin.cpp>
//==============================================================================
