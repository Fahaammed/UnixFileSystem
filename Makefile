CC = g++
C  = gcc
CCFLAGS = -Wall _Werror -std=c++11
CFLAGS = -Wall -Werror

fs: FileSystem.cpp FileSystem.h
	g++ -Wall -std=c++11 -c -g FileSystem.cpp
	g++ -Wall -std=c++11 -g FileSystem.o -o fs

clean:
	rm fs *.o

compress:
	tar -czvf fs-sim.tar.gz readme.md *.h *.cpp Makefile