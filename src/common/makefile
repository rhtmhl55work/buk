#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	    : Makefile
# Description	: Makefile for basic utility kit assignment subdir common
# Author	    :
# Created On	: 12.11.2013
# Last Modified :
#=============================================================================

include ../../makefile.inc

SRC_FILES = bukAdminMentorMsgs.cpp \
	bukUiAdminMsgs.cpp \
	bukUiMentorMsgs.cpp \
	bukUiTraineeMsgs.cpp \
	bukAdminTraineeMsgs.cpp \
	bukMentorTraineeMsgs.cpp \
	bukMyTraineeMenuTraineeMsgs.cpp
# rohitm
OBJ_FILES = $(SRC_FILES:.cpp=.o)

BUK_LIB = libbukcommon.a

all: $(BUK_LIB)

$(BUK_LIB): $(OBJ_FILES)
	@$(AR) $(ARFLAGS) $(BUK_LIB) $^

clean:
	rm -f *.o *.a

#=============================================================================
#<end of Makefile>
#=============================================================================
