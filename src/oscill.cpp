#include "oscill.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <iterator>
#include <vector>

int main(int argc, char * args[]) {

    srand(time(NULL));

    /* Try to initialise SDL. */
    if( ! init( ) )
        return 1;

    SDL_WindowRenderer graphs ( SCREEN_WIDTH, SCREEN_HEIGHT, "oscill" );
    VerticalPane settings ( SCREEN_WIDTH, SCREEN_HEIGHT, "Settings" );

    int x, y;
    SDL_GetWindowPosition( graphs.get_window(), &x, &y );
    SDL_SetWindowPosition( settings.get_window(), x + SCREEN_WIDTH, y );

    TTF_Font *font;
    if( !( font = get_font( ) ) )
        return 1;

    /* The number of samples to store. */
    int no_sample_points = 100;
    /* The rate at which the wave is sampled; samples per second. */
    int sample_rate = 50;
    /* The total time elapsed (not really). */
    time_t t = 0;

    SDL_Slider<int> sample_slider( settings, 10,
            10 + settings.get_used_height(), SCREEN_WIDTH - 20, 40,
            &sample_rate,
            [](double proportion) {
                return (int)average( MIN_SAMPLE_RATE, MAX_SAMPLE_RATE,
                        proportion );
            },
            0.5, font, "sample rate"
    );
    settings.increase_height( 50 );

    /* Create all of the generated signals. */
    Wave carrier_wave = { 1.0, 0.0, 100.0};
    Wave cos_wave = { 1.0, M_PI / 2, 100.0 };
    Wave higher_wave = { 5.0, 0.0, 10.0 };

    Signal carrier ( graphs, settings, no_sample_points, &carrier_wave,
            {255, 255, 255}, font );
    Signal cos ( graphs, settings, no_sample_points, &cos_wave, {0, 255, 0}, font );
    Signal noisy = carrier + &higher_wave;
    //carrier = carrier + &higher_wave;

    std::vector<Signal*> signals;
    signals.push_back(&carrier);
    signals.push_back(&cos);
    signals.push_back(&noisy);
    std::vector<Signal*>::iterator it;

    /* Generate the initial state of the signals. */
    for(int i = 0; i < no_sample_points; i++) {
        it = signals.begin();
        while( it != signals.end() ) {
            (*it)->update_point( i, t );
            ++it;
        }
        t += MS_PER_S / sample_rate;
    }

    int start_index = 0;

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
            if(e.type == SDL_WINDOWEVENT &&
                    e.window.event == SDL_WINDOWEVENT_CLOSE) {
                quit = 1;
                break;
            }
        }

        set_scene( graphs );

        /* Draw the oldest samples. */
        for(int i = start_index; i < no_sample_points - 1; i++) {
            auto opacity = ((i-start_index) / ((double)no_sample_points)) *
                    255;

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
        /* Draw the newer samples. */
        for(int i = 0; i < start_index - 1; i++ ) {
            auto opacity = (1.0 - ((start_index - i)/(double)no_sample_points))
                    * 255;
            it = signals.begin();
            while( it != signals.end() ) {
                (*it)->draw_line( i, opacity );
                ++it;
            }
        }

        /* Get the current sample. */
        it = signals.begin();
        while( it != signals.end() ) {
            (*it)->update_point( start_index, t );
            ++it;
        }

        /* Draw the current render. */
        SDL_RenderPresent( graphs.get_renderer() );
        SDL_RenderPresent( settings.get_renderer() );


        /* Update the total time, and wait the sample time. */
        start_index = (start_index + 1) % no_sample_points;
        t += MS_PER_S / sample_rate;
        SDL_Delay( MS_PER_S / sample_rate );

    }

    /* Once the application has finished, clean up the SDL resources. */
    close( );

    return 0;
}
