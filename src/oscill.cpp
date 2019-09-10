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
    SDL_WindowRenderer settings ( SCREEN_WIDTH, SCREEN_HEIGHT, "Settings" );

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

    VerticalPane settings_root_pane ( 2 );

    Slider<int> sample_slider( SCREEN_WIDTH - 20, 40, &sample_rate,
            [](double p) {
                return (int)average( MIN_SAMPLE_RATE, MAX_SAMPLE_RATE, p );
            }, 0.5, "sample rate", font );
    settings_root_pane.add_child( &sample_slider );

    VerticalPane sliders_pane ( 2 );
    settings_root_pane.add_child( &sliders_pane );

    /* Create all of the generated signals. */
    Wave carrier_wave = { 1.0, 0.0, 100.0};
    Wave cos_wave = { 1.0, M_PI / 2, 100.0 };
    Wave higher_wave = { 5.0, 0.0, 10.0 };

    Signal carrier ( graphs, sliders_pane, no_sample_points,
            &carrier_wave, {255, 255, 255}, font );

    Signal cos ( graphs, sliders_pane, no_sample_points, &cos_wave,
            {0, 255, 0}, font );

    std::vector<Signal*> signals;
    signals.push_back(&carrier);
    signals.push_back(&cos);
    std::vector<Signal*>::iterator it;

    class ButtonData {
      public:
        SDL_WindowRenderer &r;
        Pane &p;
        int &samples;
        TTF_Font *font;
        std::vector<Signal*> &signals;
        ButtonData(SDL_WindowRenderer &r, Pane &p, int &samples,
                        TTF_Font *font,std::vector<Signal*> &signals)
                : r { r }, p { p }, samples { samples }, font { font },
                  signals { signals } {}
    };
    ButtonData *new_data = new ButtonData( graphs, sliders_pane,
            no_sample_points, font, signals );
    Button add_signal_btn ( SCREEN_WIDTH - 20, 50, "Add Signal", font,
            [](void *dat) {
                ButtonData *data = (ButtonData *)dat;;
                Wave *new_wave = new Wave(1.0, 0.0, 100.0);
                Signal *new_signal = new Signal(data->r, data->p,
                        data->samples, new_wave, {0, 0, 255}, data->font );
                data->signals.push_back(new_signal);
                return (void *)NULL;
             }, (void *)new_data);
    settings_root_pane.add_child( &add_signal_btn );

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

        settings_root_pane.render( settings.get_renderer(), 10, 10 );
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
