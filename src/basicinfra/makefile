#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	: Makefile
# Description	: Makefile for basic utility kit assignment subdir basicinfra
# Author	: Trainee - 2013
# Created On	: 12.11.2013
# Last Modified :
#=============================================================================

include ../../makefile.inc

SRC_FILES = basicinfraAdmin.cpp \
	basicinfraMentor.cpp \
	basicinfraTrainee.cpp \
	basicinfraMyTraineeMenu.cpp
# rohitm
OBJ_FILES = $(SRC_FILES:.cpp=.o)

BUK_BASICINFRA_LIB = libbukbasicinfra.a

all: $(BUK_BASICINFRA_LIB)

$(BUK_BASICINFRA_LIB): $(OBJ_FILES)
	@$(AR) $(ARFLAGS) $(BUK_BASICINFRA_LIB) $^

clean:
	rm -f *.o *.a

#=============================================================================
#<end of Makefile>
#=============================================================================
