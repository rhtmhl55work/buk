//---------------------------------------------------------------------------
// Internet Telephony Software - (c) Copyright 2003 by ITS, C-DOT, Bangalore,
// India.
// Filename     : tdmInit.h
// Description  : provides initialization of the Time Switch and E1 framer.
// Ref Docs     : MPC8260 PowerQUICC II User's Manual
// Author       : R. Raja simha
// Created On   : 2003-05-06
// Last Modified: 2002-05-07
//----------------------------------------------------------

// *************************************************************
// include files
// *************************************************************
#ifdef __ECOS

#include <cyg/infra/cyg_type.h>
#include <pkgconf/system.h>
#include <cyg/hal/hl_e1framer.h>
#include <cyg/hal/hl_tsu.h>


// **************************************************************
// GSC Control Register2 address is hard coded here
// TODO: to be properly placed
// **************************************************************

#if defined (CYGPKG_HAL_POWERPC_GSC)
#define GSC_CR2 0x40000008
#endif 

#if defined (CYGPKG_HAL_POWERPC_GII)
#define GII_CR2 0x40000008
#endif 

// Enums required for the switching

#define	TIF_E1_LINK_NO  0
#define	TIF_E1_SLOT_SIG  16
#define	TIF_E1_SLOT_VOICE  1

#define	TIF_MCC_CHAN_SIG  64
#define	TIF_MCC_CHAN_VOICE  4

#define	TIF_H110_TX_CHAN  1
#define	TIF_H110_TX_LINK  2

#define	TIF_H110_RX_CHAN  1
#define	TIF_H110_RX_LINK  3


//---------------------------------------------------------------------------
// Class declaration for the TdmInterface
//---------------------------------------------------------------------------

class TdmInterface {

public:

	enum TdmIntfType {
		TIF_SIGNALING,
		TIF_VOICE,
		TIF_VOICE_ONLY
	};

	TdmInterface();
	~TdmInterface();

	void resetTimeSwitch ();
	void programTimeSwitch (TdmIntfType);

	void setE1InNormalMode(Uint16);
	void setE1InExternalLoopbackMode(Uint16);
	void enableE1(Uint16);
	void enableCcs (Uint16);
	void enableCas (Uint16);

private:
	HL_E1Interface *pE1Framer;
	HL_Tsu *pTsu;
};

#endif
//============================================================================
// <end of tdmInit.h>
//============================================================================

