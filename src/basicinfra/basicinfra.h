#ifndef __BUK_BASICINFRA_H__
#define __BUK_BASICINFRA_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: basicinfra.h
// Description	:
// Ref Docs	:
// Author	: Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified: 22.07.2022 by rohitm
//============================================================================

#include <cdsuthread.h>
#include "cdsunhudpapi.h"
#include "cdsumdapi.h"
#include "cdsutrace.h"
#include "cdsuutiltypes.h"
#include "cdsuinthash.h"
#include "cdsuqueue.h"

//for using network handler

// Sys Mgmt Actor ID
extern CdSuActorId smAdminMid;
extern CdSuActorId smMentorMid;
extern CdSuActorId smTraineeMid;
extern CdSuActorId smMyTraineeMenuMid; // rohitm

extern CdSuNhUdpInstId instID;
extern CdSuNhUdpPortKey portKey;

//fail code strings
extern char failCodeString  [] [100];

//functions for regitration of moduleIds of network handlers and message distributor
void basicinfraAdmin();
void basicinfraMentor();
void basicinfraTrainee();
void basicinfraMyTraineeMenu(); // rohitm

//to get hardware unit ids
CdHardwareUnitId getHwUnitIdAdmin();
CdHardwareUnitId getHwUnitIdMentor();
CdHardwareUnitId getHwUnitIdTrainee();
CdHardwareUnitId getHwUnitIdMyTraineeMenu(); // rohitm

//to get network module ids
CdSuActorId getNhMidAdmin();
CdSuActorId getNhMidMentor();
CdSuActorId getNhMidTrainee();
CdSuActorId getNhMidMyTraineeMenu(); // rohitm

//to get message distributor module ids
CdModuleId getApplMidAdmin();
CdModuleId getApplMidMentor();
CdModuleId getApplMidTrainee();
CdModuleId getApplMidMyTraineeMenu(); // rohitm

//function for ip address change
Uint32 changeIpAddr (char ipaddr[]);

#endif

//============================================================================
// End of basicinfra.h
//============================================================================
