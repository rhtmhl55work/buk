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

include ../makefile.inc

# Sub Dir.
MISCDIR = ./misc
TIMERDIR = ./timer
MSGDISTRDIR = ./msgdistr
TCPNHDIR = ./tcpnh
UDPNHDIR = ./udpnh

# Main Util Lib.
CDSU_UTIL_LIB = libcdsuutil.a

# Sub Dir Lib files.
#MISCLIB =  $(MISCDIR)/libcdsumisc.a
#TIMERLIB = $(TIMERDIR)/libcdsutimer.a
#MSGDISTRLIB = $(MSGDISTRDIR)/libcdsumd.a
#TCPNHLIB = $(TCPNHDIR)/libcdsunhtcp.a


CDSU_UTIL_SUBDIR_LIB_FILES =  $(MISCLIB)\
	  $(TIMERLIB)\
	  $(MSGDISTRLIB)\
	  $(TCPNHLIB)\
	  $(UDPNHLIB)



SRCSUBDIRS = $(MISCDIR)\
	  $(TIMERDIR)\
	  $(MSGDISTRDIR)\
	  $(TCPNHDIR) \
	  $(UDPNHDIR)

      
all: sdlib lib	

sdlib:
	@for i in $(SRCSUBDIRS); do \
		if (cd $$i && $(MAKE)) \
			then true; \
			else exit 1; \
		fi; \
	done


lib:  
	@$(AR) $(ARFLAGS) $(CDSU_UTIL_LIB) $(MISCDIR)/*.o $(TIMERDIR)/*.o \
	$(MSGDISTRDIR)/*.o $(TCPNHDIR)/*.o $(UDPNHDIR)/*.o


clean:
	@for i in $(SRCSUBDIRS); do (cd $$i; $(MAKE) clean); done
	-rm -f *.o libcdsuutil.a *.map *.tmp core

#=============================================================================
#	<end of Makefile>
#=============================================================================
