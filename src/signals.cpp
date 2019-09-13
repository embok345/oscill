#include "signals.h"r
#include <algorithm>

SDL_Point Signal::update_point( int index, time_t t, int scale ) {
    index %= no_points;
    points[index].x = ( t / MS_PER_S ) * scale;
    double y = 0;
    unsigned int arr_len = std::size( waves );

    for( unsigned int i = 0; i < arr_len; i++) {

        if( waves[i]->frequency != waves[i]->old_frequency ) {
            waves[i]->phase = (M_TAU * ( t / MS_PER_S ) *
                    ( waves[i]->old_frequency - waves[i]->frequency )) +
                    waves[i]->phase;
            waves[i]->phase -= floor( waves[i]->phase / M_TAU ) * M_TAU;
            waves[i]->old_frequency = waves[i]->frequency;
        }

        y += waves[i]->amplitude *
                sin( ( M_TAU * waves[i]->frequency * ( t / MS_PER_S ) ) +
                waves[i]->phase);
    }
    points[index].y = y;
    return points[index];
}

SDL_Point ModulatedSignal::update_point( int index, time_t t, int scale ) {
    index %= no_points;
    Signal::update_point( index, t, scale );
    points[index].y *= (1 + ( *modulation_sensitivity *
            cos( ( M_TAU * *modulation_frequency * (t / MS_PER_S) ) +
            *modulation_phase ) ) );
    return points[index];
}

/* Draws the line between index and index + 1. */
void Signal::draw_line( int i, int opacity ) {

    SDL_SetRenderDrawColor( signal_window.get_renderer(), 0, 0, 0,
        SDL_ALPHA_OPAQUE );
    SDL_RenderDrawLine( signal_window.get_renderer(), 0, 0, 1, 1 );

    SDL_SetRenderDrawColor( signal_window.get_renderer(), c.r, c.g, c.b,
            opacity);
    i %= no_points;
    auto i2 = ( i + 1 == no_points ) ? 0 : i + 1;
    auto x1 = points[ i ].x % SCREEN_WIDTH;
    auto x2 = points[ i2 ].x % SCREEN_WIDTH;
    auto y1 = points[ i ].y;
    auto y2 = points[ i2 ].y;

    if( x1 <= x2 && (y1 < MAX_AMPLITUDE || y2 < MAX_AMPLITUDE) &&
            (y1 > -MAX_AMPLITUDE || y2 > -MAX_AMPLITUDE ) ) {

        if( y1 < MAX_AMPLITUDE && y2 > MAX_AMPLITUDE ) {
             x2 = x1 + (((MAX_AMPLITUDE - y1)/(double)(y2 - y1)) * (x2 - x1));
             y2 = MAX_AMPLITUDE;
        } else if( y1 > MAX_AMPLITUDE && y2 < MAX_AMPLITUDE ) {
             x1 = x2 - (((MAX_AMPLITUDE - y2)/(double)(y1 - y2)) * (x2 - x1));
             y1 = MAX_AMPLITUDE;
        } else if( y1 > -MAX_AMPLITUDE && y2 < -MAX_AMPLITUDE ) {
             x2 = x2 - (((-MAX_AMPLITUDE - y2)/(double)(y1 - y2)) * (x2 - x1));
             y2 = -MAX_AMPLITUDE;
        } else if( y1 < -MAX_AMPLITUDE && y2 > -MAX_AMPLITUDE ) {
             x1 = x1 + (((-MAX_AMPLITUDE - y1)/(double)(y2 - y1)) * (x2 - x1));
             y1 = -MAX_AMPLITUDE;
        }

        SDL_RenderDrawLine( signal_window.get_renderer(), x1,
                ZERO_LEVEL - y1, x2, ZERO_LEVEL - y2 );

    }
    SDL_SetRenderDrawColor( signal_window.get_renderer(), 0, 0, 0,
            SDL_ALPHA_OPAQUE );
}

void Signal::add_wave(Wave *w) {
    waves.push_back(w);
    add_slider( w );
}

void Signal::remove_wave( Wave *w, Pane *p ) {
    if( waves.size() <= 1 ) return;
    waves.erase(std::remove( waves.begin(), waves.end(), w ), waves.end());
    wave_settings_pane->remove_child( p );
}

void Signal::create_sliders( ) {

    signal_settings_pane = new VerticalPane ( 5 );
    wave_settings_pane = new VerticalPane ( 5 );

    std::vector<Wave*>::iterator it = waves.begin();
    while( it != waves.end() ) {
        add_slider( *it );
        it++;
    }

    auto other_options_pane = new HorizontalPane( 5 );
    auto add_wave_btn = new Button( ( SCREEN_WIDTH / 2 ) - 20, 40, "Add wave",
            font,
            [](void *dat) {
                Signal *self = (Signal *)dat;
                auto new_wave = new Wave(1.0, 0.0, 20.0);
                self->add_wave(new_wave);
            }, (void *)this, SDL_GetWindowID(settings_window.get_window()),
            { 200, 200, 200 } );
    auto colour_slider = new Slider<SDL_Colour>( ( SCREEN_WIDTH / 2) - 20, 40,
            &c, colour_proportion,
            SDL_GetWindowID( settings_window.get_window() ), 0.5, "", font);
    other_options_pane->add_child( add_wave_btn );
    other_options_pane->add_child( colour_slider );

    signal_settings_pane->add_child( wave_settings_pane );
    signal_settings_pane->add_child( other_options_pane );

    root_settings_pane->add_child( signal_settings_pane );
}

void Signal::add_slider( Wave *w ) {

    int window_id = SDL_GetWindowID( settings_window.get_window() );

    auto frequency_slider = new Slider<double>( (SCREEN_WIDTH / 2) - 30,
            40, &(w->frequency),
            [](double p) {
                auto new_freq =
                        log_average( MIN_FREQUENCY, MAX_FREQUENCY, 10, p);
                return new_freq;
            }, window_id, 0.5, "frequency", font );

    auto amplitude_slider = new Slider<double>( (SCREEN_WIDTH / 2) - 30,
            40, &(w->amplitude),
            [](double p) { return average( 20, MAX_AMPLITUDE, p ); },
            window_id, 0.5, "amplitude", font );

    auto wave_pane = new HorizontalPane( 5 );

    struct btn_data {
        Wave *w;
        Pane *p;
        Signal *self;
    };
    auto data = new btn_data { w, wave_pane, this };
    auto remove_wave_btn = new Button( 30, 40, "X", font,
            [](void *dat) {
                auto data = (btn_data*)dat;
                data->self->remove_wave( data->w, data->p );
            }, (void *)data, window_id );

    wave_pane->add_child( frequency_slider );
    wave_pane->add_child( amplitude_slider );
    wave_pane->add_child( remove_wave_btn );

    wave_settings_pane->add_child( wave_pane );
}

Signal operator+(Wave *w, const Signal &s) {
    return s + w;
}

Signal operator+(const Signal &s, Wave *w ) {
    Signal ret ( s );
    ret.add_wave( w );
    return ret;
}

Signal operator+(const Signal &s1, const Signal &s2) {
    Signal ret ( s1 );
    auto no_frequencies = std::size(s2.waves);
    for(unsigned int i = 0; i < no_frequencies; i++) {
        ret.add_wave( s2.waves[i] );
    }

    ret.c = { (uint8_t)((s1.c.r + s2.c.r) / 2),
            (uint8_t)((s1.c.g + s2.c.g) / 2),
            (uint8_t)((s1.c.b + s2.c.b) / 2) };

    return ret;
}
