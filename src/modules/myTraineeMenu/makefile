#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	    : Makefile
# Description	: Makefile for basic utility kit assignment subdir trainee
# Author	    : rohitm
# Created On	: 27.07.2022
# Last Modified :
#=============================================================================

include ../../../makefile.inc

LIBS = $(CDSU_UTIL_LIB_PATH)/libcdsuutil.a

SRC_FILES = bukMsgdistrMyTraineeMenu.cpp \
	bukMyTraineeMenu.cpp

BINARIES = bukMsgdistrMyTraineeMenu.out

all: $(BINARIES)

bukMsgdistrMyTraineeMenu.out:bukMsgdistrMyTraineeMenu.o bukMyTraineeMenu.o
	$(LD) $(LDFLAGS) bukMsgdistrMyTraineeMenu.o bukMyTraineeMenu.o -l$(BUK_LIB_NAME) -l$(BUK_BASICINFRA_LIB) $(LIBS) -o $@ $(LDOPTS)
	mv bukMsgdistrMyTraineeMenu.out $(BUK_BUILD_DIR)

clean:
	rm -f *.o

#=============================================================================
#<end of Makefile>
#=============================================================================
