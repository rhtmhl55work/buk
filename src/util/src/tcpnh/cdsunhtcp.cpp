//============================================================================= // Internet Telephony Software - (c) Copyright 2004 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhtcp.cpp
// Description	: 
// Author       : Sumit Bijwe
// Created On   : Thu Feb 12 13:26:26 IST 2004
// Last Modified: 1. Mon Aug 16 11:12:09 IST 2004
// 			By : Vijaya Kumar
// 			To Fix Bugs bs/30
// 			To Fix Bugs bs/32 . Cleanup and Stoping TCp NH
// 			Select Time Out = 0 in reactor and MultiAcceptor 
// 				And Activity Flag Mechanism is added
//		: 2 Tue Apr 18 15:58:23 IST 2006
//			By : Vijaya Kumar
// 			Following bugs entered in bugzilla are fixed
//			- 35, 88
//		: 3. Wed Dec 16 15:04:46 IST 2009
// 			By: Anish, Arun
//============================================================================

//----------------------------------------------------------------------------
// Include Files.
//----------------------------------------------------------------------------
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#include "cdsuutiltypes.h"
#include "cdsuqueue.h"
#include "cdsuthread.h"
#include "cdsutrace.h"
#include "cdsumdapi.h"
#include "cdsunhtcptypes.h"
#include "cdsunhtcpapiparam.h"
#include "cdsunhtcpdb.h"
#include "cdsunhtcpmsg.h"
#include "cdsunhtcpstatspkg.h"
#include "cdsuinthash.h"
#include "cdsunhtcp.h"
#include "cdsunhtcpfunc.h"

//----------------------------------------------------------------------------
// Public Methods Section
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: CdSuNhTcp () 
// Description  : Constructor of CdSuNhTcp Class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcp::CdSuNhTcp (CdSuNhTcpDb* nhTcpDb)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// NH DB.
	nhDb = nhTcpDb;

	// NH State.
	changeState (CDSU_NHTCP_ST_CREATED);	

	// Max no. of Listener Handle (fds).
	maxListenHandle = nhDb->getMaxNoOfLnrs ();

	// Max no. of Connection Handle (fds).
	maxConnHandle = nhDb->getMaxNoOfConn ();

	// Listeners and Connections Info Tables.
	lstnId2ListenInfoTable = new CdSuIntHash <CdSuNhTcpLstnInfo*> (CdSuIntHash<CdSuNhTcpLstnInfo*>::TZ_2039);
	handle2ListenInfoTable =  new CdSuIntHash <CdSuNhTcpLstnInfo*> (CdSuIntHash<CdSuNhTcpLstnInfo*>::TZ_2039);
	connId2ConnInfoTable = new CdSuIntHash <CdSuNhTcpConnInfo*> (CdSuIntHash<CdSuNhTcpConnInfo*>::TZ_2039);
	handle2ConnInfoTable = new CdSuIntHash <CdSuNhTcpConnInfo*> (CdSuIntHash<CdSuNhTcpConnInfo*>::TZ_2039);
	
	// Queue to hold the list of connections to be closed.
	closeConnQueue = new CdSuQueue<CdSuNhTcpConnInfo*>(CDSU_NHTCP_MAX_CONN);
	
	// Deleted Flag.
	stopFlag = true;	// To Remove Bug bs/32
	delFlag = false;

} // CdSuNhTcp::CdSuNhTcp ()

//---------------------------------------------------------------------------
// Method	: ~CdSuNhTcp () 
// Description  : Destructor of CdSuTaskSchedkuler class.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------


CdSuNhTcp::~CdSuNhTcp ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	// To Fix Bugs 32
	for (CdSuIntHash<CdSuNhTcpLstnInfo*>::iterator iter = handle2ListenInfoTable->begin (); 
		iter != handle2ListenInfoTable->end (); ++iter)
	{
		delete *iter;
	} // for 
	
	for (CdSuIntHash<CdSuNhTcpConnInfo*>::iterator iter = handle2ConnInfoTable->begin ();
			iter != handle2ConnInfoTable->end (); ++iter)
	{
	
		delete *iter;
	} // for
	
	// Delete Listeners and Connections Info Tables.
	delete lstnId2ListenInfoTable;
	delete handle2ListenInfoTable;
	delete connId2ConnInfoTable;
	delete handle2ConnInfoTable;

	// Queue to hold the list of connections to be closed.
	 
	delete closeConnQueue;

} // CdSuNhTcp::~CdSuNhTcp ()

//---------------------------------------------------------------------------
// Method	: run () 
// Description  : This will call the spawn function  defined in the base 
//		  class CdThread. The spawn  function in turn calls the 
//		  thread entry function.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::run ()
{

	if (spawn (threadEntry, this) == CdSuThread::CDSU_ERROR)
	{
		CDSU_TRACE (1, "CdSuNhTcp thread spawn error\n");
	}

} // CdSuNhTcp::run ()

//---------------------------------------------------------------------------
// Method	: threadEntry () 
// Description  : This will call the start function here. This function
//		  hides all the intricacies of thread creation froms
//		  the application.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void* CdSuNhTcp::threadEntry (void* objPtr)
{
		// Wait here for Start control request.
	((CdSuNhTcp*) objPtr)->start ();
	return (NULL);

} // CdSuNhTcp::threadEntry ()

//---------------------------------------------------------------------------
// Method	: start () 
// Description  : This is the main function which keeps the object
//	  	  active. It will continuously wait for connection
//		  indication and data from remote and messages from
//		  the Ui, Mi Interface.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    : 
//---------------------------------------------------------------------------

void CdSuNhTcp::start ()
{
	//--------------------------------------------------------------------
	// Extract the SM Event Handler.
	//--------------------------------------------------------------------
	CdSuNhTcpEventHlr eventHlr = nhDb->getSmEventHlr ();

	//------------------------------------------------------------
	// Build Control Response Event.
	//------------------------------------------------------------
	CdSuNhTcpMiInitRspEvent* rspEvent = new CdSuNhTcpMiInitRspEvent;
	rspEvent->init (nhDb->getNhInstId (), 
			nhDb->getSmActorId (),
			CDSU_NHTCP_MI_INIT_RSP, 
			CDSU_NHTCP_RSPCODE_SUCCESS,
			CDSU_NHTCP_FCODE_NOT_SET);

	//---------------------------------------------------------
	// Call Event Handler or post message.
	//---------------------------------------------------------
	if (eventHlr != NULL)
	{
		// If Event Handler has been specified.
		eventHlr ((void*) rspEvent);

	} else
	{
		cdSuNhTcpDefSmEventHlr (*rspEvent);	
		delete rspEvent;
	} // if


	//----------------------------------------------------------------------------
	// Block Broken Pipe Signal. 
	//----------------------------------------------------------------------------
	signal (SIGPIPE, SIG_IGN);

	//----------------------------------------------------------------------------
	// Thread Loop.
	//----------------------------------------------------------------------------
	while (1)
	{
		activityFlag = false;
		if (nhState == CDSU_NHTCP_ST_RUNNING)
		{
			//----------------------------------------------------------------------------
			// Step 1: Check multiacceptor for any new connections.
			//----------------------------------------------------------------------------
			multiAcceptor ();

			//----------------------------------------------------------------------------
			// Step 2: Check reactor for any data or disconnections.
			//----------------------------------------------------------------------------
			reactor ();

		}
		//----------------------------------------------------------------------------
		// Step 3: Handle Message from UI and MI.
		//----------------------------------------------------------------------------
		CdMessage* rcvMsg = NULL;
		if (cdSuMdRecvMsgNonBlock (&rcvMsg, nhDb->getNhActorId ()) != false)
		{
			activityFlag = true;
			handleMsg (rcvMsg);
		} // if
		if (activityFlag == false)
		{
			// Descheduling the thread.
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = nhDb->getNoTcpHandleTmr ();
			nanosleep (&sleepTime, NULL);

		} // if

	} // while (1)

} // CdSuNhTcp::start ()

//---------------------------------------------------------------------------
// Method	: handleMsg () 
// Description  : This will handle all the message which has come to NH from
//		  upper layer.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::handleMsg (CdMessage* msg)
{

	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	switch (nhState)
	{
	case CDSU_NHTCP_ST_CREATED:
		createdStHlr (msg);
		break;

	case CDSU_NHTCP_ST_RUNNING:
		runningStHlr (msg);
		break;

	default:
		// Do nothing.
		break;

	} // switch 

} // CdSuNhTcp::handleMsg ()

//----------------------------------------------------------------------------
//			Private Methods Section
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: createConn () 
// Description  : This method takes the appropriate parameters for opening 
//		  connection and opens the connection with the server.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpFailCode CdSuNhTcp::createConn (const CdSuNhTcpUiConReqParam conreqparam, const CdSuNhTcpConnId connId)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuIpv4Addr dstIpAddr = conreqparam.dstIpv4Addr; 
	CdSuIpv4TcpPortNo dstPortNo = conreqparam.dstPortNo;

	CdSuNhTcpHandle tcpHandle = CDSU_NHTCP_INV_TCPHANDLE;

	//----------------------------------------------------------------------------
	// Phase 1: Prepare the socket and bind. (Deal with socket operations)
	//----------------------------------------------------------------------------
	// Create connection handle.

	if ((tcpHandle = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		CDSU_TRACE (1, "CdSuNhTcp::createConn (): socket creation error\n");
		if (errno == ENFILE)
		{
			CDSU_TRACE (1, "CdSuNhTcp::createConn ():Not enough kernel memory to allocate new socket \n");
			return (CDSU_NHTCP_FCODE_SOCK_NO_KERMEM_ERR);
		}
		if (errno == EMFILE)
		{
			CDSU_TRACE (1, "CdSuNhTcp::createConn ():Process file table overflow. \n");
			return (CDSU_NHTCP_FCODE_SOCK_NO_PFTOF_ERR);
		}
		if (errno == ENOMEM )
		{
			CDSU_TRACE (1, "CdSuNhTcp::createConn ():Insufficient memory is available. \n");
			return (CDSU_NHTCP_FCODE_SOCK_INSUF_MEM_ERR);
		}
	}

	// Create server address structure.
	sockaddr_in *servaddr = new sockaddr_in;
	memset (servaddr, 0, sizeof (*servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons (dstPortNo);
	servaddr->sin_addr.s_addr = dstIpAddr;

	if ((connect (tcpHandle, (sockaddr *) servaddr, sizeof (*servaddr))) == -1)
	{
		CDSU_TRACE (1, "CdSuNhTcp::createConn ():connect error\n");
		delete servaddr;
		return (CDSU_NHTCP_FCODE_CONNECTERROR);

	} else
	{
		CDSU_TRACE (3, "CdSuNhTcp::createConn ():connect success\n");
		// Get local ip address and port
		union
		{
			struct sockaddr sa;
			unsigned char data[MAXSOCKADDR];
		} un;

#ifdef __sun
		int len;
#else
		socklen_t len;
#endif

		len = MAXSOCKADDR;
		CdSuIpv4Addr selfIpAddr;
		CdSuIpv4TcpPortNo selfPortNo;
		if (getsockname (tcpHandle, (struct sockaddr*) un.data, &len) < 0)
		{
			CDSU_TRACE (1, "CdSuNhTcp::createConn ():getsockname error\n");
		}
		else
		{	// Set local ip address and port no
			struct sockaddr_in *localaddr;
			localaddr = (struct sockaddr_in *) &un.sa;
			selfIpAddr = localaddr->sin_addr.s_addr; 
			selfPortNo = localaddr->sin_port;
		}

		//--------------------------------------------------------------------
		// Phase 2: Connection ready. Update related data structures.
		//	- Connection Tables.
		//--------------------------------------------------------------------

		// Get Connection Id.
		CdSuNhTcpConnId tcpConnId = connId;

		//TODO: Check if tcpConnId is valid. If not, close socket.

		// Build listener info.
		CdSuNhTcpConnInfo* connInfo = NULL;  
		connInfo = buildConnInfo (tcpHandle, tcpConnId, 
					  dstIpAddr, dstPortNo, 
					  selfIpAddr, selfPortNo, 
					  conreqparam.tpkt,
					  conreqparam.conEventHlr,
					  conreqparam.applActorId);

		// Insert connection info into handle2ConnInfoTable.
		if (handle2ConnInfoTable->insert (tcpHandle, connInfo) != true)
		{
			// TODO: Cleanup!
			delete servaddr;
			return (CDSU_NHTCP_FCODE_UNKNOWNERROR);
		}
		
		// Insert connection info into connId2ListenInfoTable.
		if (connId2ConnInfoTable->insert (tcpConnId, connInfo) != true)
		{
			// TODO: Cleanup!
			handle2ConnInfoTable->remove (tcpHandle);
			delete servaddr;
			return (CDSU_NHTCP_FCODE_UNKNOWNERROR);
		}

		//--------------------------------------------------------------------
		// Phase 3: Operation Success. Call the appropriate event handler.
		//
		//--------------------------------------------------------------------
		// Form Conn Key.
		CdSuNhTcpConnKey connKey;
		connKey.init (tcpConnId, nhDb->getNhInstId());

		// Send successful response.
		CdSuNhTcpUiConRspEvent* evt = new CdSuNhTcpUiConRspEvent ();
		evt->init (connKey, 
			   conreqparam.applActorId,
			   CDSU_NHTCP_UI_CONNECT_RSP, 
			   CDSU_NHTCP_RSPCODE_SUCCESS,
			   CDSU_NHTCP_FCODE_NOT_SET);
		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = conreqparam.conEventHlr; 
		if (eventHlr != NULL)
		{
			// Notify event to specified
			// connection event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default
			// connection event handler.
			cdSuNhTcpDefConEventHlr (*evt);	
			delete evt;
		} // if
		
		delete servaddr;
		return (CDSU_NHTCP_FCODE_NOT_SET);
	}

	delete servaddr;
	return (CDSU_NHTCP_FCODE_UNKNOWNERROR);

} // CdSuNhTcp::createConn ()

//---------------------------------------------------------------------------
// Method	: closeConn () 
// Description  : This method closes the an existing connection. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::closeConn (const CdSuNhTcpUiCloseConReqParam clconreqparam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhTcpConnInfo* connInfo = NULL; 
	CdSuNhTcpRspCode rspc = CDSU_NHTCP_RSPCODE_FAIL;
	CdSuNhTcpFailCode failc = CDSU_NHTCP_FCODE_NOT_SET;
	
	// Lookup in connInfo table.
	if (connId2ConnInfoTable->lookup (clconreqparam.connKey.connId, connInfo) != true)
	{
		// No Connection.
		CDSU_TRACE (1, "CdSuNhTcp::closeConn (): Connection does not exist.\n");
		// Set -ve response code.
		rspc = CDSU_NHTCP_RSPCODE_FAIL;
		failc = CDSU_NHTCP_FCODE_NO_CONN;

	} else
	{	
		// Clear connection and related data structures.
		if (connCleanUp (connInfo) != true)
		{
			CDSU_TRACE (1, "CdSuNhTcp::closeConn (): Connection cleanup failed.\n");
			// Set -ve response code.
			rspc = CDSU_NHTCP_RSPCODE_FAIL;
			failc = CDSU_NHTCP_FCODE_UNKNOWNERROR;
		} else
		{
			CDSU_TRACE (3, "CdSuNhTcp::closeConn (): Connection cleanup successful.\n");
			// Set +ve response code.
			rspc = CDSU_NHTCP_RSPCODE_SUCCESS;
		}	

	} // if (lookup...)
	
	// Form Conn Key.
	CdSuNhTcpConnKey connKey;
	connKey.init (clconreqparam.connKey.connId, nhDb->getNhInstId ());
	// Build close connection event
	CdSuNhTcpUiCloseConRspEvent* evt = new
			CdSuNhTcpUiCloseConRspEvent ();
	evt->init (connKey,
		   clconreqparam.applActorId,
		   CDSU_NHTCP_UI_CLOSE_CON_RSP,
		   rspc,
		   failc);

	// Extract Event Handler.
	CdSuNhTcpEventHlr eventHlr = clconreqparam.conEventHlr; 
	if (eventHlr != NULL)
	{
		// Notify event to specified
		// connection event handler.
		eventHlr((void*) evt);
	} else
	{
		// Notify event to default
		// connection event handler.
		cdSuNhTcpDefConEventHlr (*evt);	
		delete evt;
	} // if
	
	// Return.
	if (rspc != CDSU_NHTCP_RSPCODE_SUCCESS)
	{
		return (false);
	} else
	{
		return (true);

	} // if (rspc...)

	return (false);

} // CdSuNhTcp::closeConn ()

//---------------------------------------------------------------------------
// Method	: addListener () 
// Description  : This method adds a Listener.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpFailCode CdSuNhTcp::addListener (CdSuIpv4Addr selfIpAddr, 
		             CdSuIpv4TcpPortNo selfPortNo, 
		   	     CdSuNhTcpEventHlr listenEventHlr,
			     CdSuNhTcpEventHlr connectEventHlr,
			     CdSuActorId apActorId,
			     CdSuNhTcpLstnId lstnId,
			     bool tpkt)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpHandle tcpHandle = CDSU_NHTCP_INV_TCPHANDLE;

	//--------------------------------------------------------------------
	// Phase 1: Prepare the socket and bind. (Deal with socket operations)
	//--------------------------------------------------------------------

	// Create listen handle 
	if ((tcpHandle = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		CDSU_TRACE (1, "CdSuNhTcp::addListener(): Socket creation error.\n");	
		if (errno == ENFILE)
		{
			CDSU_TRACE (1, "CdSuNhTcp::addListener ():Not enough kernel memory to allocate new socket \n");
			return (CDSU_NHTCP_FCODE_SOCK_NO_KERMEM_ERR);
		}
		if (errno == EMFILE)
		{
			CDSU_TRACE (1, "CdSuNhTcp::addListener ():Process file table overflow. \n");
			return (CDSU_NHTCP_FCODE_SOCK_NO_PFTOF_ERR);
		}
		if (errno == ENOMEM )
		{
			CDSU_TRACE (1, "CdSuNhTcp::addListener ():Insufficient memory is available. \n");
			return (CDSU_NHTCP_FCODE_SOCK_INSUF_MEM_ERR);
		}
	}

	// Create server address structure
	struct sockaddr_in servaddr;
	memset (&servaddr,0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = selfIpAddr;
	servaddr.sin_port = htons (selfPortNo);

	CDSU_TRACE3 (5, "CdSuNhTcp::addListener(): Binding socket %d to %s:%d\n", 
			tcpHandle, inet_ntoa (servaddr.sin_addr), selfPortNo);
	
	// Bind listen handle to serv address
	if ((bind (tcpHandle, (sockaddr *) & servaddr, 
		sizeof (servaddr))) == -1)
	{
		CDSU_TRACE (1, "socket bind error\n");	
		return (CDSU_NHTCP_FCODE_BINDERROR);
	}

	// Put the listen socket in listen mode
	if ((listen (tcpHandle, nhDb->getLnqSize())) == -1)
	{
		CDSU_TRACE (1,"CdSuNhTcp::addListener(): Socket listen error.\n");
		return (CDSU_NHTCP_FCODE_SOCK_LSTN_ERR);
	}

	CDSU_TRACE1 (4, "Self ip addr: %d\n", servaddr.sin_addr.s_addr);
	CDSU_TRACE1 (4, "Self port number : %d\n", servaddr.sin_port);

	//--------------------------------------------------------------------
	// Phase 2: Socket ready. Update related data structures.
	//	- Listner Tables.
	//--------------------------------------------------------------------
	// Get listener Id.
	CdSuNhTcpLstnId tcpLstnId = lstnId; 

	//TODO: Check if tcpLstnId is valid. If not, close socket.

	// Build listener info.
	CdSuNhTcpLstnInfo* lstnInfo = NULL;  
	lstnInfo = buildListenInfo (tcpHandle, tcpLstnId, selfIpAddr, 
				    selfPortNo, listenEventHlr, 
				    connectEventHlr,
				    apActorId,
				    tpkt);

	// Insert listener info into handle2ListenInfoTable.
	if (handle2ListenInfoTable->insert (tcpHandle, lstnInfo) != true)
	{
		// TODO: Cleanup!
		return (CDSU_NHTCP_FCODE_UNKNOWNERROR);
	}
	
	// Insert listener info into lstnId2ListenInfoTable.
	if (lstnId2ListenInfoTable->insert (tcpLstnId, lstnInfo) != true)
	{
		// TODO: Cleanup!
		handle2ListenInfoTable->remove (tcpHandle);
		return (CDSU_NHTCP_FCODE_UNKNOWNERROR);
	}

	//--------------------------------------------------------------------
	// Phase 3: Operation Success. Call the appropriate event handler.
	//
	//--------------------------------------------------------------------
	// Form Lstn Key.
	CdSuNhTcpLstnKey lstnKey;
	lstnKey.init (tcpLstnId , nhDb->getNhInstId ());
	CdSuNhTcpUiAddLstnRspEvent* evt = new CdSuNhTcpUiAddLstnRspEvent ();
	evt->init (lstnKey, 
		   lstnInfo->applActorId,
		   CDSU_NHTCP_UI_ADD_LISTENER_RSP, 
		   CDSU_NHTCP_RSPCODE_SUCCESS,
		   CDSU_NHTCP_FCODE_NOT_SET);
	// Extract Event Handler.
	CdSuNhTcpEventHlr eventHlr = listenEventHlr; 
	if (eventHlr != NULL)
	{
		// Notify event to specified listener event handler.
		eventHlr((void*) evt);
	} else
	{

		// Notify event to default listener event handler.
		cdSuNhTcpDefLstnEventHlr (*evt);	
		delete evt;
	} // if
	
	return (CDSU_NHTCP_FCODE_NOT_SET);

} // CdSuNhTcp::addListener ()

//---------------------------------------------------------------------------
// Method	: remListener () 
// Description  : This method removes the an existing listener. 
// Inputs       : 
// Return value : 
// Side effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::remListener (const CdSuNhTcpUiRemLstnReqParam rmlstnparam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhTcpLstnInfo* lstnInfo = NULL; 
	CdSuNhTcpRspCode rspc;
	CdSuNhTcpFailCode failc = CDSU_NHTCP_FCODE_NOT_SET;
	
	// lookup in lstninfo table.
	if (lstnId2ListenInfoTable->lookup (rmlstnparam.lstnKey.lstnId, lstnInfo) != true)
	{
		// no listener.
		CDSU_TRACE (1, "cdsunhtcp::remListener (): listener does not exist.\n");
		// set -ve response code.
		rspc = CDSU_NHTCP_RSPCODE_FAIL;
		failc = CDSU_NHTCP_FCODE_NO_LSTN;

	} else
	{	
		// clear listener and related data structures.
		if (lstnCleanUp (lstnInfo) != true)
		{
			CDSU_TRACE (1, "cdsunhtcp::remListener (): listener cleanup failed.\n");
			// set -ve response code.
			rspc = CDSU_NHTCP_RSPCODE_FAIL;
			failc = CDSU_NHTCP_FCODE_UNKNOWNERROR;

		} else
		{
			CDSU_TRACE (3, "cdsunhtcp::remListener (): listener cleanup successful.\n");
			// set +ve response code.
			rspc = CDSU_NHTCP_RSPCODE_SUCCESS;
		}	

	} // if (lookup...)
	
	// form lstn key.
	CdSuNhTcpLstnKey lstnkey;
	lstnkey.init (rmlstnparam.lstnKey.lstnId, nhDb->getNhInstId ());
	// build remove lstn event.
	CdSuNhTcpUiRemLstnRspEvent* evt = new
			CdSuNhTcpUiRemLstnRspEvent ();
	evt->init (lstnkey,
		   rmlstnparam.applActorId,
		   CDSU_NHTCP_UI_REM_LISTENER_RSP,
		   rspc,
		   failc);

	// Extract Event Handler.
	CdSuNhTcpEventHlr eventHlr = rmlstnparam.listenEventHlr; 
	if (eventHlr != NULL)
	{
		// Notify event to specified listener event handler.
		eventHlr((void*) evt);
	} else
	{

		// Notify event to default listener event handler.
		cdSuNhTcpDefLstnEventHlr (*evt);	
		delete evt;
	} // if
	

	// return.
	if (rspc != CDSU_NHTCP_RSPCODE_SUCCESS)
	{
		return (false);
	} else
	{
		return (true);

	} // if (rspc...)

	return (false);

} // CdSuNhTcp::remListener ()


//---------------------------------------------------------------------------
// method	: handleconind () 
// description  : this method updates connectioninfo table on new connection 
//		  indication. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::handleConInd (CdSuNhTcpConnId connId,
			      CdSuIpv4Addr destIpAddr,
			      CdSuIpv4TcpPortNo destPortNo, 
			      CdSuNhTcpEventHlr connectEventHlr)
{

	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	return (false);

} // CdSuNhTcp::handleConInd ()

//---------------------------------------------------------------------------
// Method	: multiAcceptor () 
// Description  : This function listens for connections from multiple listener
//		  handle (sockets). These listener handle are earlier set 
//		  to listen mode by setListener. This uses select call for 
//		  a set of listen handles.  
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//	* if (No listeners)
//		sleep for sometime and return.
//	* Check for activity on sockets.
//---------------------------------------------------------------------------

bool CdSuNhTcp::multiAcceptor ()
{

	int numLstnrs = handle2ListenInfoTable->size ();
	// If there are no listeners, sleep for sometime and return.
	if (numLstnrs <= 0) 			
	{
		// No connections were found. Sleep...
		/* struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = nhDb->getNoTcpHandleTmr (); 
		nanosleep (&sleepTime, NULL);
		*/

		return (false);
	} // if

	//--------------------------------------------------------------------
	// There is at least one listener. (maxListenHandle > 0)
	// 
	// * Set the FD list.
	// * Do a "select" on the active file descriptors.
	// * Check for activity
	//	- select <= 0		: No activity.
	//	- select = 0		: Some activity.
	//--------------------------------------------------------------------
	int maxDescValue = 0;	// The maximum numbered descriptor in the
				// fdlist going into select.

	fd_set lfds;			// listen tcpHandle.
	FD_ZERO (&lfds); 		// clear tcpHandle.

	for (CdSuIntHash<CdSuNhTcpLstnInfo*>::iterator i = handle2ListenInfoTable->begin ();
		i != handle2ListenInfoTable->end (); ++i)
	{
		CdSuNhTcpLstnInfo* info = *i;

		if (info != NULL)
		{
			FD_SET (info->handle, &lfds);
			if (info->handle > maxDescValue)
			{
				maxDescValue = info->handle;
			}
		}
	}

	// Setting of select time.
	struct timeval tv;
	tv.tv_sec = 0; 
	tv.tv_usec = 0; // nhDb->getLstnSelTmr();	// Time to call waiting for connections. (microsecs)

	int retval = select ((maxDescValue + 1), &lfds, NULL, NULL, &tv);

	//----------------------------------------------------------------------------
	// Check # 1 : retVal == -1
	// Action : look for error and return.
	//----------------------------------------------------------------------------
	if (retval == -1)
	{
		if (errno == EINTR)
		{
			CDSU_TRACE (1, "CdSuNhTcp::multiAcceptor(): Caught Non Blocked Signal.\n");
		} else 
		{
			CDSU_TRACE (1, "CdSuNhTcp::multiAcceptor(): Select error.\n");
			return (false);
		}
	}

	//----------------------------------------------------------------------------
	// Check # 2 : retVal <= 0
	// Action : there was no activity return.
	//----------------------------------------------------------------------------
	if (retval <= 0)
	{
		return (false);
	}

	//----------------------------------------------------------------------------
	// Check # 3 : retVal > 0
	// Action : Some activity, handle the inputs on this activity.
	//----------------------------------------------------------------------------
	for (CdSuIntHash<CdSuNhTcpLstnInfo*>::iterator i = handle2ListenInfoTable->begin ();
		i != handle2ListenInfoTable->end (); ++i)
	{
		CdSuNhTcpLstnInfo* info = *i;

		if (info != NULL)
		{
			if (FD_ISSET (info->handle, &lfds)) 
			{
				activityFlag = true;
				struct sockaddr_in* cliaddr = new  sockaddr_in; // Client address.

#ifdef __sun
				int clilen;
#else
				socklen_t clilen;
#endif
				clilen = sizeof (sockaddr_in);

				// Accept Connection.
				int tcpHandle = accept (info->handle, (sockaddr*) cliaddr, &clilen);	
				
				if (tcpHandle == -1) 
				{
					if (errno == ENFILE)
					{
						CDSU_TRACE (1, "CdSuNhTcp::addListener ():Not enough kernel memory to allocate new socket \n");
					}
					if (errno == EMFILE)
					{
						CDSU_TRACE (1, "CdSuNhTcp::addListener ():Process file table overflow. \n");
					}
					if (errno == ENOMEM )
					{
						CDSU_TRACE (1, "CdSuNhTcp::addListener ():Insufficient memory is available. \n");
					}


					CDSU_TRACE (1, "accept error\n");
					// Delete client addr.
					delete cliaddr;

					return (false);
				} else
				{
					// Allocate tcpConnId.
					CdSuNhTcpConnId tcpConnId = nhDb->allocConnId ();

					// TODO:Check for invalid tcpConnId.

					// Build ConnInfo.
					CdSuNhTcpConnInfo* connInfo = NULL; 
					connInfo = buildConnInfo (tcpHandle, tcpConnId, cliaddr, info);
					
					if (connInfo == NULL)
					{
						// Delete client addr.
						delete cliaddr;
						return (false); 
					}

					// Update the handle2ConnInfoTable ().	
					if ( handle2ConnInfoTable->insert (tcpHandle, connInfo) != true) 
					{
						// Delete client addr.
						delete cliaddr;
						return (false);
					}

					// Update the connId2ConnInfoTable ().	
					if (connId2ConnInfoTable->insert (tcpConnId, connInfo) != true)
					{
						handle2ListenInfoTable->remove (tcpHandle);
						// Delete client addr.
						delete cliaddr;
						return (false);
					}

					// Build Conn Ind Event.
					CdSuNhTcpUiConIndEvent* evt = NULL; 
					evt = buildConnIndEvent (info, connInfo); 

					if (evt == NULL)
					{
						// Revert back.
						handle2ConnInfoTable->remove (tcpHandle);
						connId2ConnInfoTable->remove (tcpConnId);
						// Delete client addr.
						delete cliaddr;
						return (false);
					}
					// Extract Event Handler.
					CdSuNhTcpEventHlr eventHlr = info->listenerEventHlr; 
					if (eventHlr != NULL)
					{
						// Notify event to specified listener event handler.
						eventHlr((void*) evt);
						// Update statistics counter.
						incrStatsCtrs (CDSU_NHTCP_CONIND_CTR);

					} else
					{
						// Notify event to default listener event handler.
						cdSuNhTcpDefLstnEventHlr (*evt);	
						delete evt;
					} // if

					// Delete client addr.
					delete cliaddr;

					return (true);

				} // if (tcpHandle == -1)

			// Delete client addr.
			delete cliaddr;

			} // if (FD_ISSET...)

		} else
		{
			CDSU_TRACE (1, "CdSuNhTcp::multiAcceptor(): Error! Got null entry in handle2ListenInfoTable.\n");
			return (false);

		} // if (info != NULL)

	} // for
	return (false);

} // CdSuNhTcp::multiAcceptor ()

//---------------------------------------------------------------------------
// Method	: reactor () 
// Description  :
//		* Takes as input maximum message size and application event hdlr.  
//		* Sets select time. 
//		* Creates buffer to write received data. 
//		* In a loop, if atleast one connection is existing, 
//		  clear all tcpHandle (sockfds), and set it to current connectionList. 
//		* Calls select. If there is some input to read, check for the
//		  active tcpHandle and read from them. 
//		* If some tcpHandle is active and no input to read indicate "end of file" 
//		  equivalent to disconnection from peer. 
//		* Calls the appropriate application event handler.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::reactor ()
{
	int numConnections = handle2ConnInfoTable->size ();
	// If there are no connections, sleep for sometime and return.
	if (numConnections <= 0)
	{
		// No connections were found. Sleep...
		/*struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = nhDb->getNoTcpHandleTmr (); 
		nanosleep (&sleepTime, NULL);
		*/ 
		return (false);

	} // if (maxConnHandle <= 0)

	//--------------------------------------------------------------------
	// Loop through the connected sockets and check for activity.
	//--------------------------------------------------------------------
	CdSuIntHash<CdSuNhTcpConnInfo*>::iterator h2ciTableIterSelect = handle2ConnInfoTable->begin ();
	CdSuIntHash<CdSuNhTcpConnInfo*>::iterator h2ciTableIterActivity = handle2ConnInfoTable->begin ();
	
	int numOfLoops = (numConnections + CDSU_NHTCP_MAX_FDSETSIZE - 1) / CDSU_NHTCP_MAX_FDSETSIZE;
	
	for (int i = 0; i < numOfLoops; i++)
	{
		//--------------------------------------------------------------------
		// There is at least one connection. (maxConnHandle > 0)
		// 
		// * Set the FD list.
		// * Do a "select" on the active file descriptors.
		// * Check for activity
		//	- select <= 0		: No activity.
		//	- select = 0		: Some activity.
		//--------------------------------------------------------------------
		int maxDescValue = 0;	// The maximum numbered descriptor in the
					// fdlist going into select.

		fd_set rfds;		// Read tcpHandle. 
		FD_ZERO (&rfds);	// Clear tcpHandle.

		int sockFdCounter = 0;
		for (int j = 0; 
		     ((j < CDSU_NHTCP_MAX_FDSETSIZE) && 
			(h2ciTableIterSelect != handle2ConnInfoTable->end ())); 
		     ++j, ++h2ciTableIterSelect)
		{
			CdSuNhTcpConnInfo* info = *h2ciTableIterSelect;

			if (info != NULL)
			{
				FD_SET (info->handle, &rfds);
				if (info->handle > maxDescValue)
				{
					maxDescValue = info->handle;
				}
			}
			sockFdCounter++;
		} // for

		//--------------------------------------------------------------------
		// Select call.
		//--------------------------------------------------------------------
		// Time value for select.
		struct timeval tv;
		tv.tv_sec = 0; 
		tv.tv_usec = 0; // nhDb->getDataSelTmr();	// Time to call waiting for data
							// on connections. (microsecs)

		int retval = select ((maxDescValue + 1), &rfds, NULL, NULL, &tv);

		//----------------------------------------------------------------------------
		// Check # 1 : retVal == -1
		// Action : look for error and return.
		//----------------------------------------------------------------------------
		if (retval == -1)
		{
			perror ("CdSuNhTcp::reactor(): Select Error!");
			continue;
		}

		//----------------------------------------------------------------------------
		// Check # 2 : retVal <= 0
		// Action : there was no activity, continue with the other sockets.
		//----------------------------------------------------------------------------
		if (retval <= 0)
		{
			continue;	
		}

		//----------------------------------------------------------------------------
		// Check # 3 : retVal > 0
		// Action : Some activity, handle the inputs on this activity.
		//----------------------------------------------------------------------------
		for (int j = 0; 
		     ((j < CDSU_NHTCP_MAX_FDSETSIZE) &&
		      (h2ciTableIterActivity != handle2ConnInfoTable->end ())); 
		     ++j, ++h2ciTableIterActivity)
		{
			CdSuNhTcpConnInfo* info = *h2ciTableIterActivity;

			if (info == NULL)
			{
				CDSU_TRACE (1, "h2ciTable has a null entry!\n");
				continue;
			}

			if (FD_ISSET (info->handle, &rfds))
			{
				activityFlag = true;
				// Client address.
				struct sockaddr_in* sockaddr = new sockaddr_in; 

				//------------------------------------------------------------
				// recvPacket ():
				//
				// retVal < 0 : error. Send discInd.
				// retVal > 0 : data.  Send dataInd.
				// retVal = 0 : disc.  Send discInd.
				//------------------------------------------------------------

				int bytesRxvd = recvPacket (info, sockaddr);
				
				if (bytesRxvd < 0)
				{
					//----------------------------------------------
					// bytesRxvd < 0  : Error.
					//----------------------------------------------
					if (errno == EINTR)
					{
						delete sockaddr;
						continue; // A non blocked signal was caught. 
					} else
					{
						CDSU_TRACE (1, "socket read error\n");

						// Build close connection indication event.
						CdSuNhTcpUiCloseConIndEvent* evt = NULL;
						evt = buildCloseConIndEvent (info);

						// Extract Event Handler.
						CdSuNhTcpEventHlr eventHlr = info->connectionEventHlr; 
						if (eventHlr != NULL)
						{
							// Notify event to
							// specified connection event handler.
							eventHlr((void*) evt);
						} else
						{
							// Notify event to
							// default connection event handler.
							cdSuNhTcpDefConEventHlr (*evt);	
							delete evt;
						} // if
	
						// Clean up the connection.
						closeConnQueue->add (info);

						delete sockaddr;

						continue;
					} // if (errno...)

				} else if (bytesRxvd > 0)
				{
					//----------------------------------------------
					// bytesRxvd > 0  : Data received.
					//----------------------------------------------
					// Build Data Ind Event.
					CdSuNhTcpUiDataIndEvent* evt = NULL;
					evt = buildDataIndEvent (info, bytesRxvd);
					
					// Notify connection event handler for
					// data arrival on a connection.  
					if (evt != NULL)
					{
						
						// Extract Event Handler.
						CdSuNhTcpEventHlr eventHlr = info->connectionEventHlr; 
						if (eventHlr != NULL)
						{
							// Notify event to
							// specified connection event handler.
							eventHlr((void*) evt);
							// Update statistics counter.
							incrStatsCtrs (CDSU_NHTCP_DATAIND_CTR);

						} else
						{
							// Notify event to
							// default Connection event handler.
							cdSuNhTcpDefConEventHlr (*evt);	
							delete evt;
						} // if
						
						// Delete addr.
						delete sockaddr;

						continue;
						// return (true);
					} else
					{
						// Delete addr.
						delete sockaddr;

						continue;
						//return (false);
					}
				} else 
				{
					//----------------------------------------------
					// bytesRxvd == 0  : Disconnection by peer. 
					//----------------------------------------------
					CDSU_TRACE (1, "disconnection by peer\n");

					// Build close connection indication event.
					CdSuNhTcpUiCloseConIndEvent* evt = NULL;
					evt = buildCloseConIndEvent (info);
					
					// Extract Event Handler.
					CdSuNhTcpEventHlr eventHlr = info->connectionEventHlr; 
					if (eventHlr != NULL)
					{
						// Notify event to specified
						// connection event handler.
						eventHlr((void*) evt);
						// Update statistics counter.
						incrStatsCtrs (CDSU_NHTCP_CLOSECON_IND_CTR);

					} else
					{
						// Notify event to default
						// connection event handler.
						cdSuNhTcpDefConEventHlr (*evt);	
						delete evt;
					} // if
					
					// Clean up the connection.
					closeConnQueue->add (info);

					// Delete addr.
					delete sockaddr;

					continue;	

				} // if (...)
			
			} // if (FD_ISSET...) 

		} // for

	} // for (numLoops)

	//--------------------------------------------------------------------
	// Close the connection that were listed above.
	//--------------------------------------------------------------------
	while (closeConnQueue->empty () != true)
	{
		CdSuNhTcpConnInfo* info;
		bool retVal = closeConnQueue->pop (info);	
		if (retVal == true)
		{
			connCleanUp (info);
		}
	}

	//--------------------------------------------------------------------
	// Finished with all the jobs!
	//--------------------------------------------------------------------
	return (true);

} // CdSuNhTcp::reactor ()

//---------------------------------------------------------------------------
// Method	: recvPacket () 
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

int CdSuNhTcp::recvPacket (CdSuNhTcpConnInfo* info, 
			    sockaddr_in *&cliaddr)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);

	int len;
	
	//----------------------------------------------------------------------------
	// Check tpkt;
	//----------------------------------------------------------------------------
	if (info->tpkt != false)
	{
		unsigned char* tpkt;
		tpkt = new unsigned char [4];

		// Read tpkt header
		int noOfBytesRead = readNoOfBytes (info->handle, (char*) tpkt, 4);

		if (noOfBytesRead == 0)
		{
			// Socket disconnect by peer. 
			CDSU_TRACE ( 1, "disconnect by peer\n");
			// Delete tpkt
			delete [] tpkt;

			return (0);

		} else if (noOfBytesRead == -1)
		{
			// Error. 
			CDSU_TRACE ( 1, "socket read error\n");
			// Delete tpkt
			delete [] tpkt;

			return (-1);

		} else if (noOfBytesRead != 4)
		{
			CDSU_TRACE1 (1, "error in reading TPKT hdr = %d\n", noOfBytesRead);		
			// Delete tpkt
			delete [] tpkt;

			return (-1);
		} // if


		// check tpkt version
		if (tpkt[0] != 3) // Only support version 3
		{
 
			CDSU_TRACE1 ( 1, "unsupported TPKT version - %d\n",tpkt[0]);
			// Delete tpkt
			delete [] tpkt;

			return (-1);
		}

		// get tpkt data length
		len = ((tpkt[2]) << 8) + tpkt[3] - 4;

		// read tpkt data
		char* tmpBuf = NULL;
		// Set buffer size in multiple of two.
		if ((len % 2) == 0)
		{
			tmpBuf = new char[len];
		} else
		{
			tmpBuf = new char[len + 1];
		}
		info->buffer = tmpBuf;

		noOfBytesRead = readNoOfBytes (info->handle, info->buffer, len);

		if (noOfBytesRead != len)
		{
			CDSU_TRACE1 (1, "Incomplete Read noOfBytesRead = %d\n", noOfBytesRead);
			CDSU_TRACE1 (1, "length = %d\n", len);
		}
		if (noOfBytesRead == 0)
		{
			// Socket disconnect by Peer. 
			CDSU_TRACE ( 1, "disconnect by peer\n");
			// Delete tmpBuf		
			delete [] tmpBuf;
			// Delete tpkt
			delete [] tpkt;

			return (0);

		} else if (noOfBytesRead == -1)
		{
			// Error is there.
			CDSU_TRACE ( 1, "socket read error\n");
			// Delete tmpBuf		
			delete [] tmpBuf;
			// Delete tpkt
			delete [] tpkt;

			return (-1);

		} else 
		{
			// Delete tpkt
			delete [] tpkt;
			return (noOfBytesRead);
		}

	} else
	{
		//----------------------------------------------------------------------------
		// Read data without tpkt
		//----------------------------------------------------------------------------
		int len = nhDb->getMaxBufferLen ();
		int noOfBytesRead;
		info->buffer = NULL;
		// Set buffer size in multiple of two.
		if ((len % 2) == 0)
		{
			info->buffer = new char[len];
		} else
		{
			info->buffer = new char[len + 1];
			len = len + 1;
		} // if (len %...)

		noOfBytesRead = recv (info->handle, info->buffer, len, 0);
		
		// This Section modified after bugs reported:bs/30
		if (noOfBytesRead <= 0)
		{
			delete [] info->buffer;
			info->buffer = NULL;
		}
		
		if (noOfBytesRead < 0) 
		{
			if (errno == EINTR)
			{
				CDSU_TRACE (1, "CdSuNhTcp::recvPacket (): A Non Blocked signal was caught !!!!\n");
			} else
			{
				CDSU_TRACE (1, "CdSuNhTcp::recvPacket (): Read Error !!!!\n");
			}

		} else if (noOfBytesRead == 0)
		{
			// Connection Close By Peer.
			CDSU_TRACE (1, "CdSuNhTcp::Connection Close by Peer\n");
		} else 
		{
			// Successfull Read. Return no of byte read
			// CDSU_TRACE (5, "SUCCESSFULL READ of Data\n");
		}
		
		return (noOfBytesRead);
		// if (recv ...)
	} // if (tpkt ...)

	return (-1);

} // CdSuNhTcp::recvPacket ()

//---------------------------------------------------------------------------
// Method	: sendPacket () 
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::sendPacket (const CdSuNhTcpUiDataReqParam dparam)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpConnInfo* connInfo = NULL; 
	CdSuNhTcpRspCode rspc;
	CdSuNhTcpFailCode failc = CDSU_NHTCP_FCODE_NOT_SET;
	
	int length = dparam.dataLen;
	int nob;				// For no. of bytes written.

	// Lookup in connInfo tables.
	if (connId2ConnInfoTable->lookup (dparam.connKey.connId, connInfo) != true)
	{
		// No Connection.
		CDSU_TRACE (1, "Connection does not exist.\n");

		// Set -ve response code.
		rspc = CDSU_NHTCP_RSPCODE_FAIL;
		failc = CDSU_NHTCP_FCODE_NO_CONN;

	} else 
	{
		//-------------------------------------------------------------
		// Connection exists, write on it.
		//-------------------------------------------------------------
		//-------------------------------------------------------------
		// Check for TPKT flag.
		//-------------------------------------------------------------
		if (connInfo->tpkt)
		{
			// set RFC1006 TPKT length
			unsigned char* tpkt = new unsigned char[4];

			tpkt[0] = 3;		// Version 3
			tpkt[1] = 0;
			int len;
			len = length + 4;       // sizeof (tpkt) replaced by 4
			// CDSU_TRACE1 (5, "Length of tpkt being sent is %d\n", len);
			tpkt[2] = (unsigned char) (len >> 8);
			tpkt[3] = (unsigned char) len;

			// send tpkt header 
			nob = writeNoOfBytes (connInfo->handle, (char*) tpkt, 4); 
			if (nob != 4)
			{
				CDSU_TRACE1 (1, "Tried to write in TPKT %d\n", length);
				CDSU_TRACE1 (1, "Able to write in TPKT %d\n", nob);
				CDSU_TRACE (1, "Write Error in TPKT\n");
				// Set -ve response code.
				rspc = CDSU_NHTCP_RSPCODE_FAIL;
				failc = CDSU_NHTCP_FCODE_TPKT_WRITE_ERR;
			}
			// send Data.
			nob = writeNoOfBytes (connInfo->handle, 
					     dparam.dataPtr,
					     dparam.dataLen);

			if (nob != length)
			{
				CDSU_TRACE1 (1, "Tried to write in Data %d\n", length);
				CDSU_TRACE1 (1, "Able to write in Data %d\n", nob);
				CDSU_TRACE (1, "Write Error in Data \n");
				// Set -ve response code.
				rspc = CDSU_NHTCP_RSPCODE_FAIL;
				failc = CDSU_NHTCP_FCODE_DATA_WRITE_ERR;


			} else 
			{
				// Set +ve response code.
				rspc = CDSU_NHTCP_RSPCODE_SUCCESS;

			} // if (nob != length)
			// Delete tpkt.
			delete [] tpkt;


		} else
		{
			//---------------------------------------------------------
			// Send data without tpkt.
			//---------------------------------------------------------
			if ( (nob = send (connInfo->handle, 
					     dparam.dataPtr,
					     dparam.dataLen, 0)) <= 0) 
			{
				if (errno == EINTR)
				{

					CDSU_TRACE (1, "CdSuNhTcp::sendPacket() : A Non Blocked signal was caught. \n");
					// Set -ve response code.
					rspc = CDSU_NHTCP_RSPCODE_FAIL;
					failc = CDSU_NHTCP_FCODE_UNKNOWNERROR;

				}
				else
				{
					CDSU_TRACE (1, "CdSuNhTcp::sendPacket() : Write Error !!! \n");
					// Set -ve response code.
					rspc = CDSU_NHTCP_RSPCODE_FAIL;
					failc = CDSU_NHTCP_FCODE_DATA_WRITE_ERR;

				} // if
				
			} else
			{
				// Set +ve response code.
				rspc = CDSU_NHTCP_RSPCODE_SUCCESS;

			} // if (send ...)

		} // if (connInfo->tpkt)	
		
	} // if (lookup...)
	
	// Build data response event.
	CdSuNhTcpUiDataRspEvent* evt = new  CdSuNhTcpUiDataRspEvent ();
	evt->init (dparam.connKey,
		      dparam.applActorId,
		      CDSU_NHTCP_UI_DATA_RSP,
		      rspc,
		      failc);

	// Extract Event Handler.
	CdSuNhTcpEventHlr eventHlr = dparam.conEventHlr; 
	if (eventHlr != NULL)
	{
		// Notify event to specified
		// connection event handler.
		eventHlr((void*) evt);
	} else
	{

		// Notify event to default
		// connection event handler.
		cdSuNhTcpDefConEventHlr (*evt);	
		delete evt;
	} // if

		// Return.
	if (rspc != CDSU_NHTCP_RSPCODE_SUCCESS)
	{
		return (false);
	} else
	{
		return (true);
	}

} // CdSuNhTcp::sendPacket ()

//---------------------------------------------------------------------------
// Method	: readNoOfBytes () 
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

int CdSuNhTcp::readNoOfBytes (int fd, char* vptr, int noOfBytes)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	int nleft;
	int nread;
	char *ptr;

	//(void*) ptr = vptr;
	ptr = vptr;
	nleft = noOfBytes;
	while (nleft > 0)
	{
		if ((nread = recv (fd, ptr, nleft, 0)) < 0) 
		{
			if (errno == EINTR)
				nread = 0;		// and call read() again 
			else
			{
				CDSU_TRACE (1, "Read Error !!!!");
				return(-1);
			}

		} else if (nread == 0)
			break;				

		nleft -= nread;
		ptr   += nread;
	}

	return (noOfBytes - nleft);	// return >= 0


} // CdSuNhTcp::readNoOfBytes ()

//---------------------------------------------------------------------------
// Method	: writeNoOfBytes () 
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

int CdSuNhTcp::writeNoOfBytes (int fd, const char* vptr, int noOfBytes)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	int		nleft;
	int		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = noOfBytes;
	while (nleft > 0)
	{
		if ((nwritten = send (fd, ptr, nleft, 0)) <= 0) 
		{
			if (errno == EINTR)
			{
				CDSU_TRACE (1, "CdSuNhTcp::sendPacket() : A Non Blocked signal was caught. \n");
				continue;

			}
			else
			{
				CDSU_TRACE (1, "CdSuNhTcp::sendPacket() : Write Error !!! \n");
				return (-1);

			} // if

		} // if

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return (noOfBytes);

} // CdSuNhTcp::writeNoOfBytes ()

//---------------------------------------------------------------------------
// Method	: changeState () 
// Description  : 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::changeState (CdSuNhTcpState tcpNhState)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	nhState = tcpNhState;

} // CdSuTcp::changeState ()

//----------------------------------------------------------------------------
// State Handlers.
//----------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Method	: createdStHlr() 
// Description  : It handles message for state CDSU_NHTCP_ST_CREATED. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::createdStHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	switch (msg->msgHdr.opcode)
	{
	case CDSU_NHTCP_MI_CFG_REQ:
		configReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_MI_CTRL_REQ:
		ctrlReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_MI_DELNH_REQ:
		delNhReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_MI_STATUS_REQ:
		statusReqMsgHlr (msg);
		break;
	default:
		outOfSeqMsgHlr (msg); 
		break;

	} // switch ()

} // CdSuNhTcp::createdStHlr ()

//---------------------------------------------------------------------------
// Method	: runningStHlr() 
// Description  : It handles message for state CDSU_NHTCP_ST_RUNNING . 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::runningStHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	switch (msg->msgHdr.opcode)
	{	
	case CDSU_NHTCP_MI_CTRL_REQ:			// NH Control Req.
		ctrlReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_MI_DELNH_REQ:			// NH delete Req.
		delNhReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_MI_STATISTICS_REQ:		// NH Statistics Req.
		statsReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_MI_STATUS_REQ:			// NH Status Req.
		statusReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_UI_ADD_LISTENER_REQ:		// Add Lstn Req.
		addLstnMsgHlr (msg);
		break;

	case CDSU_NHTCP_UI_REM_LISTENER_REQ:		// Remove Lstn Req.
		remLstnMsgHlr (msg);
		break;

	case CDSU_NHTCP_UI_CONNECT_REQ:			// Establish Connection.
		conReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_UI_DATA_REQ:			// Transmit Data Req.
		txDataReqMsgHlr (msg);
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_REQ:		// Close Connection Req.
		discReqMsgHlr (msg);
		break;

	default:
		outOfSeqMsgHlr (msg); 
		break;

	} // switch
	

} // CdSuNhTcp::runningStHlr ()

//---------------------------------------------------------------------------
// Method	: outOfSeqMsgHlr () 
// Description  : It handles out of sequence message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::outOfSeqMsgHlr (CdMessage* msg)
{
	switch (msg->msgHdr.opcode)
	{
	case CDSU_NHTCP_MI_DELNH_REQ:
		{
		CdSuNhTcpMiDelNhRspEvent*  delNhRspEvent = new CdSuNhTcpMiDelNhRspEvent;
		delNhRspEvent->init (nhDb->getNhInstId (),
				nhDb->getSmActorId (),
				CDSU_NHTCP_MI_DELNH_RSP,
				CDSU_NHTCP_RSPCODE_FAIL,
				CDSU_NHTCP_FCODE_INV_OPERATION);
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		nhDb->getSmEventHlr () ( (void*)delNhRspEvent);	
		}
		break;

	case CDSU_NHTCP_MI_STATISTICS_REQ:
		{
		CdSuNhTcpMiStsRspEvent* stsRspEvent = 
					new CdSuNhTcpMiStsRspEvent; 
		//---------------------------------------------------------
		// call callback function.
		//---------------------------------------------------------
		stsRspEvent->init (nhDb->getNhInstId (),
			nhDb->getSmActorId (),
			CDSU_NHTCP_MI_STATISTICS_RSP,
			CDSU_NHTCP_RSPCODE_FAIL,
			CDSU_NHTCP_FCODE_INV_OPERATION, 
			NULL);

		nhDb->getSmEventHlr () ( (void*)stsRspEvent);	
		}
		break;
	
	case CDSU_NHTCP_UI_ADD_LISTENER_REQ:
		{
		CdSuNhTcpUiAddLstnReqMsg* addlstnmsg =  (CdSuNhTcpUiAddLstnReqMsg*) msg;
		// Form Lstn Key.
		CdSuNhTcpLstnKey lstnKey;
		lstnKey.init (addlstnmsg->lstnId, nhDb->getNhInstId ());
		// Send failure response.
		CdSuNhTcpUiAddLstnRspEvent* evt = new CdSuNhTcpUiAddLstnRspEvent ();
		evt->init (lstnKey, 
			   addlstnmsg->addLstnReqParam.applActorId,
			   CDSU_NHTCP_UI_ADD_LISTENER_RSP, 
			   CDSU_NHTCP_RSPCODE_FAIL,
			   CDSU_NHTCP_FCODE_INV_OPERATION);

		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = addlstnmsg->addLstnReqParam.listenEventHlr; 

		if (eventHlr != NULL)
		{
			// Notify event to specified listener event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default listener event handler.
			cdSuNhTcpDefLstnEventHlr (*evt);	
			delete evt;
		} // if

		}
		break;
	
	case CDSU_NHTCP_UI_REM_LISTENER_REQ:
		{
		CdSuNhTcpUiRemLstnReqMsg* rmLstnMsg = (CdSuNhTcpUiRemLstnReqMsg*) msg;
		CdSuNhTcpUiRemLstnReqParam rmlstnparam = rmLstnMsg->remLstnReqParam;
		// form lstn key.
		CdSuNhTcpLstnKey lstnkey;
		lstnkey.init (rmlstnparam.lstnKey.lstnId, nhDb->getNhInstId ());
		// build remove lstn event.
		CdSuNhTcpUiRemLstnRspEvent* evt = new
				CdSuNhTcpUiRemLstnRspEvent ();
		evt->init (lstnkey,
			   rmlstnparam.applActorId,
			   CDSU_NHTCP_UI_REM_LISTENER_RSP,
			   CDSU_NHTCP_RSPCODE_FAIL,
			   CDSU_NHTCP_FCODE_INV_OPERATION);

		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = rmlstnparam.listenEventHlr; 
		if (eventHlr != NULL)
		{
			// Notify event to specified listener event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default listener event handler.
			cdSuNhTcpDefLstnEventHlr (*evt);	
			delete evt;
		} // if

		}
		break;
	
	case CDSU_NHTCP_UI_CONNECT_REQ:
		{
		CdSuNhTcpUiConReqMsg* conReqMsg = (CdSuNhTcpUiConReqMsg*)msg;
		// Form Conn Key.
		CdSuNhTcpConnKey connKey;
		connKey.init (conReqMsg->connId, nhDb->getNhInstId ());
		// Send failure response.
		CdSuNhTcpUiConRspEvent* evt = new CdSuNhTcpUiConRspEvent ();
		evt->init (connKey, 
			   conReqMsg->conReqParam.applActorId,
			   CDSU_NHTCP_UI_CONNECT_RSP, 
			   CDSU_NHTCP_RSPCODE_FAIL,
			   CDSU_NHTCP_FCODE_INV_OPERATION);
		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = conReqMsg->conReqParam.conEventHlr; 
		if (eventHlr != NULL)
		{
			// Notify event to specified
			// connection event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default
			// connection event handler.
			cdSuNhTcpDefConEventHlr (*evt);	
			delete evt;
		} // if
		

		}
		break;

	case CDSU_NHTCP_UI_DATA_REQ:	
		{
		CdSuNhTcpUiDataReqMsg* dataReqMsg = (CdSuNhTcpUiDataReqMsg*)msg;
		CdSuNhTcpUiDataReqParam dparam = dataReqMsg->dataReqParam;

		// Build data response event.
		CdSuNhTcpUiDataRspEvent* evt = new  CdSuNhTcpUiDataRspEvent ();
		evt->init (dparam.connKey,
			      dparam.applActorId,
			      CDSU_NHTCP_UI_DATA_RSP,
   			      CDSU_NHTCP_RSPCODE_FAIL,
			      CDSU_NHTCP_FCODE_INV_OPERATION);


		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = dparam.conEventHlr; 
		if (eventHlr != NULL)
		{
			// Notify event to specified
			// connection event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default
			// connection event handler.
			cdSuNhTcpDefConEventHlr (*evt);	
			delete evt;
		} // if

	
		}
		break;

	case CDSU_NHTCP_UI_CLOSE_CON_REQ:	
		{
		CdSuNhTcpUiCloseConReqMsg* clConMsg = (CdSuNhTcpUiCloseConReqMsg*) msg;
		CdSuNhTcpUiCloseConReqParam clconreqparam = clConMsg->closeConReqParam;
		// Form Conn Key.
		CdSuNhTcpConnKey connKey;
		connKey.init (clconreqparam.connKey.connId, nhDb->getNhInstId ());
		// Build close connection event
		CdSuNhTcpUiCloseConRspEvent* evt = new
				CdSuNhTcpUiCloseConRspEvent ();
		evt->init (connKey,
			   clconreqparam.applActorId,
			   CDSU_NHTCP_UI_CLOSE_CON_RSP,
			   CDSU_NHTCP_RSPCODE_FAIL,
			   CDSU_NHTCP_FCODE_INV_OPERATION);

		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = clconreqparam.conEventHlr; 
		if (eventHlr != NULL)
		{
			// Notify event to specified
			// connection event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default
			// connection event handler.
			cdSuNhTcpDefConEventHlr (*evt);	
			delete evt;
		} // if

		}
		break;

	default:
		// Discard.
		CDSU_TRACE (1, "Unknown Msg.\n ");
		break;

	} // switch ()

	// CDSU_FREEMSGBUF (msg);

} // void CdSuNhTcp::outOfSeqMsgHlr (CdMessage* msg)

//----------------------------------------------------------------------------
// SM Message Handlers.
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: configReqMsgHlr () 
// Description  : It handles Configure Request. Calls the sm Event Handler
// 		  if it is specified otherwise send the message directly. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
// Notes	: *) We are not considering General Config Msgs now.
//---------------------------------------------------------------------------

void CdSuNhTcp::configReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhTcpMiCfgReqMsg* cfgMsg = (CdSuNhTcpMiCfgReqMsg*) msg;

	// Extract the Sm Event Hlr.
	CdSuNhTcpEventHlr eventHlr = nhDb->getSmEventHlr ();
	
	//--------------------------------------------------------------------
	// Build Config Response Event.
	//--------------------------------------------------------------------
	CdSuNhTcpMiCfgRspEvent* rspEvent = new CdSuNhTcpMiCfgRspEvent;
	rspEvent->init (nhDb->getNhInstId (), 
			nhDb->getSmActorId (),
			CDSU_NHTCP_MI_CFG_RSP, 
			CDSU_NHTCP_RSPCODE_SUCCESS, 
			CDSU_NHTCP_FCODE_NOT_SET,
			CDSU_NHTCP_SO_INITIAL_CFG_RSP);
	
	//--------------------------------------------------------------------
	// Call Event Handler or post message.
	//--------------------------------------------------------------------
	if (eventHlr != NULL)
	{
		// If Event Handler has been specified.
		CDSU_TRACE (5, "Event Handler has been specified\n");
		eventHlr ((void*) rspEvent);
	} else
	{
		CDSU_TRACE (5, "Event Handler has not been specified\n");
		cdSuNhTcpDefSmEventHlr (*rspEvent);	
		delete rspEvent;
	}

	CDSU_FREEMSGBUF (cfgMsg);

} // CdSuNhTcp::configReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: delNhReqMsgHlr () 
// Description  : It handles delete NH message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::delNhReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpMiDelNhReqMsg* delNhReqMsg  = (CdSuNhTcpMiDelNhReqMsg*) msg;
	// Extract the SM Event Handler.
	CdSuNhTcpEventHlr eventHlr = nhDb->getSmEventHlr ();
	CdSuNhTcpMiDelNhRspEvent* rspEvent = new  CdSuNhTcpMiDelNhRspEvent;
	
	if (stopFlag != false)
	{
		CDSU_FREEMSGBUF (delNhReqMsg);
		for (CdSuIntHash<CdSuNhTcpLstnInfo*>::iterator iter = handle2ListenInfoTable->begin (); 
		iter != handle2ListenInfoTable->end (); ++iter)
		{
			// To Fix Bugs bs/32
			// Close socket.
			if (close ( (*iter)->handle) == -1)
			{
				CDSU_TRACE1 (2, "~CdSuNhTcp - close failed.  sockfd = %d\n", (*iter)->handle);
			} else
			{
				CDSU_TRACE1 (2, "~CdSuNhTcp - Listen Socket close success.  sockfd = %d\n", (*iter)->handle);
			}
		} // for 
		
		for (CdSuIntHash<CdSuNhTcpConnInfo*>::iterator iter = handle2ConnInfoTable->begin ();
				iter != handle2ConnInfoTable->end (); ++iter)
		{
			// To Fix Bugs bs/32
			// Close tcpHandle for disconnection. 
			if (shutdown ( (*iter)->handle, 2) == -1)
			{
				CDSU_TRACE1 (2, "~CdSuNhTcp - shutdown failed.  sockfd = %d\n",
					(*iter)->handle);
			}
			if (close ( (*iter)->handle) == -1)
			{
				CDSU_TRACE1 (2, "~CdSuNhTcp - Conn Socket close failed.  sockfd = %d\n", (*iter)->handle);
			} else
			{
				CDSU_TRACE1 (2, "~CdSuNhTcp - Conn Socket close success.  sockfd = %d\n", (*iter)->handle);
			}
		} // for

		//------------------------------------------------------------
		// Build Delete Success Response Event.
		//------------------------------------------------------------
		rspEvent->init (nhDb->getNhInstId (), 
				nhDb->getSmActorId (),
				CDSU_NHTCP_MI_DELNH_RSP,
				CDSU_NHTCP_RSPCODE_SUCCESS,
				CDSU_NHTCP_FCODE_NOT_SET);

		// Deleted Flag.
		delFlag = true;
		//---------------------------------------------------------
		// Call Event Handler or post message.
		//---------------------------------------------------------
		if (eventHlr != NULL)
		{
			// If Event Handler has been specified.
			eventHlr ((void*)rspEvent);

		} else
		{
			cdSuNhTcpDefSmEventHlr (*rspEvent);	
			delete rspEvent;
		}
		
		pthread_exit ((void*) NULL);
	} else
	{
		//------------------------------------------------------------
		// Build Delete Fail Response Event.
		//------------------------------------------------------------
		rspEvent->init (nhDb->getNhInstId (), 
				nhDb->getSmActorId (),
				CDSU_NHTCP_MI_DELNH_RSP,
				CDSU_NHTCP_RSPCODE_FAIL,
				CDSU_NHTCP_FCODE_INV_OPERATION);

		//---------------------------------------------------------
		// Call Event Handler or post message.
		//---------------------------------------------------------
		if (eventHlr != NULL)
		{
			// If Event Handler has been specified.
			eventHlr ((void*)rspEvent);
		} else
		{
			cdSuNhTcpDefSmEventHlr (*rspEvent);	
			delete rspEvent;
		}

	} // if (stopflag)

} // CdSuNhTcp::delNhReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: ctrlReqMsgHlr () 
// Description  : It handles delete NH message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::ctrlReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpMiCtrlReqMsg* ctrlMsg = (CdSuNhTcpMiCtrlReqMsg*) msg;

	//--------------------------------------------------------------------
	// Extract the SM Event Handler.
	//--------------------------------------------------------------------
	CdSuNhTcpEventHlr eventHlr = nhDb->getSmEventHlr ();


	//--------------------------------------------------------------------
	// Switch on the type of Control Request
	//--------------------------------------------------------------------
	switch (ctrlMsg->ctrlReqParam.subOpcode)
	{
	case CDSU_NHTCP_SO_START_REQ:
		{
		// Reset Stop flag.	
		stopFlag = false;
		// Change the state.
		changeState (CDSU_NHTCP_ST_RUNNING);
		
		//------------------------------------------------------------
		// Build Control Response Event.
		//------------------------------------------------------------
		CdSuNhTcpMiCtrlRspEvent* rspEvent = new CdSuNhTcpMiCtrlRspEvent;
		rspEvent->init (nhDb->getNhInstId (), 
				nhDb->getSmActorId (),
				CDSU_NHTCP_MI_CTRL_RSP, 
				CDSU_NHTCP_RSPCODE_SUCCESS, 
				CDSU_NHTCP_FCODE_NOT_SET,
				CDSU_NHTCP_SO_START_RSP);

		//---------------------------------------------------------
		// Call Event Handler or post message.
		//---------------------------------------------------------
		if (eventHlr != NULL)
		{
			// If Event Handler has been specified.
			eventHlr ((void*)rspEvent);

		} else
		{
			cdSuNhTcpDefSmEventHlr (*rspEvent);	
			delete rspEvent;
		}

		} // case
		break;
	
	case CDSU_NHTCP_SO_STOP_REQ:
		{
		// Set Stop flag.	
		stopFlag = true;
		// Change the State.
		changeState (CDSU_NHTCP_ST_CREATED);

		//------------------------------------------------------------
		// Build Control Response Event.
		//------------------------------------------------------------
		CdSuNhTcpMiCtrlRspEvent* rspEvent = new CdSuNhTcpMiCtrlRspEvent;
		rspEvent->init (nhDb->getNhInstId (), 
				nhDb->getSmActorId (),
				CDSU_NHTCP_MI_CTRL_RSP, 
				CDSU_NHTCP_RSPCODE_SUCCESS, 
				CDSU_NHTCP_FCODE_NOT_SET,
				CDSU_NHTCP_SO_STOP_RSP);

		//------------------------------------------------------------
		// Call Event Handler or post message.
		//------------------------------------------------------------
		if (eventHlr != NULL)
		{
			// Event Handler specified.
			eventHlr ((void*)rspEvent);

		} else
		{
			cdSuNhTcpDefSmEventHlr (*rspEvent);	
			delete rspEvent;
		}

		} // case
		break;

	default: 
		{
		// Do Nothing.
		CDSU_TRACE (1, "Invalid type of Control Request\n");
		}
		break;

	} // switch ()

	// Free rxvd msg buf.
	CDSU_FREEMSGBUF (ctrlMsg);

} // CdSuNhTcp::ctrlReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: statusReqMsgHlr () 
// Description  : It handles delete NH message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::statusReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhTcpMiStatusReqMsg* statusReqMsg = (CdSuNhTcpMiStatusReqMsg*) msg;

	//--------------------------------------------------------------------
	// Extract the Event Handler.
	//--------------------------------------------------------------------
	CdSuNhTcpEventHlr eventHlr = nhDb->getSmEventHlr ();

	//--------------------------------------------------------------------
	// Build Status Response Event.
	//--------------------------------------------------------------------
	CdSuNhTcpMiStatusRspEvent* rspEvent = new CdSuNhTcpMiStatusRspEvent;
	rspEvent->init (nhDb->getNhInstId (), 
			nhDb->getSmActorId (),
			CDSU_NHTCP_MI_STATUS_RSP, 
			CDSU_NHTCP_RSPCODE_SUCCESS, 
			CDSU_NHTCP_FCODE_NOT_SET,
			nhState);

	//--------------------------------------------------------------------
	// Call Event Handler or post message.
	//--------------------------------------------------------------------
	if (eventHlr != NULL)
	{
		// Event Handler specified.
		eventHlr ((void*) rspEvent);

	} else
	{

		cdSuNhTcpDefSmEventHlr (*rspEvent);	
		delete rspEvent;
	}

	CDSU_FREEMSGBUF (statusReqMsg);

} // CdSuNhTcp::statusReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: statsReqMsgHlr () 
// Description  : It handles Statistics Request Message.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::statsReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhTcpMiStsReqMsg* stsReqMsg = (CdSuNhTcpMiStsReqMsg*) msg;

	//--------------------------------------------------------------------
	// Extract the Event Handler.
	//--------------------------------------------------------------------
	CdSuNhTcpEventHlr eventHlr = nhDb->getSmEventHlr ();

	//---------------------------------------------------------
	// Build Statistics Response Event.
	//---------------------------------------------------------
	CdSuNhTcpMiStsRspEvent* rspEvent = new CdSuNhTcpMiStsRspEvent;

	rspEvent->init (nhDb->getNhInstId (), 
			nhDb->getSmActorId (),
			CDSU_NHTCP_MI_STATISTICS_RSP, 
			CDSU_NHTCP_RSPCODE_SUCCESS, 
			CDSU_NHTCP_FCODE_NOT_SET,
			getStatsCtrs ());

	//---------------------------------------------------------
	// Call Event Handler or post message.
	//---------------------------------------------------------
	if (eventHlr != NULL)
	{
		// Event Handler specified.
		eventHlr ((void*)rspEvent);

	} else
	{

		cdSuNhTcpDefSmEventHlr (*rspEvent);	
		delete rspEvent;
	}

	CDSU_FREEMSGBUF (stsReqMsg);

} // CdSuNhTcp::statsReqMsgHlr ()


//---------------------------------------------------------------------------- // User Layer Message Handlers.
// Application Messages.
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method	: addLstnMsgHlr () 
// Description  : It handles add listener message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::addLstnMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpUiAddLstnReqMsg* addlstnmsg =  (CdSuNhTcpUiAddLstnReqMsg*) msg;

	// Update statistics counter.
	incrStatsCtrs (CDSU_NHTCP_ADDLSTNREQ_CTR);
	//----------------------------------------------------------------------------
	// Check : if connection request is exceeding Max. no of conn.
	//----------------------------------------------------------------------------
	int noOfLstnPresent = handle2ListenInfoTable->size ();
	int maxNoOfLstn = nhDb->getMaxNoOfLnrs ();
	CdSuNhTcpFailCode retCode = CDSU_NHTCP_FCODE_NOT_SET;
	
	if (noOfLstnPresent < maxNoOfLstn)
	{

		 // Add Listener.
		 retCode = addListener (addlstnmsg->addLstnReqParam.ipv4addr, 
					   addlstnmsg->addLstnReqParam.tcpPortNo,
					   addlstnmsg->addLstnReqParam.listenEventHlr, 
					   addlstnmsg->addLstnReqParam.conEventHlr,
					   addlstnmsg->addLstnReqParam.applActorId,
					   addlstnmsg->lstnId,
					   addlstnmsg->addLstnReqParam.tpkt);
	} else
	{
		CDSU_TRACE (1, "CdSuNhTcp::addLstnMsgHlr (): Add Listener request is exceeding Max. no of Lstn.\n ");
		retCode =  CDSU_NHTCP_FCODE_TCPLSTN_OVERFLOW;
	}

	if (retCode !=  CDSU_NHTCP_FCODE_NOT_SET)
	{
		// Failed to add listener.	
		CDSU_TRACE (1, "CdSuNhTcp::addLstnMsgHlr (): Add Listener failed.\n ");
		
		// Form Lstn Key.
		CdSuNhTcpLstnKey lstnKey;
		lstnKey.init (addlstnmsg->lstnId, nhDb->getNhInstId ());
		// Send failure response.
		CdSuNhTcpUiAddLstnRspEvent* evt = new CdSuNhTcpUiAddLstnRspEvent ();
		evt->init (lstnKey, 
			   addlstnmsg->addLstnReqParam.applActorId,
			   CDSU_NHTCP_UI_ADD_LISTENER_RSP, 
			   CDSU_NHTCP_RSPCODE_FAIL,
			   retCode);

		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = addlstnmsg->addLstnReqParam.listenEventHlr; 

		if (eventHlr != NULL)
		{
			// Notify event to specified listener event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default listener event handler.
			cdSuNhTcpDefLstnEventHlr (*evt);	
			delete evt;
		} // if


	} else 
	{
		// Added listener successfully.	
		CDSU_TRACE (1, "CdSuNhTcp::addLstnMsgHlr (): Add Listener successful.\n ");
		// Update statistics counter.
		incrStatsCtrs (CDSU_NHTCP_SUCCESS_ADDLSTN_CTR);

	
	} // if
	
	// Delete rxvd msg.
	CDSU_FREEMSGBUF (addlstnmsg);

} // CdSuNhTcp::addLstnMsgHlr ()

//---------------------------------------------------------------------------
// Method	: remLstnMsgHlr () 
// Description  : It handles remove listener message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::remLstnMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpUiRemLstnReqMsg* rmLstnMsg = (CdSuNhTcpUiRemLstnReqMsg*) msg;

	// Update statistics counter.
	incrStatsCtrs (CDSU_NHTCP_REMLSTNREQ_CTR);

	bool retval =  remListener (rmLstnMsg->remLstnReqParam);

	if (retval != true)	
	{
		CDSU_TRACE (1, "CdSuNhTcp::remLstnMsgHlr (): Remove Listener failed.\n ");
	} else
	{

		CDSU_TRACE (3, "CdSuNhTcp::remLstnMsgHlr (): Remove Listener successful.\n ");
		// Update statistics counter.
		incrStatsCtrs (CDSU_NHTCP_SUCCESS_REMLSTN_CTR);

	}

	// Delete rxvd msg.
	CDSU_FREEMSGBUF (rmLstnMsg);


} // CdSuNhTcp::remLstnMsgHlr ()

//---------------------------------------------------------------------------
// Method	: conReqMsgHlr () 
// Description  : It handles establish connection message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::conReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpUiConReqMsg* conReqMsg = (CdSuNhTcpUiConReqMsg*)msg;
	
	// Update statistics counter.
	incrStatsCtrs (CDSU_NHTCP_CONREQ_CTR);

	//----------------------------------------------------------------------------
	// Check : if connection request is exceeding Max. no of conn.
	//----------------------------------------------------------------------------
	int noOfConPresent = handle2ConnInfoTable->size ();
	int maxNoOfConn = nhDb->getMaxNoOfConn ();
	CdSuNhTcpFailCode retCode = CDSU_NHTCP_FCODE_NOT_SET;

	if (noOfConPresent < maxNoOfConn)
	{
		retCode = createConn (conReqMsg->conReqParam, conReqMsg->connId);
	} else
	{
		CDSU_TRACE (1, "CdSuNhTcp::conReqMsgHlr (): Connection request is exceeding Max. no of conn.\n ");
		retCode =  CDSU_NHTCP_FCODE_TCPCONN_OVERFLOW;
	}
	

	if (retCode != CDSU_NHTCP_FCODE_NOT_SET)
	{
		// Failed to connect.	
		CDSU_TRACE (1, "CdSuNhTcp::conReqMsgHlr (): Connect failed.\n ");
		
		// Form Conn Key.
		CdSuNhTcpConnKey connKey;
		connKey.init (conReqMsg->connId, nhDb->getNhInstId ());
		// Send failure response.
		CdSuNhTcpUiConRspEvent* evt = new CdSuNhTcpUiConRspEvent ();
		evt->init (connKey, 
			   conReqMsg->conReqParam.applActorId,
			   CDSU_NHTCP_UI_CONNECT_RSP, 
			   CDSU_NHTCP_RSPCODE_FAIL,
			   retCode);
		// Extract Event Handler.
		CdSuNhTcpEventHlr eventHlr = conReqMsg->conReqParam.conEventHlr; 
		if (eventHlr != NULL)
		{
			// Notify event to specified
			// connection event handler.
			eventHlr((void*) evt);
		} else
		{

			// Notify event to default
			// connection event handler.
			cdSuNhTcpDefConEventHlr (*evt);	
			delete evt;
		} // if
		
	} else 
	{
		// Established connection successfully.	
		CDSU_TRACE (3, "CdSuNhTcp::conReqMsgHlr (): Connect successful.\n ");
		// Update statistics counter.
		incrStatsCtrs (CDSU_NHTCP_SUCCESS_CON_CTR);

	} 

	// Delete rxvd msg.
	CDSU_FREEMSGBUF (conReqMsg);
	
} // CdSuNhTcp::conReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: txDataReqMsgHlr () 
// Description  : It handles transmit data request message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::txDataReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpUiDataReqMsg* dataReqMsg = (CdSuNhTcpUiDataReqMsg*)msg;

	// Update statistics counter.
	incrStatsCtrs (CDSU_NHTCP_DATAREQ_CTR);

	//----------------------------------------------------------------------------
	// Check : if data length  is exceeding Max. buffer length.
	//----------------------------------------------------------------------------
	bool retval;
	int maxNoOfBufLen = nhDb->getMaxBufferLen ();
	int dlen = dataReqMsg->dataReqParam.dataLen;
	if (dlen <= maxNoOfBufLen)
	{
		retval = sendPacket (dataReqMsg->dataReqParam);
	} else
	{
		retval = false;
		CDSU_TRACE (1, "CdSuNhTcp::txDataReqMsgHlr (): Data length is exceeding Max. buffer length. \n ");
	}

	// Delete rxvd dataPtr
	if (dataReqMsg->dataReqParam.dataPtr != NULL)
	{
		delete [] dataReqMsg->dataReqParam.dataPtr;
		dataReqMsg->dataReqParam.dataPtr = NULL;
	}

	if (retval != true)	
	{
		CDSU_TRACE (1, "CdSuNhTcp::txDataReqMsgHlr (): Send packet failed.\n ");
	} else
	{

		CDSU_TRACE (3, "CdSuNhTcp::txDataReqMsgHlr (): Send packet successful.\n ");

		// Update statistics counter.
		incrStatsCtrs (CDSU_NHTCP_SUCCESS_DATATX_CTR);

	}
	
	// Delete rxvd msg.
	CDSU_FREEMSGBUF (dataReqMsg);

} // CdSuNhTcp::txDataReqMsgHlr ()

//---------------------------------------------------------------------------
// Method	: discReqMsgHlr () 
// Description  : It handles close connection request message. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::discReqMsgHlr (CdMessage* msg)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	CdSuNhTcpUiCloseConReqMsg* clConMsg = (CdSuNhTcpUiCloseConReqMsg*) msg;

	// Update statistics counter.
	incrStatsCtrs (CDSU_NHTCP_DISCONREQ_CTR);

	bool retval = closeConn (clConMsg->closeConReqParam);

	if (retval != true)	
	{
		CDSU_TRACE (1, "CdSuNhTcp::discReqMsgHlr (): Close connection failed.\n ");
	} else
	{

		CDSU_TRACE (3, "CdSuNhTcp::discReqMsgHlr (): Close connection successful.\n ");
		// Update statistics counter.
		incrStatsCtrs (CDSU_NHTCP_SUCCESS_CLOSE_CON_CTR);

	}

	// Delete rxvd msg.
	CDSU_FREEMSGBUF (clConMsg);

} // CdSuNhTcp::discReqMsgHlr ()

//--------------------------------------------------------------------
// Utility methods
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
// Method	: connCleanUp () 
// Description  : It clears the connection related links like removing
//	          connection info from tables and shutdown the tcpHandle.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::connCleanUp (CdSuNhTcpConnInfo* info)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	// Close tcpHandle for disconnection. 
	if (shutdown (info->handle, 2) == -1)
	{
		CDSU_TRACE1 (2, "CdSuNhTcp::connCleanUp() - shutdown failed.  sockfd = %d\n",
				info->handle);
		// return (false) Is removed to fix bug Bugzilla/35 
	}
	if (close (info->handle) == -1)
	{
		CDSU_TRACE1 (2, "CdSuNhTcp::connCleanUp() - close failed.  sockfd = %d\n",
				info->handle);
		return (false);
	}
					
	// Remove from handle2ConnInfoTable.
	if (handle2ConnInfoTable->remove (info->handle) != true)
	{
		return (false);
	}
	// Remove from tcpConnId2ConnInfoTable.
	if(connId2ConnInfoTable->remove (info->connectionId) != true)
	{
		return (false);
	}
	// At the end, delete Connection Info.
	delete info;

	return (true);

} // CdSuNhTcp::connCleanUp ()

//---------------------------------------------------------------------------
// Method	: lstnCleanUp () 
// Description  : It clears the listener related links like removing
//	          listener info from tables and shutdown the tcpHandle.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::lstnCleanUp (CdSuNhTcpLstnInfo* info)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	// Shut down tcpHandle.
	if (close (info->handle) == -1)
	{
		return (false);
	}
					
	// Remove from handle2ListenInfoTable.
	if (handle2ListenInfoTable->remove (info->handle) != true)
	{
		return (false);
	}
	// Remove from lstnId2ListenInfoTable.
	if(lstnId2ListenInfoTable->remove (info->listenerId) != true)
	{
		return (false);
	}
	// At the end, delete Listener Info.
	delete info;

	return (true);

} // CdSuNhTcp::lstnCleanUp ()


//---------------------------------------------------------------------------
// Method	: shutDownTcpHandle () 
// Description  : It shut down the given tcpHandle. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

bool CdSuNhTcp::shutDownTcpHandle (CdSuNhTcpHandle tcpHandle)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// Shutdown tcpHandle. 
	if (shutdown (tcpHandle, 2) == -1)
	{
		return (false);			
	}

	// Close tcpHandle.
	if (close (tcpHandle) == -1)
	{
		return (false);
	}

	return (true);			

} // CdSuTcp::shutDownTcpHandle()

//---------------------------------------------------------------------------
// Method	: buildConnInfo() 
// Description  : It builds the connection info structure when connection is 
//		  established from remote. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpConnInfo* CdSuNhTcp::buildConnInfo (CdSuNhTcpHandle tcpHandle, 
			      CdSuNhTcpConnId tcpConnId,
			      struct sockaddr_in* cliaddr,
			      CdSuNhTcpLstnInfo* lstnInfo)

{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpConnInfo* connInfo = new CdSuNhTcpConnInfo ();
	connInfo->connectionId = tcpConnId; 
	connInfo->handle = tcpHandle; 

	// Destination IP Information.
	connInfo->destIpv4Addr = cliaddr->sin_addr.s_addr; 
	connInfo->destPortNo = cliaddr->sin_port;

	// Self Ip Information.
	connInfo->selfIpv4Addr = lstnInfo->listenerIpv4Addr;
	connInfo->selfPortNo = lstnInfo->listenPortNo;

	// Connection Event Handler.	
	connInfo->connectionEventHlr = lstnInfo->connectionEventHlr;
	
	// Connection Event Handler.	
	connInfo->applActorId = lstnInfo->applActorId;

	// TPKT flag.
	connInfo->tpkt = lstnInfo->tpkt;


	return (connInfo);

} // CdSuTcp::buildConnInfo ()

//---------------------------------------------------------------------------
// Method	: buildConnInfo() 
// Description  : It builds the connection info structure when connection is 
//		  established by application as a connect request. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpConnInfo* CdSuNhTcp::buildConnInfo 
		(CdSuNhTcpHandle tcpHandle, 
		 CdSuNhTcpConnId tcpConnId,
		 CdSuIpv4Addr dstIpv4Addr,
		 CdSuIpv4TcpPortNo dstPortNo,
		 CdSuIpv4Addr selfIpv4Addr,
		 CdSuIpv4TcpPortNo selfPortNo,
		 bool tpkt,
		 CdSuNhTcpEventHlr conEventHlr,
		 CdSuActorId applActorId
	         )


{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpConnInfo* connInfo = new CdSuNhTcpConnInfo ();
	connInfo->connectionId = tcpConnId; 
	connInfo->handle = tcpHandle; 

	// Destination IP Information.
	connInfo->destIpv4Addr = dstIpv4Addr; 
	connInfo->destPortNo = dstPortNo;

	// Self Ip Information.
	connInfo->selfIpv4Addr = selfIpv4Addr;
	connInfo->selfPortNo = selfPortNo;

	// Connection Event Handler.	
	connInfo->connectionEventHlr = conEventHlr;
	
	// Connection Event Handler.	
	connInfo->applActorId = applActorId;

	// TPKT flag.
	connInfo->tpkt = tpkt;

	return (connInfo);

} // CdSuTcp::buildConnInfo ()


//---------------------------------------------------------------------------
// Method	: buildListenInfo() 
// Description  : It builds the listener info structure. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpLstnInfo* CdSuNhTcp::buildListenInfo (CdSuNhTcpHandle tcpHandle, 
			      CdSuNhTcpLstnId tcpLstnId,
			      CdSuIpv4Addr lipaddr,
			      CdSuIpv4TcpPortNo lport,
			      CdSuNhTcpEventHlr levthlr,
			      CdSuNhTcpEventHlr cevthlr,
			      CdSuActorId apActorId,
			      bool tpkt)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	CdSuNhTcpLstnInfo* lstnInfo = new CdSuNhTcpLstnInfo ();
	lstnInfo->listenerId = tcpLstnId; 
	lstnInfo->handle = tcpHandle; 

	// Listener IP Information.
	lstnInfo->listenerIpv4Addr = lipaddr;
	lstnInfo->listenPortNo = lport;

	// Listener Event Handler.	
	lstnInfo->listenerEventHlr = levthlr;
	// Connection Event Handler.	
	lstnInfo->connectionEventHlr = cevthlr;
	// Application Actor Id.
	lstnInfo->applActorId = apActorId;

	// TPKT flag.
	lstnInfo->tpkt = tpkt;

	return (lstnInfo);

} // CdSuTcp::buildListenInfo ()

//---------------------------------------------------------------------------
// Method	: buildConnIndEvent () 
// Description  : It builds the connection indication event structure. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpUiConIndEvent* CdSuNhTcp::buildConnIndEvent 
				   (CdSuNhTcpLstnInfo* lstnInfo,
				    CdSuNhTcpConnInfo* connInfo)

{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// Form Conn Key.
	CdSuNhTcpConnKey connKey;
	connKey.init (connInfo->connectionId, nhDb->getNhInstId ());
	// Form Lstn Key.
	CdSuNhTcpLstnKey lstnKey;
	lstnKey.init (lstnInfo->listenerId, nhDb->getNhInstId ());

	CdSuNhTcpUiConIndEvent* evt = new CdSuNhTcpUiConIndEvent ();
	evt->init (connKey,
		   lstnInfo->applActorId,
		   CDSU_NHTCP_UI_CON_IND,
		   lstnKey,
		   lstnInfo->tpkt,
		   connInfo->destIpv4Addr,
		   ntohs (connInfo->destPortNo));

	return (evt);

} // CdSuTcp::buildConnIndEvent ()

//---------------------------------------------------------------------------
// Method	: buildDataIndEvent () 
// Description  : It builds the data indication event structure. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpUiDataIndEvent* CdSuNhTcp::buildDataIndEvent (
			CdSuNhTcpConnInfo* info, 
			Uint16 dlen)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	// Form Conn Key.
	CdSuNhTcpConnKey connKey;
	connKey.init (info->connectionId, nhDb->getNhInstId ());
	
	CdSuNhTcpUiDataIndEvent* evt = new CdSuNhTcpUiDataIndEvent ();

	evt->init (connKey,
	           info->applActorId,
		   CDSU_NHTCP_UI_DATA_IND,
		   info->buffer,
		   dlen, info->tpkt);
	
	// Deleting tmp buffer assigned to conn info.	
	delete [] (info->buffer);
	info->buffer = NULL;

	return (evt);

} // CdSuTcp::buildDataIndEvent ()

//---------------------------------------------------------------------------
// Method	: buildCloseConIndEvent () 
// Description  : It builds the close indication event structure. 
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

CdSuNhTcpUiCloseConIndEvent* CdSuNhTcp::buildCloseConIndEvent
				(CdSuNhTcpConnInfo* info)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);
	
	// Form Conn Key.
	CdSuNhTcpConnKey connKey;
	connKey.init (info->connectionId, nhDb->getNhInstId ());

	CdSuNhTcpUiCloseConIndEvent* evt = new CdSuNhTcpUiCloseConIndEvent ();
	evt->init (connKey, 
		   info->applActorId,
		   CDSU_NHTCP_UI_CLOSE_CON_IND);

	return (evt);

} // CdSuTcp::buildCloseConIndEvent ()

//---------------------------------------------------------------------------
// Method	: incrStatsCtrs () 
// Description  : It increments the statistics counters.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void CdSuNhTcp::incrStatsCtrs (CdSuNhTcpCtrType ct)
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	nhDb->statsPkg->incrementCtr (ct);

} // CdSuNhTcp::incrStatsCtrs ()

//---------------------------------------------------------------------------
// Method	: getStatsCtrs () 
// Description  : It returns the statistics counters.
// Inputs       : 
// Return Value : 
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

Ulong* CdSuNhTcp::getStatsCtrs ()
{
	CDSU_TRACE1 (4, "%s\n", __PRETTY_FUNCTION__);

	nhDb->statsPkg->setCtr (CDSU_NHTCP_CUR_ACTIVE_CON_CTR, handle2ConnInfoTable->size ());
	nhDb->statsPkg->setCtr (CDSU_NHTCP_CUR_ACTIVE_LSN_CTR, handle2ListenInfoTable->size ());
	
	return (nhDb->statsPkg->getCounters ());
	
} // CdSuNhTcp::getStatsCtrs ()


//----------------------------------------------------------------------------
// < End of cdsunhtcp.cpp>
//----------------------------------------------------------------------------
