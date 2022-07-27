//-----------------------------------------------------------------------------
//File		: hl_timer.h
//Description   : Global functions to use H/W Timer.
//-----------------------------------------------------------------------------
#include<cyg/hal/mpc82xx_hl/hl_gptimer.h>
#include <cyg/hal/mpc82xx_hl/hl_quiccii.h>
#include <cyg/hal/quiccII/ppc82xx.h>
#include "cdsutrace.h"


//-----------------------------------------------------------------------------
//Class		:HL_Timer
//Description	:
//-----------------------------------------------------------------------------
class HL_Timer
{
public:
	HL_Timer();
	void init();
	Uint16 readCounter();
	
private:
	GPTimer *GPTimer2 ;
	unsigned short GPTPreScalerValue ;
	unsigned short GPTModeRegValue;
	Uint16 time_count;
}; //HL_Timer

//-----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
HL_Timer::HL_Timer()
{
	//Create GpTimer Handler.Pass GPTimer object, offset.
	GPTimer2 = new (GPTimer)(GPTIMER2, TMR2_OFFSET);	
}//HL_Timer()

//-----------------------------------------------------------------------------
// init().
//-----------------------------------------------------------------------------
void HL_Timer::init ()
{
	CDSU_TRACE(4, "init\n");
	GPTPreScalerValue = 0xff00;

	//Set Timer Register Mode.
	GPTModeRegValue = (GPTPreScalerValue | GPT_ResetTimer2 |
				GPT_TM_MasterClock16 | GPT_TM_Restart |
				GPT_TM_DisInt |  GPT_TM_Pulseoutput |
				GPT_TM_DisIntCE );

	//Create GpTimer Handler.Pass GPTimer object, offset.
	GPTimer2 = new (GPTimer)(GPTIMER2, TMR2_OFFSET);

	//Set Clock Frequency for 16.
	GPTimer2->setclockFrequency(GPT_TM_MasterClock16);     //0x04

	//Set Prescaler Value.
	//GPTimer2->setPrescalerValue(GPTPreScalerValue);

	//Set GPTimer Mode to Restart.
	GPTimer2->setMode(GPT_ResetTimer2);	

	//Set Timer Register Mode.
	GPTimer2->setmodeReg(GPTModeRegValue);

	//Read the mode register set.
	Uint16 readmode =  GPTimer2->readmodeReg();
	CDSU_TRACE1 (4, "Read Mode Set:%d\n",readmode);

	//Set Reference Value of Timer Reference Register.
	GPTimer2->setReferenceValue(30, TIMER_SECS);

	//Enable GPTimer.
	GPTimer2->enable();
}//init()

//----------------------------------------------------------------------------
// readCounter().
// ---------------------------------------------------------------------------
Uint16 HL_Timer::readCounter()
{
	//CDSU_TRACE(4, "ReadCounter\n");
	time_count = GPTimer2->readcounter();
	return time_count;

}//read_count()


