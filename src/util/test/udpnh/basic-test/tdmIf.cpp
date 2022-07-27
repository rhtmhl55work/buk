//---------------------------------------------------------------------------
// Internet Telephony Software - (c) Copyright 2003 by ITS, C-DOT, Bangalore,
// India.
// Filename     : tdmInit.cpp
// Description  : provides initialization of the Time Switch and E1 framer.
// Ref Docs     : MPC8260 PowerQUICC II User's Manual
// Author       : R. Raja simha
// Created On   : 2003-05-06
// Last Modified: 2002-05-07
//----------------------------------------------------------

// *************************************************************
// include files
// *************************************************************
#include "tdmIf.h"

//--------------------------------------------------------------------------
// Function Name : TdmInterface()
// Purpose       : constructor
// Input Para    : none
// Return Value  : none
// Side Effects  : none
// Algorithm     : 
// Notes	 : none
//--------------------------------------------------------------------------
TdmInterface::TdmInterface ()
{
	pE1Framer =  new HL_E1Interface ((Uint32) 0x42000000);
	pTsu =  new HL_Tsu ((Uint32) 0x41000000);
}


//--------------------------------------------------------------------------
// Function Name : ~TdmInterface()
// Purpose       : destructor
// Input Para    : none
// Return Value  : none
// Side Effects  : none
// Algorithm     : 
// Notes	 : none
//--------------------------------------------------------------------------
TdmInterface::~TdmInterface ()
{
}


//--------------------------------------------------------------------------
// Function Name : resetTimeSwitch()
// Purpose       : to get pointer to E1 Framer memory
// Input Para    : none
// Return Value  : none
// Side Effects  : the DT control register will be modified
// Algorithm     : 
// Notes	 : e1 number ranges from 0 to 15
//--------------------------------------------------------------------------
void TdmInterface::resetTimeSwitch ()
{

//		Uint16 e1Number = 0;
		Uint32 *pGiiCr2;
//		Uint16 *pModeImaskReg;

		pGiiCr2 = (Uint32*)GII_CR2;
//		pModeImaskReg = (Uint16*) GII_TS_IMASK_REG;

		// Enable the child card interrupt
		*pGiiCr2 = (*pGiiCr2) | 0x00040000;

		// Set the clock mode to manual
		*pGiiCr2 = (*pGiiCr2) | 0x00000800;

		// Set the clock source to copy 0.
		*pGiiCr2 = (*pGiiCr2) & ~0x00000400;

		// Both reference clk are selected.
		*pGiiCr2 = (*pGiiCr2) & ~0x00000008;
		*pGiiCr2 = (*pGiiCr2) & ~0x00000004;

		// Give a reset to the Time Switch, 1-0-1 sequence
		*pGiiCr2 = (*pGiiCr2) | 0x00002000;
		TS_ACCESS_DELAY;
		*pGiiCr2 = (*pGiiCr2) & ~0x00002000;
		TS_ACCESS_DELAY;
		*pGiiCr2 = (*pGiiCr2) | 0x00002000;
		TS_ACCESS_DELAY;

		pTsu->init ();
		pTsu->inhibitH110SideTs ();
		pTsu->inhibitLocalSideTs ();	
//		TS_ACCESS_DELAY;
//		*pModeImaskReg = 0x00005000;

		// Remove the loopback from the child card
//		setE1InNormalMode (e1Number);
//		setE1InExternalLoopbackMode (e1Number);
//		enableCcs (e1Number);
		//enableCas (e1Number);
}

//--------------------------------------------------------------------------
// Function Name : programTimeSwitch()
// Purpose       : to get pointer to E1 Framer memory
// Input Para    : none
// Return Value  : none
// Side Effects  : the DT control register will be modified
// Algorithm     : 
// Notes	 : e1 number ranges from 0 to 15
//--------------------------------------------------------------------------
void TdmInterface::programTimeSwitch (TdmIntfType type)
{


   	pTsu->inhibitH110SideTs ();
	pTsu->inhibitLocalSideTs ();

	switch (type)
	{
	case TIF_SIGNALING:

	// Connect the E1 slot to the MCC.
	pTsu->connect (HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_SIG*4, 
		HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_SIG);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_SIG*4, 
		HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_SIG);

	// Connect the other way round
	pTsu->connect (HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_SIG, 
		HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_SIG*4);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_SIG, 
		HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_SIG*4);
	
	// Connect the Voice 
	// E1 link-0 Ts-1 ---->H.110  link-1 ts-1
	pTsu->connect (HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4, 
		     HL_Tsu::HLTSU_LT_H110, TIF_H110_RX_LINK, TIF_H110_RX_CHAN);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4, 
		     HL_Tsu::HLTSU_LT_H110, TIF_H110_RX_LINK, TIF_H110_RX_CHAN);

	// H.110 link-0 ts-1 -->MAX E1 link-0 ts-1
	pTsu->connect (HL_Tsu::HLTSU_LT_H110, TIF_H110_TX_LINK, TIF_H110_TX_CHAN,
		     HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_H110, TIF_H110_TX_LINK, TIF_H110_TX_CHAN,
		     HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4);

	break;
	

	case TIF_VOICE:

	// MCC to  Ts-1 ---->H.110  link-0 ts-1
	pTsu->connect (HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE, 
		     HL_Tsu::HLTSU_LT_H110, TIF_H110_TX_LINK, TIF_H110_TX_CHAN);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE, 
		     HL_Tsu::HLTSU_LT_H110, TIF_H110_TX_LINK, TIF_H110_TX_CHAN);

	// H.110 link-0 ts-1 -->  MCC
	pTsu->connect (HL_Tsu::HLTSU_LT_H110, TIF_H110_RX_LINK, TIF_H110_RX_CHAN,
		     HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_H110, TIF_H110_RX_LINK, TIF_H110_RX_CHAN,
		     HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE);

	break;


	case TIF_VOICE_ONLY:

	// Connect the E1 slot to the MCC.
	pTsu->connect (HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4, 
		HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE);

	pTsu->enableConn (HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4, 
		HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE);

	// Connect the other way round
	pTsu->connect (HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE, 
		HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4);
	
	pTsu->enableConn (HL_Tsu::HLTSU_LT_LOCAL, 1, TIF_MCC_CHAN_VOICE, 
		HL_Tsu::HLTSU_LT_LOCAL, 0, TIF_E1_SLOT_VOICE*4);

	break;

	default:
	break;

	}
	pTsu->setH110SideNrmlMode ();
	pTsu->setLocalSideNrmlMode();

	
}


//-------------------------------------------------
// E1 card related methods
// ------------------------------------------------
//--------------------------------------------------------------------------
// Function Name : setE1InNormalMode()
// Purpose       : 
// Input Para    : none
// Return Value  : none
// Side Effects  : DTCR will be modified
// Algorithm     : 
//--------------------------------------------------------------------------
void TdmInterface::setE1InNormalMode (Uint16 e1)
{

	// set e1 in normal mode
	pE1Framer->resetFrontEndLoopback (e1);
	pE1Framer->resetRemoteLoopback (e1);
	
	// enable the e1
	pE1Framer->enableDt (e1); 

	// reset the DT
	pE1Framer->resetDt (e1);
	for(int i=0; i< 1000; i++);
	pE1Framer->removeResetOnDt (e1);
} 

//--------------------------------------------------------------------------
// Function Name : setE1InExternalLoopbackMode()
// Purpose       : to set external loopback on e1
// Input Para    : e1 number
// Return Value  : none
// Side Effects  : the loopback bit in E1 framer memory will be set
// Algorithm     : 
// Notes	 : e1 number ranges from 0 to 15
//--------------------------------------------------------------------------
void TdmInterface::setE1InExternalLoopbackMode (Uint16 e1)
{
	// set e1 in normal mode
	pE1Framer->setFrontEndLoopback (e1);
	pE1Framer->resetRemoteLoopback (e1);
	
	// enable the e1
	pE1Framer->enableDt (e1); 
	
	// reset the DT
	pE1Framer->resetDt (e1);
	for(int i=0; i< 1000; i++);
	pE1Framer->removeResetOnDt (e1);
} 


//--------------------------------------------------------------------------
// Function Name : enableE1()
// Purpose       : to enable an E1
// Input Para    : e1 number
// Return Value  : none
// Side Effects  : the DT control register will be modified
// Algorithm     : 
// Notes	 : e1 number ranges from 0 to 15
//--------------------------------------------------------------------------
void TdmInterface::enableE1 (Uint16 e1)
{
	// enable the e1
	pE1Framer->enableDt (e1); 

	// reset the DT
	pE1Framer->resetDt (e1);
	for(int i=0; i< 1000; i++);
	pE1Framer->removeResetOnDt (e1);
} 

//--------------------------------------------------------------------------
// Function Name : enableCcs ()
// Purpose       : to enable CCS an E1
// Input Para    : e1 number
// Return Value  : none
// Side Effects  : the DT control register will be modified
// Algorithm     : 
// Notes	 : e1 number ranges from 0 to 15
//--------------------------------------------------------------------------
void TdmInterface::enableCcs (Uint16 e1)
{
	// set Tx CCS the e1
	pE1Framer->setTxCcsMode (e1); 

	// set Tx CCS the e1
	pE1Framer->setRxCcsMode (e1); 

	// reset the DT
	pE1Framer->resetDt (e1);
	for(int i=0; i< 1000; i++);
	pE1Framer->removeResetOnDt (e1);
} 

//--------------------------------------------------------------------------
// Function Name : enableCas ()
// Purpose       : to enable CCS an E1
// Input Para    : e1 number
// Return Value  : none
// Side Effects  : the DT control register will be modified
// Algorithm     : 
// Notes	 : e1 number ranges from 0 to 15
//--------------------------------------------------------------------------
void TdmInterface::enableCas (Uint16 e1)
{
	// set Tx CCS the e1
	pE1Framer->setTxCasMode (e1); 

	// set Tx CCS the e1
	pE1Framer->setRxCasMode (e1); 

	// reset the DT
	pE1Framer->resetDt (e1);
	for(int i=0; i< 1000; i++);
	pE1Framer->removeResetOnDt (e1);
} 

//============================================================================
// <end of tdmInit.cpp>
//============================================================================

