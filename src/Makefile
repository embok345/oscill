LIBRARY_PATH = -L/home/poulter/lib
INCLUDE_PATH = -I/home/poulter/include

EXECUTABLE_PATH = ../bin/

CC = g++

CFLAGS = -std=gnu++11 -g -Wall -Wno-unused-variable -Wno-unused-but-set-variable
LDFLAGS = -lm
SDLFLAGS = -lSDL2 $(sdl2-config --libs --cflags)
OBJ = sdl_util.o slider.o oscill.o

NAME = oscill

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

oscill : $(OBJ)
	$(CC) $^ -o $(EXECUTABLE_PATH)$@ $(CFLAGS) $(LDFLAGS) $(SDLFLAGS) $(LIBRARY_PATH) $(INCLUDE_PATH)

.PHONY: clean
clean:
	rm -f *.o *~ core
