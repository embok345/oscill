#include "oscill.h"
#include <stdio.h>
#include <time.h>
#include <iterator>
#include <vector>

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
    if( !( font = get_font( ) ) )
        return 1;

    /* Draw the initial graphics to the renderer. */

    /* Fill the window with black. */
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderClear( r );

    /* Set up the rectangle to contain the graph. */
    SDL_Rect graph_rect;
    graph_rect.x = 0;
    graph_rect.y = ZERO_LEVEL - MAX_AMPLITUDE;
    graph_rect.w = SCREEN_WIDTH;
    graph_rect.h = 2 * MAX_AMPLITUDE;

    /* Set the statistics for generating the wave. */

    /* The number of samples to store. */
    int no_sample_points = 100;
    /* The rate at which the wave is sampled; samples per second. */
    double sample_rate = 50.0;
    /* The total time elapsed (not really). */
    time_t t = 0;

    /* The initial frequency of the wave; oscillations per second. */
    double carrier_frequency = 1.0;
    double old_carrier_frequency = carrier_frequency;
    /* The phase offset of the wave. Changed when the frequency is changed, so
     * that the current sample point remains the same. */
    double carrier_phase = 0.0;
    double carrier_amplitude = 100.0;

    double cos_phase = carrier_phase + (M_PI/2);

    double modulation_amplitude_sensitivity = 0.5;
    double modulation_frequency = 0.3;
    double modulation_phase = 0.0;

    Signal carrier ( r, no_sample_points, &carrier_frequency, &carrier_phase,
            &carrier_amplitude, {255, 255, 255} );
    Signal cos ( r, no_sample_points, &carrier_frequency, &cos_phase,
            &carrier_amplitude, {0, 255, 0} );
    double higher_frequency = 10.0;
    double higher_amplitude = 10.0;
    double higher_phase = 0.0;
    Signal higher ( r, no_sample_points, &higher_frequency, &higher_phase,
            &higher_amplitude, { 0, 0, 255 } );

    Signal combined = carrier + higher;

    ModulatedSignal modulated ( carrier, &modulation_amplitude_sensitivity,
            &modulation_frequency, &modulation_phase, {255, 0, 0} );

    std::vector<Signal*> signals;
    //signals.push_back(&carrier);
    signals.push_back(&cos);
    signals.push_back(&higher);
    signals.push_back(&combined);
    //signals.push_back(&modulated);
    std::vector<Signal*>::iterator it;

    for(int i = 0; i < no_sample_points; i++) {
        it = signals.begin();
        while( it != signals.end() ) {
            (*it)->update_point( i, t );
            ++it;
        }
        t += MS_PER_S / sample_rate;
    }

    int start_index = 0;

    /* Create the settings sliders. */
    SDL_Slider frequency_slider( r, 10, 10, (SCREEN_WIDTH / 2) - 20,
            40, &carrier_frequency,
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

    SDL_Slider sensitivity_slider( r, 10, SCREEN_HEIGHT - 50,
            (SCREEN_WIDTH / 2) - 20, 40, &modulation_amplitude_sensitivity,
            [](double proportion) { return proportion; }, 0.5,
            font, "modulation sensitivity"
    );

    SDL_Slider modulation_frequency_slider( r, (SCREEN_WIDTH / 2) + 10,
            SCREEN_HEIGHT - 50, (SCREEN_WIDTH / 2) - 20, 40,
            &modulation_frequency,
            [](double proportion) {
                if(proportion < 0) proportion = 0;
                else if(proportion > 1) proportion = 1;
                return ((1 - proportion) * 0.5) + (proportion * 0.1);
            },
            0.5, font, "modulation frequency"
    );

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

        set_scene( r, &graph_rect );

        /* Draw the oldest lines. */
        for(int i = start_index; i < no_sample_points - 1; i++) {
            auto opacity = ((i-start_index) / ((double)no_sample_points)) *
                    255;

            it = signals.begin();
            while( it != signals.end() ) {
                (*it)->draw_line( i, opacity );
                ++it;
            }
        }
        /* Draw the newer samples. We don't actaully draw the newest sample. */
        for(int i = 0; i < start_index - 1; i++ ) {
            auto opacity = (1.0 - ((start_index - i)/(double)no_sample_points))
                    * 255;
            it = signals.begin();
            while( it != signals.end() ) {
                (*it)->draw_line( i, opacity );
                ++it;
            }
        }

        /* Draw the line between the end and start of the array, if present. */
        if(start_index != 0) {
            auto opacity = ( 1 - (start_index / (double)no_sample_points))
                    * 255;
            it = signals.begin();
            while( it != signals.end() ) {
                (*it)->draw_line( no_sample_points - 1, opacity );
                ++it;
            }
        }

        /* If the frequency has changed since the last sample, update the
         * phase offset. */
        if(carrier_frequency != old_carrier_frequency) {
            carrier_phase =
                    ( ( M_TAU * ( t / MS_PER_S) * old_carrier_frequency ) +
                    carrier_phase ) -
                    ( M_TAU * ( t / MS_PER_S ) * carrier_frequency );
            /* Normalise the phase offset in (0, 2pi). */
            carrier_phase -= M_TAU * floor(carrier_phase / M_TAU);
            cos_phase = carrier_phase + ( M_PI / 2 );
            old_carrier_frequency = carrier_frequency;
        }


        it = signals.begin();
        while( it != signals.end() ) {
            (*it)->update_point( start_index, t );
            ++it;
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
