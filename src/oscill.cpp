#include "oscill.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <iterator>
#include <vector>

void set_scene( SDL_WindowRenderer& r ) {

    /* Clear the screen. */
    SDL_SetRenderDrawColor( r.get_renderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear( r.get_renderer() );

    /* Draw the top and bottom guide lines. */
    SDL_SetRenderDrawColor( r.get_renderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine( r.get_renderer(), 0, ZERO_LEVEL - MAX_AMPLITUDE,
            SCREEN_WIDTH, ZERO_LEVEL - MAX_AMPLITUDE );
    SDL_RenderDrawLine( r.get_renderer(), 0, ZERO_LEVEL + MAX_AMPLITUDE,
            SCREEN_WIDTH, ZERO_LEVEL + MAX_AMPLITUDE );

    /* Draw the center guide line. */
    SDL_SetRenderDrawColor( r.get_renderer(), 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine( r.get_renderer(), 0, ZERO_LEVEL, SCREEN_WIDTH,
            ZERO_LEVEL );
    /* Draw a black line, otherwise the alpha goes weird.
     * TODO i'm sure there is a correct way to make sure the blending
     * doesn't go weird. */
    SDL_SetRenderDrawColor( r.get_renderer(), 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderDrawLine( r.get_renderer(), 0, 0, 2, 0 );
}

int main(int argc, char * args[]) {

    srand(time(NULL));

    /* Try to initialise SDL. */
    if( ! init( ) )
        return 1;

    auto graphs = new SDL_WindowRenderer( SCREEN_WIDTH, SCREEN_HEIGHT,
            "oscill" );
    auto settings = new SDL_WindowRenderer( SCREEN_WIDTH, SCREEN_HEIGHT,
            "Settings" );

    int x, y;
    SDL_GetWindowPosition( graphs->get_window(), &x, &y );
    SDL_SetWindowPosition( settings->get_window(), x + SCREEN_WIDTH, y );

    TTF_Font *font;
    if( !( font = get_font( ) ) )
        return 1;

    /* The number of samples to store. */
    int no_sample_points = 100;
    /* The rate at which the wave is sampled; samples per second. */
    int sample_rate = 50;
    /* The total time elapsed (not really). */
    time_t t = 0;
    /* The number of pixels per second horizontally. */
    int horizontal_scale = 100;

    auto settings_root_pane = new VerticalPane( 30 );

    auto sample_slider = new Slider<int>( SCREEN_WIDTH - 20, 40, &sample_rate,
            [](double p) {
                return (int)average( MIN_SAMPLE_RATE, MAX_SAMPLE_RATE, p );
            }, SDL_GetWindowID( settings->get_window() ), 0.5, "sample rate",
            font );
    auto horizontal_scale_slider = new Slider<int>( SCREEN_WIDTH - 20, 40,
            &horizontal_scale,
            [](double p) {
                return (int)log_average( MIN_HORIZONTAL_SCALE,
                        MAX_HORIZONTAL_SCALE, 10, p );
            }, SDL_GetWindowID( settings->get_window() ), 0.5, "scale", font );

    settings_root_pane->add_child( sample_slider );
    settings_root_pane->add_child( horizontal_scale_slider );

    auto sliders_pane = new VerticalPane( 20 );
    settings_root_pane->add_child( sliders_pane );

    /* Create all of the generated signals. */
    auto carrier_wave = new Wave(1.0, 0.0, 100.0);

    auto carrier = new Signal( *graphs, *settings, sliders_pane,
            no_sample_points, carrier_wave, {255, 255, 255}, font );

    std::vector<Signal*> signals;
    signals.push_back(carrier);
    std::vector<Signal*>::iterator it;

    class ButtonData {
      public:
        SDL_WindowRenderer &graph;
        SDL_WindowRenderer &set;
        Pane *p;
        int &samples;
        TTF_Font *font;
        std::vector<Signal*> &signals;
        ButtonData( SDL_WindowRenderer &graph, SDL_WindowRenderer &set,
                       Pane *p, int &samples, TTF_Font *font,
                       std::vector<Signal*> &signals )
                : graph { graph }, set { set },  p { p }, samples { samples },
                  font { font }, signals { signals } {}
    };
    auto new_data = new ButtonData( *graphs, *settings, sliders_pane,
            no_sample_points, font, signals );
    auto add_signal_btn = new Button( SCREEN_WIDTH - 20, 50, "Add Signal",
            font,
            [](void *dat) {
                ButtonData *data = (ButtonData *)dat;;
                Wave *new_wave = new Wave(1.0, 0.0, 100.0);
                Signal *new_signal = new Signal(data->graph, data->set,
                        data->p, data->samples, new_wave, {0, 0, 255},
                        data->font );
                data->signals.push_back(new_signal);
            }, (void *)new_data, SDL_GetWindowID( settings->get_window() ),
            { 200, 200, 200 } );

    settings_root_pane->add_child( add_signal_btn );

    /* Generate the initial state of the signals. */
    for(int i = 0; i < no_sample_points; i++) {
        it = signals.begin();
        while( it != signals.end() ) {
            (*it)->update_point( i, t, horizontal_scale);
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

        set_scene( *graphs );

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
            (*it)->update_point( start_index, t, horizontal_scale );
            ++it;
        }

        /* Draw the current render. */
        SDL_RenderPresent( graphs->get_renderer() );

        SDL_SetRenderDrawColor( settings->get_renderer(), 255, 255, 255,
                SDL_ALPHA_OPAQUE );
        SDL_RenderClear( settings->get_renderer() );
        settings_root_pane->render( settings->get_renderer(), 10, 10 );
        SDL_RenderPresent( settings->get_renderer() );

        /* Update the total time, and wait the sample time. */
        start_index = (start_index + 1) % no_sample_points;
        t += MS_PER_S / sample_rate;
        SDL_Delay( MS_PER_S / sample_rate );

    }

    /* Once the application has finished, clean up the SDL resources. */

    delete settings_root_pane;
    delete new_data;
    it = signals.begin();
    while( it != signals.end() ) {
        delete *it;
        it++;
    }

    TTF_CloseFont(font);

    delete graphs;
    delete settings;

    close( );

    return 0;
}
