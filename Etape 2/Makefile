.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Keep intermediate files
#.PRECIOUS: %.o

CC = gcc
CFLAGS = -Wall
LDFLAGS =

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LIBS += -lpthread

INCLUDE = Lib/csapp.h
OBJS = Lib/csapp.o
INCLDIR = -I.
PROGS = ftpclient ftpserver

all:
	cd Client && make
	cd Serveur && make
	cp Serveur/ftpserver ftpserver
	cp Client/ftpclient ftpclient
	
clean:
	rm -f $(PROGS)
	cd Client && make clean
	cd Serveur && make clean
