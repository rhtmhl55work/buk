#ifndef __CDSU_NHUDP_TXPT_SERVICES_H__
#define __CDSU_NHUDP_TXPT_SERVICES_H__
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudptxptsrvc.h
// Description	: 
// Ref Docs     : 
// Author       : Sumit Bijwe 
// Created On   : Mon Jul 21 18:15:10 IST 2003
// Last Modified:
//============================================================================

//----------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------
#include "cdsunhudpglobals.h"

//----------------------------------------------------------------------------
// Class        : CdSuUdpTxptServices
// Description  :
//----------------------------------------------------------------------------

class  CdSuUdpTxptServices 
{
public:
	CdSuUdpTxptServices ();
	~CdSuUdpTxptServices ();

	//Create new listener
	int setListener (Uint32 ipaddr, Uint16 servPort) ;

	void stopListener();
	bool sendMsg (char* buf, Uint16 length, Uint32 ipAddr, Uint16 portNo);
	bool recvMsg (char* buf, int& length, Uint32& ipAddr, 
					Uint16& portNo);

private:
	int listenSockfd; 
	int sendSockfd;


};//class CdSuUdpTxptServices 


#endif
