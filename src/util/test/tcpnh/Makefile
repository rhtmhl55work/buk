#=============================================================================
# Internet Telephony Software - (c) Copyright 2002 by Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
#
# Filename	: Makefile
# Description	: Makefile for the src directory.
# Author	: Sumit Bijwe
# Created On	: Wed Feb 25 15:14:56 IST 2004
# Last Modified : 
#=============================================================================

include makefile.inc

SRCSUBDIRS = basictest \
		featuretest \
		mp3Streamer
      
all: test 

test:
	@for i in $(SRCSUBDIRS); do \
		if (cd $$i && $(MAKE)) \
			then true; \
			else exit 1; \
		fi; \
	done


clean:
	@for i in $(SRCSUBDIRS); do (cd $$i; $(MAKE) clean); done

#=============================================================================
#	<end of Makefile>
#=============================================================================
