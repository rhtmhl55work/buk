//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename     : cdsunhudpthrd.cpp 
// Description	: 
// Ref Docs     : 
// Author       : Sumit Bijwe 
// Created On   : Mon Jul 21 18:15:10 IST 2003
// Last Modified:
// Modified By	: Vijaya Kumar
// Modified On	: 1. Mon Aug  2 17:36:21 IST 2004
//			Error Case Handling
//				- If Bind Fails
//		  2.  Wed Apr 19 19:18:43 IST 2006
//			By : Vijaya Kumar
// 			Code has been changed to port over Fedora 4
//============================================================================

//----------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------


#include <string.h>
#include "cdsumdapi.h"
#include "cdsunhudpdb.h"
#include "cdsunhudptxptsrvc.h"
#include "cdsutrace.h"


//----------------------------------------------------------------------------
// Global variables.
//----------------------------------------------------------------------------
#ifdef __ECOS
char rxThrdStkSpace[ 64 * 1024 ];
#endif


//----------------------------------------------------------------------------
// Udp Thread Locks this and waits TxptService thead to unlock this
// CdSuUdpTxptServices returns status through cdSuMdTxptSrvcThrdStatus variable
//----------------------------------------------------------------------------
sem_t cdSuMdUdpThrdCondSem;	 
bool cdSuMdTxptSrvcThrdStatus;

//----------------------------------------------------------------------------
// Message Distributor is waiting on semaphore cdSuMdCondSem.
// Once this thread does basic operation it has to unlock
// semphore cdSuMdCondSem. Before unlocking correct value for
// cdSuMdUdpThrdStatus should be set.
//----------------------------------------------------------------------------
extern sem_t cdSuMdCondSem;
bool cdSuMdUdpThrdStatus = false;

//----------------------------------------------------------------------------
// Function prototypes.
//----------------------------------------------------------------------------

void* cdsuMdUdpRxThrdStart(void*);


void* cdsuMdUdpNwHlrStart (void* iPrm)
{
	cdSuMdUdpThrdStatus = false;
	
	// CDSU_TRACE (4, "cdsuMdUdpNwHlrStart\n");

	CdSuMdUdpNhInitParam* initParam = (CdSuMdUdpNhInitParam*) iPrm; 
	
	// Create the UdpNetworkHandler DB.
	CdSuNwhlrDb* nwDb = new CdSuNwhlrDb;

	// Udate the UdpNetworkHandler DB.
	nwDb->setNetworkMsgQ (initParam->udpNhQueue);
	nwDb->setSelfIpAddr (initParam->selfIpAddr);
	nwDb->setUdpListenPort (initParam->portNo);
	nwDb->setUdpNwHlrMid (initParam->nhMid);

	CdSuTsQueue<CdMessage*>* udpNetworkMsgQ = nwDb->getUdpNtwkMesgQ ();

	CdSuUdpTxptServices* udpServ = new CdSuUdpTxptServices ();
	// CDSU_TRACE (5, "CdSuUdpTxptServices new success.\n");
	nwDb->setTxptServUdp (udpServ);

	CdSuUdpTxptServices* tx = nwDb->getTxptServUdp ();

	sem_init (&cdSuMdUdpThrdCondSem, 0, 1);
	
	//--------------------------------------------------------------------
	// Lock The Semaphore
	// Create the receiveThread.
	//--------------------------------------------------------------------
	int rv;
	do 
	{
		rv = sem_wait (&cdSuMdUdpThrdCondSem);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return NULL;
	}
	pthread_t rxThrdId;
#ifdef __ECOS

	pthread_attr_t rxAttr;
	pthread_attr_init (&rxAttr);
	pthread_attr_setstackaddr (&rxAttr, 
		(void*)&rxThrdStkSpace [sizeof (rxThrdStkSpace)]);
	pthread_attr_setstacksize (&rxAttr, sizeof (rxThrdStkSpace));
	pthread_create (&rxThrdId, &rxAttr,cdsuMdUdpRxThrdStart, nwDb);

#elif sun
	pthread_attr_t rxAttr;
	pthread_attr_init (&rxAttr);
	pthread_attr_setscope (&rxAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_create (&rxThrdId, &rxAttr,cdsuMdUdpRxThrdStart, nwDb);
#else
	pthread_create (&rxThrdId, 0, cdsuMdUdpRxThrdStart, nwDb);
#endif

	//--------------------------------------------------------------------
	// Waits for Semaphore to unlock[txptSrvc will unlock]
	//--------------------------------------------------------------------
	do 
	{
		rv = sem_wait (&cdSuMdUdpThrdCondSem);
	} while (rv == -1 && errno == EINTR);
	if (rv != 0)
	{
		CDSU_TRACE (1, "semaphore wait error\n");
		return NULL;
	}
	if (cdSuMdTxptSrvcThrdStatus == false)
	{
		delete nwDb;
		delete udpServ;
		cdSuMdUdpThrdStatus = false;
		//------------------------------------------------------------
		// Unlock the semaphore of Message Distributor and
		// udp thread[myself]
		//------------------------------------------------------------
		sem_post (&cdSuMdCondSem);
		sem_post (&cdSuMdUdpThrdCondSem);
		// Kill Myself
		pthread_exit (NULL);
	} // if (cdSuMdTxptSrvcThrdStatus ==)
	
	cdSuMdUdpThrdStatus = true; // All basic operation done successfully.
	//------------------------------------------------------------
	// Unlock the semaphore of Message Distributor and
	// udp thread[myself]
	//------------------------------------------------------------
	sem_post (&cdSuMdCondSem);
	sem_post (&cdSuMdUdpThrdCondSem);
	
	// No more this semaphore is requied. So kill it. 
	sem_destroy (&cdSuMdUdpThrdCondSem);
	
	while (1)
	{
		CdMessage* msg;
		bool retVal = udpNetworkMsgQ->popBlocking (msg);
		if (retVal != false)
		{
		// CDSU_TRACE (5, "UDP Network Hlr has received Msg from Appln.\n");
		switch (msg->msgHdr.opcode)
		{
		case CDSU_MD_NH_TX_BUF_MSG:
			{
			// CDSU_TRACE (4, "cdsuMdUdpNwHlrStart (): CDSU_MD_NH_TX_BUF_MSG rxd\n");
			CdSuMdUdpNhTxBufMsg* txmsg =
						(CdSuMdUdpNhTxBufMsg*) msg;
			tx->sendMsg (txmsg->encodedMessage, 
				txmsg->dataLen, txmsg->ipAddress, 
				txmsg->portNo);
			delete [] (txmsg->encodedMessage);
			delete (txmsg);
			}
			break;

		case CDSU_MD_DEL_UDPNH_MSG:
			{
			
			// CDSU_TRACE (3, "Exiting UDP Network Handler Thread. \n");
			// Ack Message to be sent to user of this routine.
			delete msg;
			pthread_cancel (rxThrdId);
			delete nwDb;
			udpServ->stopListener ();
			delete udpServ;
			// Message Distributor is waiting for this
			// Semaphore to be unlocked. So Unlock Before
			// dieying
			sem_post (&cdSuMdCondSem);
			pthread_exit (NULL);
			}
			break;
		
		default:
			// CDSU_TRACE (4, "cdsuMdUdpNwHlrStart (): Got a junk msg!\n");
			break;
		} //switch (msg->opcode)
		} else
		{
			// CDSU_TRACE1 (5, "%s: Got a NULL msg from the network queue!\n", __PRETTY_FUNCTION__);
		}
	}
}

//----------------------------------------------------------------------------
// Function	: cdsuMdUdpRxThrdStart
// Description	:
//----------------------------------------------------------------------------

void* cdsuMdUdpRxThrdStart(void* args)
{
	cdSuMdTxptSrvcThrdStatus = false;		
	
	// CDSU_TRACE (3, "cdsuMdUdpRxThrdStart () \n");
	
	CdSuNwhlrDb* nwDb = (CdSuNwhlrDb*) args;	
	CdSuUdpTxptServices* txUdp = nwDb->getTxptServUdp ();
	
	// CDSU_TRACE1 (5, "lpn = %d\n", nwDb->getUdpListenPort ());
	int sockFd = txUdp->setListener (nwDb->getSelfIpAddr (),
					nwDb->getUdpListenPort ());
	if (sockFd > 0)
	{
		cdSuMdTxptSrvcThrdStatus = true;		
		sem_post (&cdSuMdUdpThrdCondSem);
	} else
	{
		cdSuMdTxptSrvcThrdStatus = false;		
		sem_post (&cdSuMdUdpThrdCondSem);
		pthread_exit (NULL);
	}

	char recvBuf [CDSU_MSGDISTR_READ_BUF_LEN];
	while (1)
	{
		Uint32 ipaddr;
		int len;
		Uint16 portNo;
		bool retVal = txUdp->recvMsg (recvBuf, len, ipaddr, portNo);
		if (retVal == true)
		{
			// CDSU_TRACE (4, "UDP Network Hlr has received Msg from Peer\n");
#if 0
			for (int i = 0; i < len; i++)
			{
				printf ("recvBuf[ %d ] = %02x\n", i, recvBuf[ i ]);
			}
#endif
			CdMessage* rxdMsg = (CdMessage*) cdSuGetMsgBuf (len);
			memcpy (rxdMsg, recvBuf, len);
			rxdMsg->msgHdr.ntoh ();
			if (cdSuMdSendMsg (rxdMsg) != true)
			{
				CDSU_TRACE (1, "ReceiverThreadStart_UDP : cdSuMdSendMsg failed!\n");
				cdSuFreeMsgBuf (rxdMsg);
			}

		}
	}
}

//============================================================================
// End of cdsunhudpthrd.cpp
//============================================================================



