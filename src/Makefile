SOURCES = test.cpp Bus.cpp CPU.cpp
CC=g++
CFLAGS=-c -Wall

all: main

main: test.o Bus.o CPU.o
	$(CC) test.o Bus.o CPU.o -o main

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

%.o: %.cpp
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o all
