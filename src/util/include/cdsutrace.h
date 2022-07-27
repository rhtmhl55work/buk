#ifndef CDSU_TRACE_H_
#define CDSU_TRACE_H_
//=============================================================================
// Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename     : cdsutrace.h 
// Description  : This file contains declaration of the CdSuTrace Class.
// Ref Docs     : 
// Author       : Seema Chandak 
// Created On   : 
// Last Modified: Wed Aug  6 16:32:34 IST 2003
//============================================================================

class CdSuTrace
{
public:
	static void setLevel (unsigned level);
	static bool canTrace (unsigned level);
	static void print (const char* fname, const unsigned lineno, unsigned level, char* fmt, ...);

private:
	static unsigned traceLevel;
};

#ifdef CDSU_TRACING

#define CDSU_TRACE(level,a)   CdSuTrace::print (__FILE__, __LINE__, level, a)
#define CDSU_TRACE1(level,a,b)     CdSuTrace::print (__FILE__, __LINE__, level, a,b)
#define CDSU_TRACE2(level,a,b,c)   CdSuTrace::print (__FILE__, __LINE__, level, a,b,c)
#define CDSU_TRACE3(level,a,b,c,d)   CdSuTrace::print (__FILE__, __LINE__, level, a,b,c,d)
#define CDSU_TRACE4(level,a,b,c,d,e) CdSuTrace::print (__FILE__, __LINE__, level, a,b,c,d,e)
#define CDSU_TRACE5(level,a,b,c,d,e,f) CdSuTrace::print (__FILE__, __LINE__, level, a,b,c,d,e,f)

#else

#define CDSU_TRACE(level,a)
#define CDSU_TRACE1(level,a,b)
#define CDSU_TRACE2(level,a,b,c)
#define CDSU_TRACE3(level,a,b,c,d)
#define CDSU_TRACE4(level,a,b,c,d,e)
#define CDSU_TRACE5(level,a,b,c,d,e,f)

#endif

//============================================================================
// <end of cdsutrace.h>
//============================================================================
#endif
