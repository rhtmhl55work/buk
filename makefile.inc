#=============================================================================
# BUK Assignment - (c) Copyright 2013, Application Software Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : makefile.inc
# Description  : common makefile variables
# Author       :
# Created On   : 12.11.2013
# Last Modified:  
#=============================================================================

#BASE_DIR=/home/prama/Documents/bukassignment/
BASE_DIR=/home/rohitm/Documents/SystemManagement/BUK/buk_assignment_rohitm
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
BUK_SRC_DIR = $(BUK_BASE_DIR)/src
BUK_TEST_DIR = $(BUK_BASE_DIR)/test
BUK_BUILD_DIR = $(BUK_BASE_DIR)/build
CDSU_UTIL_BASE_DIR = $(BUK_SRC_DIR)/util

#-----------------------------------------------------------------------------
# Customization of makefile
#-----------------------------------------------------------------------------

CCOPTS += -DCDSU_TRACING 
#-DCDSU_NETPRINT
#CCFLAGS +=  -I$(BUK_COMMON_DIR) 
CCFLAGS += -I$(BUK_INCLUDE) \
	-I$(CDSU_UTIL_INCLUDE) \
	-I$(BUK_BASICINFRA)
LDOPTS += -L$(BUK_LIB_PATH) \
		-L$(CDSU_UTIL_LIB_PATH) \
		-L$(BUK_BASICINFRA)

#-----------------------------------------------------------------------------
# general rules
#-----------------------------------------------------------------------------
%.o:%.c; $(CC) $(CCFLAGS) $(CCOPTS) $< -o $@
%.o:%.cpp; $(CC) $(CCFLAGS) $(CCOPTS) $< -o $@

#=============================================================================
# <End of makefile.inc>
#=============================================================================
