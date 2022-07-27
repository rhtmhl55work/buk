#=============================================================================
# BUK Assignment - (c) Copyright 2013 by Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	    : Makefile
# Description	: Makefile for basic utility kit assignment
# Author	    :
# Created On	: 12.11.2013
# Last Modified :
#=============================================================================

# Sub Dir.
SRC_DIR=./src
TEST_DIR=./test
BUILD_DIR=./build

all: lib testing

lib:
	@cd $(SRC_DIR); $(MAKE)

testing:
	@cd $(TEST_DIR); $(MAKE)

clean:
	cd $(SRC_DIR); $(MAKE) clean
	cd $(TEST_DIR); $(MAKE) clean
	cd $(BUILD_DIR); rm -f *.out


#=============================================================================
#<end of Makefile>
#=============================================================================
