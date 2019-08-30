MK_PATH = $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_DIR = $(dir $(MK_PATH))
EX_DIR = $(SRC_DIR)../bin/
EX_NAME = oscill

CC = g++

WFLAGS = -Wall -Wno-unused-variable -Wno-unused-but-set-variable
CFLAGS = -std=gnu++11 -g $(WFLAGS)
LDFLAGS = -lm
SDLFLAGS = -lSDL2 -lSDL2_ttf $(sdl2-config --libs --cflags)
OBJS = sdl_util.o slider.o oscill.o

slider.o: slider.cpp slider.h
	$(CC) -c -o $@ $< $(CFLAGS)
oscill.o: oscill.cpp oscill.h slider.h
	$(CC) -c -o $@ $< $(CFLAGS)
%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)


$(EX_NAME) : $(OBJS)
	$(CC) $^ -o $(EX_DIR)$(EX_NAME) $(CFLAGS) $(LDFLAGS) $(SDLFLAGS)

.PHONY: clean
clean:
	rm -f *.o *~ core $(EX_DIR)$(EX_NAME)
