#=============================================================================
# Internet Telephony Software - (c) Copyright 2006 by Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	: Makefile
# Description	: Makefile for basic utility kit
# Author	: Vijaya Kumar 
# Created On	: Tue Apr 25 14:20:11 IST 2006
# Last Modified : 
#=============================================================================


# Sub Dir.
SRCDIR=./src
TESTDIR=./test

all: lib testing

lib:  
	@cd $(SRCDIR); $(MAKE)
testing:
	@cd $(TESTDIR); $(MAKE)

clean: lib-clean test-clean

lib-clean:
	@cd $(SRCDIR); $(MAKE) clean

test-clean:
	@cd $(TESTDIR); $(MAKE) clean

#=============================================================================
#<end of Makefile>
#=============================================================================
