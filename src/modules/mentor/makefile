#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename  	: Makefile
# Description	: Makefile for basic utility kit assignment subdir mentor
# Author	    : Trainee 2013
# Created On	: 12.11.2013
# Last Modified :
#=============================================================================

include ../../../makefile.inc

LIBS = $(CDSU_UTIL_LIB_PATH)/libcdsuutil.a

SRC_FILES = bukMsgdistrMentor.cpp \
	bukMentor.cpp

BINARIES = bukMsgdistrMentor.out

all: $(BINARIES)

bukMsgdistrMentor.out:bukMsgdistrMentor.o bukMentor.o
	$(LD) $(LDFLAGS) bukMsgdistrMentor.o bukMentor.o $(LIBS) -l$(BUK_LIB_NAME) -l$(BUK_BASICINFRA_LIB) -o $@ $(LDOPTS)
	mv bukMsgdistrMentor.out $(BUK_BUILD_DIR)

clean:
	rm -f *.o

#=============================================================================
#<end of Makefile>
#=============================================================================
