#include "signals.h"

SDL_Point Signal::update_point( int index, time_t t ) {
    index %= no_points;
    points[index].x = ( t / MS_PER_S ) * PX_PER_S;
    double y = 0;
    unsigned int arr_len = std::size( waves );
    for( unsigned int i = 0; i < arr_len; i++) {
        y += waves[i]->amplitude *
                sin( ( M_TAU * waves[i]->frequency * ( t / MS_PER_S ) ) +
                waves[i]->phase);
    }
    points[index].y = y;
    return points[index];
}

SDL_Point ModulatedSignal::update_point( int index, time_t t ) {
    index %= no_points;
    Signal::update_point( index, t );
    points[index].y *= (1 + ( *modulation_sensitivity *
            cos( ( M_TAU * *modulation_frequency * (t / MS_PER_S) ) +
            *modulation_phase ) ) );
    return points[index];
}

/* Draws the line between index and index + 1. */
void Signal::draw_line( int i, int opacity ) {
    SDL_SetRenderDrawColor( signal_window.get_renderer(), c.r, c.g, c.b,
            opacity);
    i %= no_points;
    auto i2 = ( i + 1 == no_points ) ? 0 : i + 1;
    auto x1 = points[ i ].x % SCREEN_WIDTH;
    auto x2 = points[ i2 ].x % SCREEN_WIDTH;
    auto y1 = points[ i ].y;
    auto y2 = points[ i2 ].y;
    if( x1 <= x2 && abs(y1) < MAX_AMPLITUDE && abs(y2) < MAX_AMPLITUDE ) {
        SDL_RenderDrawLine( signal_window.get_renderer(), x1,
                ZERO_LEVEL - points[ i ].y, x2, ZERO_LEVEL - points[ i2 ].y );
    }
    SDL_SetRenderDrawColor( signal_window.get_renderer(), 0, 0, 0,
            SDL_ALPHA_OPAQUE );
}

void Signal::add_wave(Wave *w) {
    waves.push_back(w);
}

void Signal::create_sliders( TTF_Font *font, int y ) {
    Wave *w = waves.front();

    frequency = new SDL_Slider<double>(settings_window, 10, y + 10,
            (SCREEN_WIDTH / 2) - 20, 40, &(w->frequency),
            [](double proportion) {
                return log_average( MIN_FREQUENCY, MAX_FREQUENCY, 10,
                        proportion);
            }, 0.5, font, "frequency" );

    amplitude = new SDL_Slider<double>( settings_window,
            (SCREEN_WIDTH / 2) + 10, y + 10, (SCREEN_WIDTH / 2) - 20, 40,
            &(w->amplitude),
            [](double proportion) {
                return average( 20, MAX_AMPLITUDE, proportion );
            }, 0.5, font, "amplitude" );

    colour = new SDL_Slider<SDL_Colour>( settings_window, 10, y + 60,
            SCREEN_WIDTH - 20, 40, &c, colour_proportion );

    settings_window.increase_height(100);

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
