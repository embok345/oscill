#ifndef CONSTS_H
#define CONSTS_H

/* The width and height of the window to be shown. */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

/* The minimum and maximum values for the frequency of the wave. */
#define MIN_FREQUENCY 0.1
#define MAX_FREQUENCY 10

/* The minimum and maximum values for the sample rate from the wave. */
#define MIN_SAMPLE_RATE 1
#define MAX_SAMPLE_RATE 100

/* The maximum amplitude of the wave. */
#define MAX_AMPLITUDE 180

/* The level 0 in the graph. */
#define ZERO_LEVEL (SCREEN_HEIGHT / 2)

/* Milliseconds in a second. */
#define MS_PER_S 1000.0

#define PX_PER_S 100

#define M_TAU (2 * M_PI)

#endif
