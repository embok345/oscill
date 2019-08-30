#ifndef OSCILL_H
#define OSCILL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "slider.h"
#include "sdl_util.h"

/* The width and height of the window to be shown. */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

/* The minimum and maximum values for the frequency of the wave. */
#define MIN_FREQUENCY 0.1
#define MAX_FREQUENCY 10

/* The minimum and maximum values for the sample rate from the wave. */
#define MIN_SAMPLE_RATE 1
#define MAX_SAMPLE_RATE 100

/* The level 0 in the graph. */
#define ZERO_LEVEL (SCREEN_HEIGHT / 2)
/* The amplitude of the wave.
 * TODO this should be modifiable. */
#define AMPLITUDE 50

/* Milliseconds in a second. */
#define MS_PER_S 1000.0

#define M_TAU (2 * M_PI)

#endif
