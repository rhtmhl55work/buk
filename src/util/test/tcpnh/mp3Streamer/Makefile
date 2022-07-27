#=============================================================================
# Internet Telephony Software - (c) Copyright 2003, Internet Telephony Group
# Center for Development of Telematics (C-DOT), Bangalore, India.
# Filename     : Makefile
# Description  : makefile for testclient and testserver 
# Author       : Titty Thomas
# Created On   : Wed Jan 29 17:15:29 IST 2003
# Last Modified:
#============================================================================

include ../makefile.inc

SRC_FILES = mp3StreamerServer.cpp \
	    mp3StreamServer.cpp 

CLIENT_OBJS = mp3StreamerClient.cpp
	      	   

SRC_OBJS = $(SRC_FILES:.cpp=.o)
CLIENT_OBJS = $(SRC_FILES:.cpp=.o)

all : mp3StreamerServer mp3StreamerClient
	
mp3StreamerServer: $(SRC_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(SRC_OBJS) $(LDOPTS)

mp3StreamerClient: mp3StreamerClient.o
	$(LD) $(LDFLAGS) -o $@ mp3StreamerClient.o $(LDOPTS)


#mp3StreamerClient : mp3StreamerClient.cpp
#	 g++  -g mp3StreamerClient.cpp -o  mp3StreamerClient


clean: 
	-rm -f *.o *.tmp *.map mp3StreamerServer mp3StreamerClient *.ab* *.bdx core

#============================================================================
# <End of Makefile>
#============================================================================
