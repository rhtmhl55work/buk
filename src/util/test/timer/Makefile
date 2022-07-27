#=============================================================================
# Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	: Makefile
# Description	: Makefile for the test directory.
# Author	: Shiv Kumar B.
# Created On	: November 11th, 2002 
# Last Modified:  November 11th, 2002
#=============================================================================

include ../../makefile.inc

LIBS=$(CDSU_UTIL_SRC_DIR)/libcdsuutil.a

TESTSRC = timerFuncTest.cpp \
	timerListTest.cpp \
	test-listclient.cpp \
	timerCascade.cpp\
	test-funcclient.cpp \
	sup-test.cpp \
	  

TESTOBJS = $(TESTSRC:.cpp=.o)

RTP_TESTS = timerCascade.out  timerListTest.out  timerFuncTest.out funcClient.out listClient.out
	
all: $(RTP_TESTS)

timerCascade.out: $(TESTOBJS)
	$(LD) $(LDFLAGS) timerCascade.o sup-test.o $(LIBS) -o $@ $(LDOPTS)

timerListTest.out: $(TESTOBJS)
	$(LD) $(LDFLAGS) timerListTest.o sup-test.o $(LIBS) -o $@ $(LDOPTS)

timerFuncTest.out: $(TESTOBJS)
	$(LD) $(LDFLAGS) timerFuncTest.o sup-test.o $(LIBS) -o $@ $(LDOPTS)

funcClient.out: $(TESTOBJS)
	$(LD) $(LDFLAGS) test-funcclient.o $(LIBS) -o $@ $(LDOPTS)

listClient.out: $(TESTOBJS)
	$(LD) $(LDFLAGS) test-listclient.o $(LIBS) -o $@ $(LDOPTS)
	
clean:
	-rm -f *.o  timerCascade.out timerListTest.out timerFuncTest.out funcClient.out listClient.out 

#=============================================================================
#	<end of Makefile>
#=============================================================================
