#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	    : Makefile
# Description	: Makefile for basic utility kit assignment test subdir
# Author	    : Trainee 2013
# Created On	: 12.11.2013
# Last Modified :
#=============================================================================
include ../makefile.inc

BUK_UI_SRC = bukAdminMenu.cpp \
	bukMentorMenu.cpp \
	bukTraineeMenu.cpp

BINARIES = bukAdminMenu.out \
	bukMentorMenu.out \
	bukTraineeMenu.out

all: $(BINARIES)

bukAdminMenu.out: bukAdminMenu.o
	$(LD) $(LDFLAGS) bukAdminMenu.o -l$(BUK_LIB_NAME) -o $(@) $(LDOPTS)
	mv bukAdminMenu.out $(BUK_BUILD_DIR)

bukMentorMenu.out: bukMentorMenu.o
	$(LD) $(LDFLAGS) bukMentorMenu.o -l$(BUK_LIB_NAME) -o $(@) $(LDOPTS)
	mv bukMentorMenu.out $(BUK_BUILD_DIR)

bukTraineeMenu.out: bukTraineeMenu.o
	$(LD) $(LDFLAGS) bukTraineeMenu.o -l$(BUK_LIB_NAME) -o $(@) $(LDOPTS)
	mv bukTraineeMenu.out $(BUK_BUILD_DIR)

clean:
	rm -f *.o

#=============================================================================
#<end of Makefile>
#=============================================================================
