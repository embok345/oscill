#include "oscill.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char * args[]) {

    srand(time(NULL));

    SDL_Window *w = NULL;
    SDL_Renderer *r = NULL;

    /* Try to initialise SDL, and the window and renderer. */
    if( ! init( &w, &r, SCREEN_WIDTH, SCREEN_HEIGHT ) ) {
        /* If we can't, exit the program. */
        return 1;
    }

    TTF_Font *font;
    if( !( font = get_font( "Anonymous_Pro" ) ) )
        return 1;

    /* Draw the initial graphics to the renderer. */

    /* Fill the window with black. */
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderClear( r );

    /* Set up the rectangle to contain the graph. */
    SDL_Rect graph_rect;
    graph_rect.x = 0;
    graph_rect.y = (ZERO_LEVEL - AMPLITUDE) * 0.9;
    graph_rect.w = SCREEN_WIDTH;
    graph_rect.h = 3 * AMPLITUDE;

    /* Set the statistics for generating the wave. */

    /* The number of samples to store. */
    int no_sample_points = 100;
    /* The horizontal scale of the graph. */
    int pixels_per_second = 100;
    /* The initial frequency of the wave; oscillations per second. */
    double frequency = 1.0;
    double old_frequency = frequency;
    /* The rate at which the wave is sampled; samples per second. */
    double sample_rate = 50.0;
    /* The total time elapsed (not really). */
    time_t t = 0;
    /* The phase offset of the wave. Changed when the frequency is changed, so
     * that the current sample point remains the same. */
    double phase = 0.0;

    int start_index = 0;

    SDL_Point points[no_sample_points];
    for(int i = 0; i < no_sample_points; i++) {
        double v = AMPLITUDE * sin( M_TAU * frequency * (t / MS_PER_S));
        points[i].x = (t / MS_PER_S) * pixels_per_second;
        points[i].y = ZERO_LEVEL - v;
        t += MS_PER_S / sample_rate;
    }


    /* Create the frequency and sample rate sliders. */
    SDL_Slider frequency_slider( r, 10, 10, (SCREEN_WIDTH / 2) - 20,
            40, &frequency,
            [](double proportion) {
                if(proportion < 0) proportion = 0;
                else if(proportion > 1) proportion = 1;
                return pow(10, ((1 - proportion) * log10(MIN_FREQUENCY)) +
                        (proportion * log10(MAX_FREQUENCY)));
            },
            0.5, font, "frequency"
    );

    SDL_Slider sample_slider( r, (SCREEN_WIDTH / 2) + 10, 10,
            (SCREEN_WIDTH / 2) - 20, 40, &sample_rate,
            [](double proportion) {
                if(proportion < 0) proportion = 0;
                else if(proportion > 1) proportion = 1;
                return ((1-proportion) * MIN_SAMPLE_RATE) +
                        (proportion * MAX_SAMPLE_RATE);
            },
            0.5, font, "sample rate"
    );

    double test_val = 0;
    SDL_Slider test_slider( r, 10, SCREEN_HEIGHT - 50, (SCREEN_WIDTH / 2) - 20,
            40, &test_val, [](double proportion) { return 0.0; }, 0.5,
            font, "ABCD EFGy" );

    /* Run the application until we get the quit signal. */
    int quit = 0;
    SDL_Event e;

    while( !quit ) {

        /* Handle all events in the queue. */
        while( SDL_PollEvent( &e ) != 0 ) {
            /* If quit signal is received, stop the loop after the next one. */
            if(e.type == SDL_QUIT) {
                quit = 1;
                break;
            }
        }

        /* Clear the screen. */
        SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect( r, &graph_rect );

        /* Draw the top and bottom guide lines. */
        SDL_SetRenderDrawColor( r, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine( r, 0, ZERO_LEVEL - AMPLITUDE, SCREEN_WIDTH,
                ZERO_LEVEL - AMPLITUDE );
        SDL_RenderDrawLine( r, 0, ZERO_LEVEL + AMPLITUDE, SCREEN_WIDTH,
                ZERO_LEVEL + AMPLITUDE );

        /* Draw the center guide line. */
        SDL_SetRenderDrawColor( r, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine( r, 0, ZERO_LEVEL, SCREEN_WIDTH, ZERO_LEVEL );

        /* Draw a black line, otherwise the alpha goes weird.
         * TODO i'm sure there is a correct way to make sure the blending
         * doesn't go weird. */
        SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderDrawLine( r, 0, 0, 2, 0 );

        /* Draw the oldest lines. */
        for(int i = start_index; i < no_sample_points - 1; i++) {
            /* If the line goes backward, don't draw it. */
            if( points[ i ].x > points[ i + 1 ].x ) continue;

            /* Draw the line, with older lines having less opacity. */
            SDL_SetRenderDrawColor( r, 255, 255, 255,
                    ((i-start_index) / ((double)no_sample_points)) * 255 );
            SDL_RenderDrawLine( r, points[ i ].x, points[ i ].y,
                    points[ i + 1 ].x, points[ i + 1 ].y);
        }


        /* If the frequency has changed since the last sample, update the
         * phase offset. */
        if(frequency != old_frequency) {
            phase = ( ( M_TAU * ( t / MS_PER_S) * old_frequency ) + phase ) -
                    ( M_TAU * ( t / MS_PER_S ) * frequency );
            /* Normalise the phase offset in (0, 2pi). */
            phase -= M_TAU * floor(phase / M_TAU);
            old_frequency = frequency;
        }

        /* Get the next sample. */
        double v = AMPLITUDE * sin(M_TAU * frequency * (t / MS_PER_S) + phase);
        points[start_index].x = (int)((t / MS_PER_S) * pixels_per_second) %
                SCREEN_WIDTH;
        points[start_index].y = ZERO_LEVEL - v;


        /* Draw the newer samples. We don't actaully draw the newest sample. */
        for(int i = 0; i < start_index - 1; i++ ) {
            /* If the line goes backwards, don't draw it. */
            if( points[ i ].x > points[ i + 1 ].x ) continue;

            /* Draw the lines, with older lines having less opacity. */
            double opacity = no_sample_points + i - start_index;
            SDL_SetRenderDrawColor( r, 255, 255, 255,
                    (opacity / (double)no_sample_points) * 255);
            SDL_RenderDrawLine( r, points[ i ].x, points[ i ].y,
                    points[ i + 1 ].x, points[ i + 1 ].y);
        }

        /* Draw the line between the end and start of the array, if present. */
        if(start_index != 0 && points[no_sample_points - 1].x < points[0].x) {
            SDL_SetRenderDrawColor( r, 255, 255, 255,
                    ( ( no_sample_points - start_index ) /
                      (double)no_sample_points ) * 255 );
            SDL_RenderDrawLine( r, points[ no_sample_points - 1 ].x,
                    points[ no_sample_points - 1 ].y,
                    points[ 0 ].x, points[ 0 ].y);
        }

        /* Draw the current render. */
        SDL_RenderPresent( r );

        /* Update the total time, and wait the sample time. */
        start_index = (start_index + 1) % no_sample_points;
        t += MS_PER_S / sample_rate;
        SDL_Delay( MS_PER_S / sample_rate );

    }

    /* Once the application has finished, clean up the SDL resources. */
    close(&w, &r);

    return 0;
}
