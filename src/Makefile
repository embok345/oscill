MK_PATH = $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_DIR = $(dir $(MK_PATH))
EX_DIR = $(SRC_DIR)../bin/
EX_NAME = oscill

CC = g++

WFLAGS = -Wall -Wno-unused-variable -Wno-unused-but-set-variable
CFLAGS = -std=gnu++17 -g $(WFLAGS)
LDFLAGS = -lm
SDLFLAGS = -lSDL2 -lSDL2_ttf $(sdl2-config --libs --cflags)
OBJS = sdl_util.o signals.o slider.o oscill.o

GLOB_DEPS = slider.h sdl_util.h consts.h

slider.o: slider.cpp slider.h $(GLOB_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
oscill.o: oscill.cpp oscill.h slider.h signals.h $(GLOB_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
signals.o: signals.cpp signals.h $(GLOB_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
%.o: %.cpp $(GLOB_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EX_NAME) : $(OBJS)
	$(CC) $^ -o $(EX_DIR)$(EX_NAME) $(CFLAGS) $(LDFLAGS) $(SDLFLAGS)

.PHONY: clean
clean:
	rm -f *.o *~ core $(EX_DIR)$(EX_NAME)
