//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
//
// Filename		: cdsu_counter.cpp
// Description		: Used to provide H/W timers of CPM - MPC8260.
// Ref Docs		: MPC8260 Document. 
// Author		: Indu Mishra
// Created On		: 30/05/03
// Last Modified	:
// Last Modified On  	:
//============================================================================

//----------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------
#include "cdsutrace.h"
#include <string.h>
#include <stdio.h>

#ifdef __ECOS

#include<cyg/hal/mpc82xx_hl/hl_gptimer.h>
#include <cyg/hal/mpc82xx_hl/hl_quiccii.h>
#include <cyg/hal/quiccII/ppc82xx.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>

#define printf diag_printf
#endif

//-----------------------------------------------------------------------------
//Class		:CdSuCounter
//Description	:Singleton Pattern.
//-----------------------------------------------------------------------------
class CdSuCounter
{
public:
	static CdSuCounter* Instance();
	Uint32 readCounter(void);
	
protected:
	CdSuCounter();

private:
	static CdSuCounter* _instance;

	// CPM of MPC8260 consists of 4 Timers out of which GPTimer1 is being
	// used by System.
	GPTimer *GPTimer3;
	GPTimer *GPTimer4;
	
	unsigned short GPTPreScalerValue ;
	unsigned short GPTModeRegValue;
	
	//Private function.
	void init();
	
}; //CdSuCounter

//--------------------------------------------------------------------------
// Function Name  : Instance()
// Description    : 
// -------------------------------------------------------------------------
CdSuCounter* CdSuCounter::Instance()
{
	if(_instance == 0)
	{
		_instance = new CdSuCounter;
	}
	return _instance;
}//Instance()

CdSuCounter* CdSuCounter::_instance = 0;

//-----------------------------------------------------------------------------
// Function Name : CdSuCounter(GPtimerId, GPtimerId).
// Description   : Constructor for creating Cascaded Timers.
// ----------------------------------------------------------------------------
CdSuCounter::CdSuCounter()
{
		GPTimer3 = new GPTimer(GPTIMER3, TMR3_OFFSET);
		GPTimer4 = new GPTimer(GPTIMER4, TMR4_OFFSET);
		init();
	
}//CdSuCounter()

//-----------------------------------------------------------------------------
// Function name   : init().
// Description	   : Cascaded timers
//-----------------------------------------------------------------------------
void CdSuCounter::init ()
{
	CDSU_TRACE(4, "Cascaded init\n");

	//Set Timer Register Mode.
	GPTPreScalerValue = 0xff00;
	GPTModeRegValue = (GPTPreScalerValue | GPT_TM_DisIntCE |
			GPT_TM_Pulseoutput | GPT_TM_DisInt | GPT_TM_Restart | 
			GPT_TM_MasterClock16 | GPT_TM_GateIgnore);
	
	GPTimer4->setmodeReg(GPTModeRegValue);	//set TMR for timer4.
	
	GPTModeRegValue = (GPTPreScalerValue | GPT_TM_DisIntCE |
			GPT_TM_Pulseoutput | GPT_TM_DisInt | GPT_TM_Restart | 
			GPT_TM_Inal_Cascade | GPT_TM_GateIgnore);
	
	GPTimer3->setmodeReg(GPTModeRegValue);  //set TMR for timer3

	//Set GPTimer Mode to  a Cascaded timer.
	GPTimer4->setMode(GPT_Cascaded);
	GPTimer3->setMode(GPT_Cascaded);

	//Set Reference Value of Timer Reference Register.
	GPTimer4->setReferenceValue(0xffff, TIMER_NOUNITS);
	
	GPTimer4->reset();		// reset timer4.
	GPTimer4->enable();		// enable timer4.
	
	GPTimer3->reset();		// reset timer3.
	GPTimer3->enable();		// enable timer3.
	
}//init()

//----------------------------------------------------------------------------
// Function	: ReadCounter().
// Description  : To read Counter values.
// ---------------------------------------------------------------------------
Uint32 CdSuCounter::readCounter()
{
	Uint16 counter[2];

	// FIXME: The counter should be read in a 32 bit cycle. However, the HAL
	// does not provide us the facility, hence we are doing it in a 16 bit
	// cycle.


	// Read the counter of lesser significance first. This is to ensure that
	// the timer that changes faster is read first.
	counter[1] = GPTimer4->readcounter();

	counter[0] = GPTimer3->readcounter();

	return(*(Uint32 *)counter);
}//readcounter()

