#ifndef __CDSU_TCP_NETWORK_HANDLER_H
#define __CDSU_TCP_NETWORK_HANDLER_H
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsutcpnh.h
// Description  : This file contains class declarations of the generic
// 		  network handler class.
// Ref Docs     : 
// Author       : Sumit Bijwe
// Created On   : Feb 4th, 2004
// Last Modified:Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

#include "cdsunhtcptypes.h"
#include "cdsunhtcpapiparam.h"
#include "cdsunhtcpmsg.h"
#include "cdsunhtcpdb.h"
#include "cdsuthread.h" 
#include "cdsuqueue.h"

//-----------------------------------------------------------------------------
// Enumerations
//-----------------------------------------------------------------------------

enum
{
	MAXSOCKADDR = 128	// max socket address structure size
};

#ifdef __linux

enum
{
	CDSU_NHTCP_MAX_FDSETSIZE = 1024	// TODO: Assign this to __FD_SETSIZE
};

#elif __sun
enum
{
	CDSU_NHTCP_MAX_FDSETSIZE = 	256
};

#elif __ECOS
enum
{
	CDSU_NHTCP_MAX_FDSETSIZE = 32	
};
#else
enum
{
	CDSU_NHTCP_MAX_FDSETSIZE = 1024	
};
#endif

//============================================================================
// struct	: CdSuNhTcpConnInfo
// Description	: It keeps the information per connection.
//============================================================================

struct CdSuNhTcpConnInfo
{
	CdSuNhTcpConnId connectionId;	// Connection Id.
	CdSuNhTcpHandle handle;		// Handle.

	// Destination IP Information.
	CdSuIpv4Addr destIpv4Addr;
	CdSuIpv4TcpPortNo destPortNo;		

	// Self IP Information.
	CdSuIpv4Addr selfIpv4Addr;
	CdSuIpv4TcpPortNo selfPortNo;	

	bool tpkt;				// TPKT support info.

	CdSuNhTcpEventHlr connectionEventHlr;	// Event Handler to be notified for events on this connection.

	CdSuActorId applActorId;		// Application Actor Id.		

	char* buffer;				// Buffer to receive data.

	//--------------------------------------------------------------------
	// Methods.
	//--------------------------------------------------------------------
	// CdSuNhTcpConnInfo ();

}; // struct CdSuNhTcpConnInfo

//============================================================================
// struct	: CdSuNhTcpLstnInfo
// Description	: It keeps the information per listener. There are 2 Event
// 		  Handler functions:
//			a) listener Event Hlr.
//		 	b) connection Event Hlr.
//		  The listener Event Hlr is called when any event is
//		  detected on any of the listeners.
//============================================================================

struct CdSuNhTcpLstnInfo
{
	CdSuNhTcpLstnId listenerId;		// Listener Id.
	CdSuNhTcpHandle handle;			// Handle.

	CdSuIpv4Addr listenerIpv4Addr;
	CdSuIpv4TcpPortNo listenPortNo;		// Listen Port No.			

	bool tpkt;				// TPKT support info.
	
	CdSuNhTcpEventHlr listenerEventHlr;	// Event Handler to be notified for events on this listener.
	CdSuNhTcpEventHlr connectionEventHlr;	// Event Handler to be passed to the ConnectionInfo when a connection is opened on the listener.

	CdSuActorId applActorId;		// Application Actor Id.		

}; // struct CdSuNhTcpLstnInfo

//----------------------------------------------------------------------------
// Class	: CdSuNhTcp
// Description	: The TCP Network Handler class.
//
//		: void run ():
//			The Mi will create the object of this class and call
//			the run function. This will call the spawn function
//			defined in the base class CdThread. The spawn
//			function in turn calls the thread entry function.
//
//		  static void* threadEntry ():
//			We will call the start function here. This function
//			hides all the intricacies of thread creation from
//			the application.
//
//		  void start ():
//			This is the main function which keeps the object
//			active. It will continuously wait for connection
//			indication and data from remote and messages from
//			the Ui, Mi Interface. 
//----------------------------------------------------------------------------

class CdSuNhTcp : public CdSuThread
{
public:
	//--------------------------------------------------------------------
	//		Public Methods
	//--------------------------------------------------------------------
	CdSuNhTcp (CdSuNhTcpDb*);
	~CdSuNhTcp ();
	
	static void* threadEntry (void*); 
	void run ();			 	
	void start ();
	inline bool isDeleted () { return delFlag; }
private:
	//--------------------------------------------------------------------
	// 			Private Methods.
	//--------------------------------------------------------------------
	void handleMsg (CdMessage*);

	// To connect to the server.
	CdSuNhTcpFailCode createConn (const CdSuNhTcpUiConReqParam conreqparam, CdSuNhTcpConnId cid);

	// To close an existing connection.
	bool closeConn (const CdSuNhTcpUiCloseConReqParam clconreqparam);

	// To add a listener.
	CdSuNhTcpFailCode addListener (CdSuIpv4Addr selfIpAddr, 
				  CdSuIpv4TcpPortNo selfPortNo, 
				  CdSuNhTcpEventHlr listenEventHlr,
				  CdSuNhTcpEventHlr connectEventHlr,
				  CdSuActorId apActorId,
				  CdSuNhTcpLstnId lstnId,
				  bool tpkt);

	// To remove a Listener.
	bool remListener (const CdSuNhTcpUiRemLstnReqParam rmlstnparam);

	// To move to the ConnectionInfo table.
	bool handleConInd (CdSuNhTcpConnId connId,
			   CdSuIpv4Addr destIpAddr,
			   CdSuIpv4TcpPortNo destPortNo, 
			   CdSuNhTcpEventHlr connectEventHlr);

	// To accept connections from the client.
	bool multiAcceptor ();

	// To receive data or close connection indication on existing connections.
	bool reactor ();
	
	// To take appropriate action when connection is closed from peer.
	bool closeConnFromPeer ();
	
	// To send a packet.
	bool sendPacket (const CdSuNhTcpUiDataReqParam dparam);

	// To receive a Packet.
	int recvPacket (CdSuNhTcpConnInfo* info, sockaddr_in *&cliaddr);

	// To read exact no of bytes.
	int readNoOfBytes (int fd, char* vptr, int noOfBytes);
	
	// To write exact no of bytes.
	int writeNoOfBytes (int fd, const char* vptr, int noOfBytes);
		
	// To change state of NH.
	void changeState (CdSuNhTcpState tcpNhState);
	
	//--------------------------------------------------------------------
	// State Handlers.
	//--------------------------------------------------------------------
	void createdStHlr (CdMessage* msg);
	void runningStHlr (CdMessage* msg);

	//--------------------------------------------------------------------
	// SM Message Handlers.
	//--------------------------------------------------------------------
	void configReqMsgHlr (CdMessage* msg);
	void delNhReqMsgHlr (CdMessage* msg);
	void ctrlReqMsgHlr (CdMessage* msg);
	void statusReqMsgHlr (CdMessage* msg);
	void statsReqMsgHlr (CdMessage* msg);
	void outOfSeqMsgHlr (CdMessage* msg);

	//--------------------------------------------------------------------
	// User Layer Message Handlers.
	//--------------------------------------------------------------------
	void addLstnMsgHlr (CdMessage* msg);
	void remLstnMsgHlr (CdMessage* msg);
	void conReqMsgHlr (CdMessage* msg);
	void txDataReqMsgHlr (CdMessage* msg);
	void discReqMsgHlr (CdMessage* msg);
		
	//--------------------------------------------------------------------
	// Utility methods
	//--------------------------------------------------------------------
	
	// Clean up the connection.
	bool connCleanUp (CdSuNhTcpConnInfo* info);

	// Clean up the listener.
	bool lstnCleanUp (CdSuNhTcpLstnInfo* info);
	
	// Shut down the tcpHandle.
	bool shutDownTcpHandle (CdSuNhTcpHandle tcpHandle);
	
	CdSuNhTcpConnInfo* buildConnInfo 
		(CdSuNhTcpHandle tcpHandle, 
		 CdSuNhTcpConnId tcpConnId,
		 struct sockaddr_in *saddr,
	         CdSuNhTcpLstnInfo* listnInfo);

	CdSuNhTcpConnInfo* buildConnInfo 
		(CdSuNhTcpHandle tcpHandle, 
		 CdSuNhTcpConnId tcpConnId,
		 CdSuIpv4Addr dstIpv4Addr,
		 CdSuIpv4TcpPortNo dstPortNo,
		 CdSuIpv4Addr selfIpv4Addr,
		 CdSuIpv4TcpPortNo selfPortNo,
		 bool tpkt,
		 CdSuNhTcpEventHlr conEventHlr,
		 CdSuActorId applActorId
	         );

	CdSuNhTcpLstnInfo* buildListenInfo
		(CdSuNhTcpHandle tcpHandle, 
	         CdSuNhTcpLstnId tcpListnId,
		 CdSuIpv4Addr ipaddr,
	         CdSuIpv4TcpPortNo lport,
	         CdSuNhTcpEventHlr levthlr,
	         CdSuNhTcpEventHlr cevthlr,
		 CdSuActorId apActorId,
	         bool tpkt);


	CdSuNhTcpUiConIndEvent* buildConnIndEvent
		(CdSuNhTcpLstnInfo* lstnInfo, CdSuNhTcpConnInfo* connInfo);

	CdSuNhTcpUiDataIndEvent* buildDataIndEvent 
		(CdSuNhTcpConnInfo* info, Uint16 dlen);

	CdSuNhTcpUiCloseConIndEvent* buildCloseConIndEvent
		(CdSuNhTcpConnInfo* info);

	void incrStatsCtrs (CdSuNhTcpCtrType ct);
	Ulong* getStatsCtrs ();

	//--------------------------------------------------------------------
	// 			Attributes
	//--------------------------------------------------------------------
	CdSuNhTcpDb* nhDb;
	CdSuNhTcpState nhState;
	CdSuNhTcpHandle maxListenHandle;
	CdSuNhTcpHandle maxConnHandle;

	// Flags.
	bool stopFlag;
	bool delFlag;
	bool activityFlag;
	// Main tables
	CdSuIntHash <CdSuNhTcpLstnInfo*>* lstnId2ListenInfoTable;
	CdSuIntHash <CdSuNhTcpLstnInfo*>* handle2ListenInfoTable;

	CdSuIntHash <CdSuNhTcpConnInfo*>* connId2ConnInfoTable;
	CdSuIntHash <CdSuNhTcpConnInfo*>* handle2ConnInfoTable;

	// Queue to hold the list of connections to be closed.
	CdSuQueue<CdSuNhTcpConnInfo*>* closeConnQueue;

}; // class CdSuNhTcp : public CdThread

//============================================================================
// <End of cdsunhtcp.h>
//============================================================================
#endif
