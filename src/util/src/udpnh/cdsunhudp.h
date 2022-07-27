#ifndef __CDSU_UDP_NETWORK_HANDLER_H
#define __CDSU_UDP_NETWORK_HANDLER_H
//=============================================================================
// Internet Telephony Software - (c) Copyright 2003 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsuudpnh.h
// Description  : This file contains class declarations of the generic
// 		  network handler class.
// Ref Docs     : 
// Author       : Vijaya Kumar 
// Created On   : Tue Mar  9 15:04:17 IST 2004
// Last Modified: 
//============================================================================

//----------------------------------------------------------------------------
// Includes.
//----------------------------------------------------------------------------
#include "cdsunhudptypes.h"
#include "cdsunhudpapiparam.h"
#include "cdsunhudpmsg.h"
#include "cdsuthread.h" 
#include "cdsunhudpdb.h"

//----------------------------------------------------------------------------
// Class	: CdSuNhUdp
// Description	: The UDP Network Handler class.
//
//		: void run ():
//			The Mi will create the object of this class and call
//			the run function. This will call the spawn function
//			defined in the base class CdSuThread. The spawn
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

class CdSuNhUdp : public CdSuThread
{
public:
	//--------------------------------------------------------------------
	//		Public Methods
	//--------------------------------------------------------------------
	
	// Constructor
	CdSuNhUdp (CdSuNhUdpDb*);
	// Destructor
	~CdSuNhUdp ();
	
	static void* threadEntry (void*); 
	void run ();			 	
	void start ();
	inline bool isDeleted () { return deleted;};

private:
	// Socket FD
	typedef int CdSuNhUdpHandle;
	
	enum 
	{
		CDSU_NHUDP_INV_UDPHANDLE = -1
	};
	
	//--------------------------------------------------------------------
	// struct	: CdSuNhUdpPortInfo
	// Description	: It keeps the information of UDP Port.
	//--------------------------------------------------------------------
	
	struct CdSuNhUdpPortInfo
	{
		CdSuNhUdpPortInfo ();		// Constructor
	
		CdSuNhUdpPortId udpPortId;	// Unique Id Of Udp Port.
		CdSuNhUdpHandle handle;		// Socket Fd.

		CdSuIpv4Addr destIpv4Addr;	// Destination IP Information.
		CdSuIpv4UdpPortNo destPortNo;	// Destination Port NO.	
		bool isDTAddrFixed;		// Flag : Is Destination Transport Address fixed?
		
		CdSuIpv4Addr selfIpv4Addr;	// Self IP Information.
		CdSuIpv4UdpPortNo selfPortNo;	// Self Port NO.
		bool  isLTAddrFixed;		// Flag : Is Local Transport Address Fixed.
		bool isEphemeral;		// Flag : Is Local Port Is ephermal.

		CdSuNhUdpEventHlr applEventHlr; // Event Handler to be notified for events on
	       					// this udp port.
		CdSuActorId applActorId;	// Application Actor Id who opened udp port.		

	}; // struct CdSuNhUdpPortInfo


	//--------------------------------------------------------------------
	// 			Private Methods.
	//--------------------------------------------------------------------
	
	// To receive data indication on existing udp port.
	bool reactor ();

	// Handling Received Message.
	void handleMsg (CdMessage*);
		
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
	void stCreatedCtrlReqMsgHlr (CdMessage* msg);
	void stRunningCtrlReqMsgHlr (CdMessage* msg);
	void statusReqMsgHlr (CdMessage* msg);
	void statsReqMsgHlr (CdMessage* msg);

	//--------------------------------------------------------------------
	// User Layer Message Handlers.
	//--------------------------------------------------------------------
	void addUdpPortReqMsgHlr (CdMessage* msg);
	void remUdpPortReqMsgHlr (CdMessage* msg);
	void dataReqMsgHlr (CdMessage* msg);
	void sendReqMsgHlr (CdMessage* msg);	
	
	//--------------------------------------------------------------------
	// Out Of Sequence Message Handler
	//--------------------------------------------------------------------
	void outOfSeqMsgHlr (CdMessage* msg);

	//--------------------------------------------------------------------
	// Functions which does actual job like
	//  *) Adding Udp Port
	//  *) Removing Udp Port
	//  *) Data Request Port
	//--------------------------------------------------------------------
	
	// Adding Udp Port.
	CdSuNhUdpRspFailCode addUdpPort (const CdSuNhUdpPortId& udpPortId,
				CdSuIpv4UdpPortNo&,
				const CdSuIpv4Addr&,
				const CdSuIpv4UdpPortNo&,
				const CdSuIpv4Addr&,
				const CdSuActorId&,
				CdSuNhUdpEventHlr);

	// To close an existing udp port.
	CdSuNhUdpRspFailCode remUdpPort (const CdSuNhUdpPortId&);
	// To send a packet.
	CdSuNhUdpRspFailCode sendDatagram (const CdSuNhUdpPortId&,
			const void*,
			const Uint16&,
			const CdSuIpv4UdpPortNo&,
			const CdSuIpv4Addr&);

	// Send Datagram.
	CdSuNhUdpRspFailCode sendDatagram (const void*,
			const Uint16&,
			const CdSuIpv4UdpPortNo&,
			const CdSuIpv4Addr&);
	
	// Returns Rsp subopcode for Subopcode Request
	CdSuNhUdpSubOpcode getCorrectSubOpcodeRsp(CdSuNhUdpSubOpcode subOpcode);
	
	//--------------------------------------------------------------------
	// Statistics update functions.
	// *) One For always opened udp port
	// *) Second One For other udp port.
	//--------------------------------------------------------------------
	void updateStatsPkgCounter (CdSuNhUdpOpcode opcode, CdSuNhUdpRspFailCode failCode);
	void updateStatsPkgCounter (CdSuNhUdpOpcode opcode, CdSuIpv4UdpPortNo localPortNo,
				CdSuIpv4UdpPortNo destPortNo, CdSuNhUdpRspFailCode failCode);
	//--------------------------------------------------------------------
	// Helper Functions
	//--------------------------------------------------------------------
	// To receive a Packet.
	int recvPacket (CdSuNhUdpPortInfo* info, sockaddr_in *&cliaddr);
	// To read exact no of bytes. 
	// *) one for connected Udp Port
	// *) non connected Udp Port
	int readNoOfBytes (CdSuNhUdpPortInfo* info, char* vptr, int noOfBytes);
	int readNoOfBytes (CdSuNhUdpPortInfo* info, char* vptr, int noOfBytes,
		 	CdSuIpv4UdpPortNo& destPortNo, CdSuIpv4Addr& destAddr);	
	// To write exact no of bytes.
	// *) one For connected Udp Port
	// *) Second one for non connected udp port
	int writeNoOfBytes (CdSuNhUdpPortInfo* info, const char* vptr, int noOfBytes);
	int writeNoOfBytes (CdSuNhUdpPortInfo* info, const char* vptr, int noOfBytes,
			CdSuIpv4UdpPortNo destPortNo, CdSuIpv4Addr destAddr);	
	
	//--------------------------------------------------------------------
	// Other Functions
	//--------------------------------------------------------------------
	// To change state of NH.
	void changeState (CdSuNhUdpState udpNhState);
	// Clean up the udp port.
	bool udpPortCleanUp (const CdSuNhUdpPortInfo* info);
	
	//--------------------------------------------------------------------
	// 			Attributes
	//--------------------------------------------------------------------
	
	CdSuNhUdpDb* nhDb;			// Pointer to DB Object
	
	CdSuNhUdpStatisticsPkg* statisticsPkg;	// Pointer to Statistics

	CdSuNhUdpState nhState;			// State of the UDP Nh
	
	CdSuNhUdpHandle maxUdpPortHandle;	// Maximum Fds.
	
	char* recvBuffer;			// Pointer to Receive Buffer
	
	bool deleted;				// flag: is Deleted.
	
	bool activityFlag;			// Activity Flag
	// Main tables
	// Udp Port Info table(Lookup By Udp POrt Id).
	CdSuIntHash <CdSuNhUdpPortInfo*>* udpPortId2UdpPortInfoTable;
	// Udp Port Info Table (lookup by udp handle(fd).)
	CdSuIntHash <CdSuNhUdpPortInfo*>* handle2UdpPortInfoTable;

}; // class CdSuNhUdp : public CdThread

//============================================================================
// <End of cdsunhudp.h>
//============================================================================
#endif
