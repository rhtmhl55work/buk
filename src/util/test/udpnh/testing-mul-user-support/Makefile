#=============================================================================
# Internet Telephony Software - (c) Copyright 2004, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : Makefile
# Description  : makefile for tester  
# Author       : Vijaya Kumar
# Created On   : Tue Apr 9 15:59:37 GMT 2004
# Last Modified:
#===========================================================================

include ../makefile.inc

# source and object files
TEST_SRC_FILES = tester.cpp 

TEST_SRC_OBJS = $(TEST_SRC_FILES:.cpp=.o)

ifndef CLIENT
all : tester
else
CCFLAGS += -DCLIENT
#all : testclient
endif
tester: $(TEST_SRC_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(TEST_SRC_OBJS) $(LDOPTS)

# testclient: $(TEST_SRC_OBJS)
#	$(LD) $(LDFLAGS) -o $@ $(TEST_SRC_OBJS) $(LDOPTS)
#	rm -f *.o

clean: 
	-rm -f *.o *.tmp *.map tester testclient  *.ab* *.bdx core

#============================================================================
# <End of Makefile>
#============================================================================
