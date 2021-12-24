#makefile for file system

CC=g++
CGLAGS = -g -Wall

all: main

main: file_control.cpp
	$(CC) $(CFLAGS) -o main file_system.cpp


clean:
	rm main
