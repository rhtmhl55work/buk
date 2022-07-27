#=============================================================================
# Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : Makefile
# Description  : makefile for testclient and testserver 
# Author       : Titty Thomas
# Created On   : Wed Jan 29 17:15:29 IST 2003
# Last Modified:
#============================================================================

include ../makefile.inc

# source and object files
TEST_SERVER_FILES = testserver.cpp \
	testfunctions.cpp \
	testmenu.cpp \
	timerdef.cpp 

TEST_CLIENT_FILES = testclient.cpp \
	testfunctions.cpp \
	testmenu.cpp

TEST_SERVER_OBJS = $(TEST_SERVER_FILES:.cpp=.o)
TEST_CLIENT_OBJS = $(TEST_CLIENT_FILES:.cpp=.o)

# rules

testserver: $(TEST_SERVER_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(TEST_SERVER_OBJS) $(LDOPTS)

testclient: $(TEST_CLIENT_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(TEST_CLIENT_OBJS) $(LDOPTS)

clean: 
	-rm -f *.o *.tmp *.map testserver testclient *.ab* *.bdx core

#============================================================================
# <End of Makefile>
#============================================================================
