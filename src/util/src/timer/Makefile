#=============================================================================
# Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : Makefile
# Description  : Makefile for Misc Util Source Directory.
# Author       : Sumit Bijwe
# Created On   : Wed Feb 25 15:27:12 IST 2004
# Last Modified:
#=============================================================================

include ../../makefile.inc

CDSU_UTIL_TIMER_SRC_FILES = cdsukeyallocator.cpp \
		      cdsutimingwheel.cpp \
		      cdsutimer.cpp 

CDSU_UTIL_TIMER_OBJ_FILES = $(CDSU_UTIL_TIMER_SRC_FILES:.cpp=.o)


CDSU_UTIL_TIMER_LIB=libcdsutimer.a

all: $(CDSU_UTIL_TIMER_OBJ_FILES)

clean:
	-rm -f $(CDSU_UTIL_TIMER_LIB) $(CDSU_UTIL_TIMER_OBJ_FILES) core *.tmp *.map

#=============================================================================
#	<end of Makefile>
#=============================================================================
