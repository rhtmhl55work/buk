#ifndef __CDSU_NHTCP_INSTMGR_
#define __CDSU_NHTCP_INSTMGR_

//----------------------------------------------------------------------------
// Include files.
//----------------------------------------------------------------------------

#include <pthread.h>

#include "cdsunhtcptypes.h"
#include "cdsunhtcpdb.h"

//----------------------------------------------------------------------------
// Class	: CdSuNhTcpInstMgr
// Description	: It is the Managing entity of all the TCP Network Handlers
// 		  in the system. It has pointers to the data base of all the
// 		  TCP Network Handlers. It is a Singleton object.
//----------------------------------------------------------------------------

class CdSuNhTcpInstMgr
{
public:
	~CdSuNhTcpInstMgr ();

	CdSuNhTcpInstId addNhTcpInst (const CdSuNhTcpMiInitParam&);
	bool removeNhTcpInst (const CdSuNhTcpInstId);

	static CdSuNhTcpInstMgr* createNhTcpInstMgr ();

	CdSuNhTcpDb* getNhTcpDb (CdSuNhTcpInstId nhInstId) 
	{
		return (nhTcpDbTable [nhInstId]);
	}

	Uint32 getNhInstCounter ()
	{
		return (noOfNhInst);
	}

private:

	//--------------------------------------------------------------------
	// Private methods.
	//--------------------------------------------------------------------
	CdSuNhTcpInstMgr ();

	int allocateNhInstId ();

	//--------------------------------------------------------------------
	// Private attributes
	//--------------------------------------------------------------------
	static CdSuNhTcpInstMgr* nhTcpInstMgr;

	pthread_mutex_t dbLock;

	
	CdSuNhTcpDb* nhTcpDbTable[ CDSU_NHTCP_MAX_INST ];
	// Presently max no of Nh instances is not a configurable parameter.
	Uint32 maxNhInst;
	Uint32 noOfNhInst;

}; // class CdSuNhTcpInstMgr

//----------------------------------------------------------------------------
// < End of file>
//----------------------------------------------------------------------------
#endif
