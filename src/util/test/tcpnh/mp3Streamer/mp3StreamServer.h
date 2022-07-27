//=============================================================================
// Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : mp3StreamServer.h
// Description  : Header File
// Author       : Vijaya Kumar
// Created On   : Fri Apr  1 16:13:01 GMT 2004
// Last Modified:
//============================================================================

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include <pthread.h>
#include <stdio.h>
#include "cdsuutiltypes.h"
#include "cdsuthread.h"
#include "cdsuinthash.h"
#include "cdsulist.h"
#include "cdsunhtcpapi.h"


//----------------------------------------------------------------------------
// Struct	: Channel Info
// Description	: Contains information streamerConnKey, Mount Point, ListnerList
//----------------------------------------------------------------------------

struct ChannelInfo
{
	ChannelInfo ()
	{
		mountPoint = NULL;
		listenerList = new CdSuList <CdSuNhTcpConnKey*>;
	}
	~ChannelInfo ()
	{
		delete listenerList;
	}	
	CdSuNhTcpConnKey streamerConnKey;		// Streamer Connection Key
	char* mountPoint;				// Mount Point
	CdSuList<CdSuNhTcpConnKey*>* listenerList;	// Listener List who are listening on this channel
	
};


//----------------------------------------------------------------------------
// class	: Mp3StreamServer
// Description	: 
//----------------------------------------------------------------------------
//
class Mp3StreamServer
{
public:
	Mp3StreamServer ();	// Constructor

	void start ();		// Starting Mp3StreamServer
private:
	// 1) Hash Table : streamer Connection Id To Channel Info
	static CdSuIntHash <ChannelInfo*>* streamerConnId2ChannelInfoTable;
	// 2) Hash Table : listener Connection Id to Streame Connection ID.
	static CdSuIntHash <CdSuNhTcpConnKey*>* listenerConnId2ConnKeyTable;			
	// Nh Actor ID
	CdSuActorId nhModId;	 
	// Application Actor ID
	static CdSuActorId applnModId;
	// Nh Instance ID
	CdSuNhTcpInstId nhInstId;
	// listen key of Stream Server 
	static CdSuNhTcpLstnKey lstnKeyOfStrServer;
	// listen Key of Mp3 Listener Server
	static CdSuNhTcpLstnKey lstnKeyOfLstServer;
	
	// Varible Initializing Function
	void initAll ();
	
	// NH Set Up Event Handler ( Management Event Handler)
	static void setUpEventHlr (void* arg);
	// Nh Tcp Listen Event Handler
	static void lstEventHlr (void* arg);
	// Nh Tcp Connection Event Handler
	static void conEventHlr (void* arg);

	// Write Data (Mp3) to All Mp3 Listner on the perticular Channel.
	static void writeStreamData (ChannelInfo*, char* data, int dataLen);
	// Extract Channel Wanted By Mp3 Listner
	static void tellChannelWanted (const CdSuNhTcpConnId&, const char* data);
	// Close Channel
	static void closeChannel (const ChannelInfo*);
	// Close Mp3 Lisnter 
	static void closeMp3ListnerConnection (const CdSuNhTcpConnId&);
	
}; // class Mp3StreamServer

//============================================================================
// <End Of File mp3StreamServer.h>
//============================================================================
