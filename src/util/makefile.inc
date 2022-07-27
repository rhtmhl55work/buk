#=============================================================================
# Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : makefile.inc
# Description  : common makefile variables
# Author       : Titty Thomas
# Created On   : Wed Apr 30 17:01:52 IST 2003
# Last Modified: 1. By Sumit Bijwe
#		 2. Makefile has been changed according new strategy
#		as explained in doc
#		itel/modules/ss7amf/docs/misc/notes/organizing-makefiles.sxw 
#=============================================================================

ifndef INTEGRATION
export BASE_DIR = $(PWD)
endif
#BASE_DIR=/home/prama/Documents/bukassignment/src/util
#BASE_DIR=/home/rohitm/Documents/SystemManagement/BUK/BUK_System_Design_Assignment/src/util
#BASE_DIR = $(PWD)
#-----------------------------------------------------------------------------
# Following is mandatory include
#-----------------------------------------------------------------------------
include $(BASE_DIR)/makefile.path

#-----------------------------------------------------------------------------
# Include following one if you want makefile.flag
# If your makefile.inc have common flags, then you need not include following 
# one. But it is good practice to include this file and remove common flags 
# from your makefile.inc
# So While integration from one place you can changes these common flags.
# Keep only module dependent flags in makefile.inc
#-----------------------------------------------------------------------------
include $(BASE_DIR)/makefile.flag

#-----------------------------------------------------------------------------
# common directories
#-----------------------------------------------------------------------------
CDSU_UTIL_SRC_DIR = $(CDSU_UTIL_BASE_DIR)/src
CDSU_UTIL_TEST_DIR = $(CDSU_UTIL_BASE_DIR)/test
MSG_DIR = $(CDSU_UTIL_BASE_DIR)/../common
#-----------------------------------------------------------------------------
# Customization of makefile
#-----------------------------------------------------------------------------

CCOPTS += -DCDSU_TRACING 
#-DCDSU_NETPRINT
CCFLAGS +=  -I$(CDSU_UTIL_INCLUDE) -I$(MSG_DIR)
LDOPTS += -L$(CDSU_UTIL_LIB_PATH)

#-----------------------------------------------------------------------------
# general rules
#-----------------------------------------------------------------------------
%.o:%.c; $(CC) $(CCFLAGS) $(CCOPTS) $< -o $@
%.o:%.cpp; $(CC) $(CCFLAGS) $(CCOPTS) $< -o $@

#=============================================================================
# <End of makefile.inc>
#=============================================================================
