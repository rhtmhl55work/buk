#ifndef TIMER_DEF_H_
#define TIMER_DEF_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : timerdef.h
// Description  : Timer definitions 
// Author       : Titty Thomas
// Created On   : Tue Jun 24 11:43:58 IST 2003
// Last Modified:
//============================================================================
#include <pthread.h>
#include "cdsutimer.h"

//extern CdSuTimerHandler* cdSuTimerHandler;

int cdCreateTimerHandler ();
int cdDeleteTimerHandler ();
int cdCreateTimerThread ();
int cdDeleteTimerThread ();
void* cdTimerThreadEntry (void*);

//============================================================================
// <End of timerdef.h>
//============================================================================
#endif // TIMER_DEF_H_
