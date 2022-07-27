#ifndef __BUK_BASICINFRAPARAM_H__
#define __BUK_BASICINFRAPARAM_H__

//============================================================================
//Application Software Group
//Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename	: basicinfraparam.h
// Description	:
// Ref Docs	:
// Author	: Trainee 2013
// Contributors	:
// Created On	: 11.11.2013
// Last Modified: 22.07.2022 by rohitm
//============================================================================

//ports for message distributor and network handler
#define CDSU_UDP_ADMIN_MD 50001
#define CDSU_UDP_ADMIN_NH 50003
#define CDSU_UDP_MENTOR_MD 50004
#define CDSU_UDP_MENTOR_NH 50005
#define CDSU_UDP_TRAINEE_MD 50007
#define CDSU_UDP_TRAINEE_NH 50008
#define CDSU_UDP_MYTRAINEEMENU_MD 50010 // rohitm
#define CDSU_UDP_MYTRAINEEMENU_NH 50011 // rohitm

#define REMOTE_PORT_BOTH_FIXED 0xffff

//ip address
#define IP0 192
#define IP1 168
#define IP2 16
//#define IP3 129
#define IP3 40

//for UDP nh
extern int localPortOfBothFixedAdmin;
extern int remotePortOfBothFixedAdmin;

extern int localPortOfBothFixedMentor;
extern int remotePortOfBothFixedMentor;

extern int localPortOfBothFixedTrainee;
extern int remotePortOfBothFixedTrainee;

extern int localPortOfBothFixedMyTraineeMenu; // rohitm
extern int remotePortOfBothFixedMyTraineeMenu; // rohitm

//server ip
extern char localIpAddr [4];

//client ip
extern char remoteIpAddr [4];

#endif

//============================================================================
// End of basicinfraparam.h
//============================================================================
