#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	    : Makefile
# Description	: Makefile for basic utility kit assignment subdir trainee
# Author	    : Trainee 2013
# Created On	: 12.11.2013
# Last Modified :
#=============================================================================

include ../../../makefile.inc

LIBS = $(CDSU_UTIL_LIB_PATH)/libcdsuutil.a

SRC_FILES = bukMsgdistrTrainee.cpp \
	bukTrainee.cpp

BINARIES = bukMsgdistrTrainee.out

all: $(BINARIES)

bukMsgdistrTrainee.out:bukMsgdistrTrainee.o bukTrainee.o
	$(LD) $(LDFLAGS) bukMsgdistrTrainee.o bukTrainee.o -l$(BUK_LIB_NAME) -l$(BUK_BASICINFRA_LIB) $(LIBS) -o $@ $(LDOPTS)
	mv bukMsgdistrTrainee.out $(BUK_BUILD_DIR)

clean:
	rm -f *.o

#=============================================================================
#<end of Makefile>
#=============================================================================
