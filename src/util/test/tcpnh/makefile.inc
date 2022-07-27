#=============================================================================
# Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : makefile.inc
# Description  : makefile includes
# Author       : Sumit Bijwe
# Created On   : Wed Jan 29 17:04:11 IST 2003
# Last Modified:
#============================================================================

#UTIL_BASE_DIR = /home/prama/Documents/bukassignment/src/util
#UTIL_BASE_DIR = /home/rohitm/Documents/SystemManagement/BUK/BUK_System_Design_Assignment/src/util
SRCDIR = .

#-----------------------------------------------------------------------------
# eCos directory 
#-----------------------------------------------------------------------------
ECOSDIR = /export/home1/seema/users/build-192.168.50.176/install/

#-----------------------------------------------------------------------------
# defines
#-----------------------------------------------------------------------------
DEFINES = -DCDSU_TIMER_STANDALONE -DCDSU_TRACING -DCDSU_NETPRINT
ifdef SERVER
DEFINES += -DTEST_SERVER -DTEST_INPUT_THRU_SOCK -DM_TRACING -DCDSU_TRACING
endif
ifdef ECOS
DEFINES += -D__ECOS -DM_TRACING -DCDSU_TRACING
endif

#-----------------------------------------------------------------------------
# compiler and flags
#-----------------------------------------------------------------------------
ifdef ECOS
CC = powerpc-eabi-gcc -msoft-float -mcpu=860

CCFLAGS = -c -msoft-float -mcpu=860 -Wall -Wpointer-arith \
	-Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g \
	-ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
	-finit-priority -Wp,-MD,clock.tmp
CCFLAGS += -I$(ECOSDIR)/include
else
CC = g++
endif

CCFLAGS += -g -c -Wall $(INCLUDES)

CCOPTS = $(DEFINES)

#-----------------------------------------------------------------------------
# includes
#-----------------------------------------------------------------------------
#INCLUDES = -Iutilities/basic/include \
	-Iutilities/timer/src/timer
	
INCLUDES = -I$(SRCDIR) \
		-I$(UTIL_BASE_DIR)/include

#-----------------------------------------------------------------------------
# linker and flags
#-----------------------------------------------------------------------------
ifdef ECOS
LD = $(CC) 
LDFLAGS = -nostartfiles -nostdlib -Xlinker -Map -Xlinker $@.map \
	-Wl,--gc-sections -L$(ECOSDIR)/lib
else
LD = g++
endif

LDFLAGS += -g
#LDFLAGS += -L$(UTIL_BASE_DIR)/src

LDFLAGS += -L$(UTIL_BASE_DIR)/src

ifdef ECOS
LDOPTS = -L$(ECOSDIR)/lib -Ttarget.ld $(LIBS)
else
#LDOPTS = $(LIBS) -lpthread
# rohitm
LDOPTS = $(LIBS) -pthread
endif

# Solaris libraries
OS = $(shell uname -s)
ifeq (SunOS, $(findstring SunOS, $(OS)))
LDOPTS += -lsocket -lnsl -lposix4

endif

#-----------------------------------------------------------------------------
# libraries
#-----------------------------------------------------------------------------
LIBS = $(UTIL_BASE_DIR)/src/libcdsuutil.a

#-----------------------------------------------------------------------------
# general rules
#-----------------------------------------------------------------------------
%.o:%.cpp; $(CC) $(CCFLAGS) $(CCOPTS) $< -o $@

#============================================================================
# <End of makefile.inc>
#============================================================================
