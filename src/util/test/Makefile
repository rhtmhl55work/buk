#=============================================================================
# Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : Makefile
# Description  : makefile for int hash tests
# Author       : Titty Thomas
# Created On   : Wed Apr 30 17:09:45 IST 2003
# Last Modified:
#=============================================================================

include ../makefile.inc

LIBS = $(CDSU_UTIL_SRC_DIR)/libcdsuutil.a

CDSU_UTIL_TEST_SRC = thread-test.cpp \
	bukmsgthrd.cpp \
	bukmsgthrd2.cpp \
	bukMsgdistrAdmin.cpp
		#inthash-test.cpp \
		     #charhash-test.cpp \
		     #mbhash-test.cpp \
		     #list-test.cpp\
		     #taskscheduler-test.cpp \
		     #tsqueue-test.cpp \
		     #queue-test.cpp \
		     #tsqueue-thread-test.cpp \
		     #msgdistr-test.cpp \
		     #msgdistr2-test.cpp \
		     #trace-test.cpp \
		     #utiltypes-test.cpp \
		 #    thread-test.cpp 
ifndef ECOS
CDSU_UTIL_TEST_SRC += unixmemmon-test.cpp 
endif	
CDSU_UTIL_TEST_SRC_C = utiltypes-test-c.c

CDSU_UTIL_TEST_OBJ = $(CDSU_UTIL_TEST_SRC:.cpp=.o)
CDSU_UTIL_TEST_OBJ += $(CDSU_UTIL_TEST_SRC_C:.c=.o)

BINARIES = thread-test.out\
	bukmsgthrd.out \
	bukmsgthrd2.out	\
	bukMsgdistrAdmin.out

	#utiltypes-test.out \
       	   #inthash-test.out \
	#utiltypes-test-c.out \
	#	   charhash-test.out \
	#	   mbhash-test.out \
         #  list-test.out \
	#	   taskscheduler-test.out \
         #  tsqueue-test.out \
          # queue-test.out \
           #tsqueue-thread-test.out  \
	    #   trace-test.out \
	#	   thread-test.out \
	#	   msgdistr-test.out
	  
ifndef ECOS
BINARIES += unixmemmon-test.out
endif

ifdef TEST1
BINARIES+= msgdistr-test.out
else
BINARIES+= msgdistr2-test.out
endif


all: $(BINARIES)


utiltypes-test.out:utiltypes-test.o
	$(LD) $(LDFLAGS) utiltypes-test.o $(LIBS) -o $@ $(LDOPTS)

utiltypes-test-c.out:utiltypes-test-c.o
	$(LD) $(LDFLAGS) utiltypes-test-c.o $(LIBS) -o $@ $(LDOPTS)

inthash-test.out: inthash-test.o
	$(LD) $(LDFLAGS) inthash-test.o $(LIBS) -o $@ $(LDOPTS)

charhash-test.out: charhash-test.o
	$(LD) $(LDFLAGS) charhash-test.o $(LIBS) -o $@ $(LDOPTS)

mbhash-test.out: mbhash-test.o
	$(LD) $(LDFLAGS) mbhash-test.o $(LIBS) -o $@ $(LDOPTS)

list-test.out: list-test.o
	$(LD) $(LDFLAGS) list-test.o $(LIBS) -o $@ $(LDOPTS)

taskscheduler-test.out: taskscheduler-test.o
	$(LD) $(LDFLAGS) taskscheduler-test.o $(LIBS) -o $@ $(LDOPTS)

tsqueue-test.out:tsqueue-test.o
	$(LD) $(LDFLAGS) tsqueue-test.o $(LIBS) -o $@ $(LDOPTS)

queue-test.out:queue-test.o
	$(LD) $(LDFLAGS) queue-test.o $(LIBS) -o $@ $(LDOPTS)

tsqueue-thread-test.out:tsqueue-thread-test.o
	$(LD) $(LDFLAGS) tsqueue-thread-test.o $(LIBS) -o $@ $(LDOPTS)

msgdistr-test.out:msgdistr-test.o
	$(LD) $(LDFLAGS) msgdistr-test.o $(LIBS) -o $@ $(LDOPTS)

msgdistr2-test.out:msgdistr2-test.o
	$(LD) $(LDFLAGS) msgdistr2-test.o $(LIBS) -o $@ $(LDOPTS)

trace-test.out:trace-test.o
	$(LD) $(LDFLAGS) trace-test.o $(LIBS) -o $@ $(LDOPTS)

thread-test.out:thread-test.o
	$(LD) $(LDFLAGS) thread-test.o $(LIBS) -o $@ $(LDOPTS)

unixmemmon-test.out:unixmemmon-test.o
	$(LD) $(LDFLAGS) unixmemmon-test.o $(LIBS) -o $@ $(LDOPTS)

bukmsgthrd.out:bukmsgthrd.o
	$(LD) $(LDFLAGS) bukmsgthrd.o $(LIBS) -o $@ $(LDOPTS)

bukmsgthrd2.out:bukmsgthrd2.o
	$(LD) $(LDFLAGS) bukmsgthrd2.o $(LIBS) -o $@ $(LDOPTS)

bukMsgdistrAdmin.out:bukMsgdistrAdmin.o
	$(LD) $(LDFLAGS) bukMsgdistrAdmin.o $(LIBS) -o $@ $(LDOPTS)

#abc:	
#	cd /home/prama/Sumathi/Documents/training/bukassignment/src/util/test/timer; make


clean:
	rm -f *.o *.tmp *.map $(BINARIES) $(CDSU_UTIL_TEST_OBJ) core

		
#=============================================================================
# <End of Makefile>
#=============================================================================
