# The makefile used for compiling the source files and generating the executable "mydispatcher".
#
# - Author: Hieu Tran
# - Course: COMP 354: Operating Systems
# - Instructor: Shikha Mittal

SOURCES = mydispatcher.c myutility.c spn.c srt.c
OBJECTS = mydispatcher.o myutility.o spn.o srt.o
HEADERS = mydispatcher.h
CC = gcc
CFLAGS = -g -Wall

mydispatcher: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o mydispatcher
clean:
	rm -fR *.o mydispatcher