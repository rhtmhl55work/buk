
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename		: cdsusanity.cpp
// Description		: Implementation of the SanityModule class
// Ref Docs		: ITS-MDD-PM_PM-001
// Author		: Dipanjan Dutta
// Contributor		: 
// Created On		: 19 Jun, 2002
// Last Modified by	: Arun Kumar Swain & Anish Shankar
// Last Modified	: Tue Nov 24 17:54:57 IST 2009
//============================================================================


#include "cdsusanity.h"
// Defining DEMO2 will use TRIGGER once timer instead of Cyclic Timer
// #define DEMO2

#ifdef SM_MEMTEST
externC struct mallinfo mallinfo( void );
externC void cyg_kmem_print_stats (void);
#endif

//---------------------------------------------------------------------------
// SanityModule::SanityModule()
// Description  : Contructor of the sanity module object
// 		  It initializes almost all attributes and creates timers.
// Inputs       : const SanityParams& params
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

SanityModule::SanityModule (const SanityParams& ci, CdSuTimerHandler*
timerHlr)
{
	CDSU_TRACE1 (5,"Sanity Module for AppId[%d]\n",
	ci.src.srvcHlrId.appSrvcHlrId.appId);
	// Flag initialization.
	resetSanityFlag = false;
	estdSanityFlag = false;
	txSanityFlag = false;
	rxSanityFlag = false;

	// Init the configuration information.
	cnfgInfo = ci;

	// Init Timers and related info
	timerHandler = timerHlr;
	
	txSanityTI.timeUnits		= CDSU_TMR_TU_SECOND;
	txSanityTI.timeCounts		= ci.txTimeout;
	txSanityTI.destModuleId		= ci.fdst;
	txSanityTI.opcode		= ci.txTimerMsgOpcode;
#ifdef DEMO2
	txSanityTI.timerType		= CDSU_TMR_TT_TRIGGER_ONCE;
#else
	txSanityTI.timerType		= CDSU_TMR_TT_CYCLIC;
#endif
	txSanityTI.timeoutReportType	= CDSU_TMR_TRT_MESSAGE;
	txSanityTI.timerCallBackFn	= NULL;
	txSanityTI.userInfo [0]		= (Uint32) cnfgInfo.sanityTag;
	txSanityTI.userInfo [1]		= (Uint32) cnfgInfo.link;

	rxSanityTI.timeUnits		= CDSU_TMR_TU_SECOND;
	rxSanityTI.timeCounts		= ci.rxTimeout;
	rxSanityTI.destModuleId		= ci.fdst;
	rxSanityTI.opcode		= ci.rxTimerMsgOpcode;
	rxSanityTI.timerType		= CDSU_TMR_TT_TRIGGER_ONCE;
	rxSanityTI.timeoutReportType	= CDSU_TMR_TRT_MESSAGE;
	rxSanityTI.timerCallBackFn	= NULL;
	rxSanityTI.userInfo [0]		= (Uint32) cnfgInfo.sanityTag;
	rxSanityTI.userInfo [1]		= (Uint32) cnfgInfo.link;

	txSanityTimer = 0;
	rxSanityTimer = 0;

	// Convenience references to src, dst and faildst appIds.
	/* srcShId  = ShIdtoUint32(cnfgInfo.src.srvcHlrId);
	 dstShId  = ShIdtoUint32(cnfgInfo.dst.srvcHlrId);
	 fdstShId = ShIdtoUint32(cnfgInfo.fdst.srvcHlrId);
	 */
		
	// For message passing

#ifdef SM_MEMTEST
	count = 0;
#endif

}

//---------------------------------------------------------------------------
// SanityModule::~SanityModule()
// Description  : Destructor of the sanity module object
// 		  It frees the memory taken by timers and messages
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

SanityModule::~SanityModule ()
{
	timerHandler->stopTimer (txSanityTimer);
	timerHandler->stopTimer (rxSanityTimer);
}

//---------------------------------------------------------------------------
// SanityModule::reset()
// Description  : It resets the sanity module object.
// 		  once reset the module is inert, unless explicitly 
// 		  configured and started
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::reset (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	txSanityFlag = false;
	rxSanityFlag = false;
	resetSanityFlag = true;

	timerHandler->stopTimer (txSanityTimer);
	timerHandler->stopTimer (rxSanityTimer);
	
	missedSanityCount = 0;
}

//---------------------------------------------------------------------------
// SanityModule::setSanityParams()
// Description  : This method is used to configure the sanity module object
// 		  if it has been reset
// Inputs       : const SanityParams& params
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::setSanityParams (const SanityParams& ci)
{
		CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
		//if (resetSanityFlag == true)
		cnfgInfo = ci;
}

//---------------------------------------------------------------------------
// SanityModule::getSanityParams()
// Description  : This methods obtain the configured parametes of the sanity
// 		  module object
// Inputs       : SanityParams& param
// Return Value : SanityParams param
// Side Effects : Changes the user provided global struct "params"
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::getSanityParams (SanityParams& ci)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	ci = cnfgInfo;
}

//---------------------------------------------------------------------------
// SanityModule::startTx()
// Description  : This method starts the tranission of the sanity messages
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::startTx (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	if (txSanityFlag != true)
	{
		txSanityFlag = true;
		resetSanityFlag = false;
	
		CDSU_TRACE1 (5, "Sanity Module for AppId[%d] : Started Tx\n", 
			srcShId);
		txSanityMsg = (SanityMsg*)cdSuGetMsgBuf (sizeof (SanityMsg));
		txSanityMsg->initMsg (cnfgInfo.dst, cnfgInfo.src);
		
		if (cdSuMdSendMsg (txSanityMsg) == false)
		{
			CDSU_TRACE (1, "SanityModule:startTx:cdSuMdSendMsg failed!\n");
			cdSuFreeMsgBuf (txSanityMsg);
		}
		
		timerHandler->startTimer (txSanityTimer, txSanityTI);
	}
}// SanityModule::startTx () 

//---------------------------------------------------------------------------
// SanityModule::stopTx()
// Description  : This method stops the transmission of the sanity messages
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::stopTx (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	if (txSanityFlag != false)
	{
		CDSU_TRACE1 (5, "Sanity Module for AppId[%d] : Stopped Tx\n", 
			srcShId);
		timerHandler->stopTimer (txSanityTimer);
		txSanityFlag = false;
	}
}

//---------------------------------------------------------------------------
// SanityModule::startRx()
// Description  : This method starts the reception of the sanity messages
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::startRx (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	if (rxSanityFlag != true)
	{
		CDSU_TRACE1 (5, "Sanity Module for AppId[%d] : Started Rx\n", 
			srcShId);
		rxSanityFlag = true;
		resetSanityFlag = false;
		timerHandler->startTimer (rxSanityTimer, rxSanityTI);
		CDSU_TRACE1 (5, "Started rx sanity timer [ %d ]\n", rxSanityTimer);
	}
}

//---------------------------------------------------------------------------
// SanityModule::stopRx()
// Description  : This method stops the reception of the sanity messages
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::stopRx (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	if (rxSanityFlag != false)
	{
		CDSU_TRACE1 (5, "Sanity Module for AppId[%d] : Stopped Rx\n", 
			srcShId);
		timerHandler->stopTimer (rxSanityTimer);
		rxSanityFlag = false;
		missedSanityCount = 0;
	}
}

//---------------------------------------------------------------------------
// SanityModule::getMissedSanityCount()
// Description  : This method is used to obtain current statistics of 
// 		  consecutive sanity misses.
// Inputs       : None
// Return Value : No of consecutively missed sanities
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

Uint8 SanityModule::getMissedSanityCount (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	return missedSanityCount;
}

//---------------------------------------------------------------------------
// SanityModule::handleMessage()
// Description  : The method to handle incoming messages. The incoming
// 		  messages can be either the sanity messages or the timeout
// 		  (timer expiry) messages
// Inputs       : CdMessage*
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::handleMessage (CdMessage* msg)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	Uint16 opc;

	opc = msg->msgHdr.opcode;

	if ( opc == cnfgInfo.rxSanityMsgOpcode)
		sanityHlr ();
	else if ( opc == cnfgInfo.timeoutOpcode )
		timeoutHlr (msg);

}	

//---------------------------------------------------------------------------
// SanityModule::sanityHlr()
// Description  : The method to handle a received sanity message
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::sanityHlr (void)
{
	//Remove Latter
	// CDSU_TRACE(1,"SanityModule::sanityHlr:Sanity Message Received\n");
	

	if (rxSanityFlag == true)
	{
		if (estdSanityFlag == false)
		{
			
			// Commented by Arvind
			/*CDSU_TRACE2 (3, 
			"SANITY: [%s] : Established Sanity with [%s]\n", 
			smGetPlfModuleName(cnfgInfo.src.srvcHlrId),
			smGetPlfModuleName(cnfgInfo.dst.srvcHlrId));
			*/

			estdSanityFlag = true;
		}
		
		// Re-start the Rx Timer.
		CdSuTimerId tmpTimer = rxSanityTimer;
		timerHandler->stopTimer (rxSanityTimer);
		timerHandler->startTimer (rxSanityTimer, rxSanityTI);
		missedSanityCount = 0;
		
		CDSU_TRACE1 (5, "Stopped rx sanity timer [ %d ]\n",
				tmpTimer);
		CDSU_TRACE1 (5, "Started rx sanity timer [ %d ]\n",
				rxSanityTimer);
		//Commented by Arvind
		//	CDSU_TRACE2 (5, "SANITY: [%s] : Rxd Sanity from [%s]\n", 
		// 	smGetPlfModuleName(cnfgInfo.src.srvcHlrId),
		 //	smGetPlfModuleName(cnfgInfo.dst.srvcHlrId));
	}
}

//---------------------------------------------------------------------------
// SanityModule::timeoutHlr(CdMessage* msg)
// Description  : The method to handle a transmit-timer expiry
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::timeoutHlr (CdMessage* msg)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	SanityTimeoutMsg* tom = (SanityTimeoutMsg*)msg;

	if ( tom->opcode == cnfgInfo.rxTimerMsgOpcode) 
	{
		CDSU_TRACE1 (4, "SANITY: Rxd Rx TimeOut [ %d ]\n",
				(Uint32) tom->timerId);
		rxTimeoutHlr ();
	}
	else if ( tom->opcode == cnfgInfo.txTimerMsgOpcode )  
	{
		CDSU_TRACE (4, "SANITY: Rxd Tx TimeOut\n");
		txTimeoutHlr ();
	}
}

//---------------------------------------------------------------------------
// SanityModule::txTimeoutHlr()
// Description  : The method to handle a transmit-timer expiry
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::txTimeoutHlr (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	if (txSanityFlag == true)
	{
#ifdef DEMO2
		timerHandler->stopTimer (txSanityTimer);
		timerHandler->startTimer (txSanityTimer, txSanityTI);
#endif
		// Commented by Arvind
		//CDSU_TRACE2 (5, "SANITY: [%s] : Txd Sanity to [%s]\n", 
		//	smGetPlfModuleName(cnfgInfo.src.srvcHlrId), 
		//	smGetPlfModuleName(cnfgInfo.dst.srvcHlrId));
		
		//TODO:Remove Later
		// CDSU_TRACE (1, "Sending Sanity Message \n"); 
		txSanityMsg = (SanityMsg*)cdSuGetMsgBuf (sizeof (SanityMsg));
		txSanityMsg->initMsg (cnfgInfo.dst, cnfgInfo.src);

		if (cdSuMdSendMsg (txSanityMsg) == false)
		{
			CDSU_TRACE (1, "SanityModule::txTimeoutHlr:cdSuMdSendMsg Failed !\n");
			cdSuFreeMsgBuf (txSanityMsg);
		}
		
		
#ifdef SM_MEMTEST
		struct mallinfo meminfo;
		++count;
		if (count == 20)
		{
			count = 0;
			// To print the malloc's memory status
			//meminfo = mallinfo ();
			//printf ("The total size of memory allocated = %d\n", meminfo.arena);
			//printf ("The amount of memory occupied by chunks handed over by malloc blocks = %d\n", meminfo.uordblks);
			//printf ("The amount of memory occupied by free chunks blocks = %d\n", meminfo.fordblks);
			//printf ("The amount of free memory blocks = %d\n", meminfo.maxfree);
			printf ("MEMSTAT: Total: malloc()== %d, free = %d\n", mallocCount, freeCount);  
			
			// To print the network memory status
			//cyg_kmem_print_stats ();
		}
#endif
	}
	else
	{
		CDSU_TRACE (1, "SANITY: Not Ready to Tx !\n");
	}
}

//---------------------------------------------------------------------------
// SanityModule::rxTimeoutHlr()
// Description  : The method to handle a receive-timer expiry
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------

void SanityModule::rxTimeoutHlr (void)
{
	CDSU_TRACE1 (5, "%s\n", __PRETTY_FUNCTION__);
	if (rxSanityFlag == true)
	{
		++missedSanityCount;
		if (missedSanityCount >= cnfgInfo.sanityMissThreshold)
		{
			
			if ( estdSanityFlag != false )
			{
			
			//Commented by Arvind	
			/*	CDSU_TRACE2 (3, 
				"SANITY: [%s] : LOST Sanity with [%s]\n", 
				smGetPlfModuleName(cnfgInfo.src.srvcHlrId), 
				smGetPlfModuleName(cnfgInfo.dst.srvcHlrId));
			*/	
				estdSanityFlag = false;
			}
			
			// Successive missing of sanity: Send sanity failure
			// msg sent to "fdst"
			SanityFailureMsg* failureMsg = (SanityFailureMsg*)
				cdSuGetMsgBuf (sizeof (SanityFailureMsg)); 
			failureMsg->initMsg (cnfgInfo.fdst, cnfgInfo.src, 
							cnfgInfo.sanityTag);
			
			if (cdSuMdSendMsg (failureMsg) == false)
			{
				CDSU_TRACE (1, "SanityModule:startTx:cdSuMdSendMsg failed!\n");
				cdSuFreeMsgBuf (failureMsg);
			}
		}
		// Just in case, stop any running timer !
		timerHandler->stopTimer (rxSanityTimer);
		timerHandler->startTimer (rxSanityTimer, rxSanityTI);
		CDSU_TRACE (5, "Started rx sanity timer\n");
	}
}

//---------------------------------------------------------------------------
// SanityModule::print()
// Description  : 
// Inputs       : None
// Return Value : None
// Side Effects : 
// Algorithm    :
//---------------------------------------------------------------------------
void SanityModule::print ()
{
	CDSU_TRACE  (1, "========================================\n");
	CDSU_TRACE1 (1, "sanityTag == %d\n", cnfgInfo.sanityTag);
	CDSU_TRACE1 (1, "link == %d\n", cnfgInfo.link);
	CDSU_TRACE1 (1, "sanityMissThreshold == %d\n", cnfgInfo.sanityMissThreshold);
	CDSU_TRACE1 (1, "txSanityMsgOpcode == %d\n", cnfgInfo.txSanityMsgOpcode);
	CDSU_TRACE1 (1, "rxSanityMsgOpcode == %d\n", cnfgInfo.rxSanityMsgOpcode);
	CDSU_TRACE1 (1, "sanityFailureMsgOpcode == %d\n", cnfgInfo.sanityFailureMsgOpcode);
	CDSU_TRACE1 (1, "txTimerMsgOpcode == %d\n", cnfgInfo.txTimerMsgOpcode);
	CDSU_TRACE1 (1, "rxTimerMsgOpcode == %d\n", cnfgInfo.rxTimerMsgOpcode);
	CDSU_TRACE1 (1, "rxTimeout == %d\n", cnfgInfo.rxTimeout);
	CDSU_TRACE  (1, "----------------------------------------\n");
	CDSU_TRACE1 (1, "txSanityFlag == %d\n", txSanityFlag);
	CDSU_TRACE1 (1, "rxSanityFlag == %d\n", rxSanityFlag);
	CDSU_TRACE1 (1, "resetSanityFlag == %d\n", resetSanityFlag);
	CDSU_TRACE1 (1, "missedSanityCount == %d\n", missedSanityCount);
	CDSU_TRACE  (1, "========================================\n");
}

//============================================================================
// <end of filename.cpp>
//============================================================================
