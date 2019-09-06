#include "signals.h"

SDL_Point Signal::update_point( int index, time_t t ) {
    index %= no_points;
    points[index].x = ( t / MS_PER_S ) * PX_PER_S;
    double y = 0;
    unsigned int arr_len = std::size( frequencies );
    for( unsigned int i = 0; i < arr_len; i++) {
        y += *(amplitudes[i]) * sin( ( M_TAU * *(frequencies[i]) *
                ( t / MS_PER_S ) ) + *(phases[i]) );
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
    SDL_SetRenderDrawColor( r, c.r, c.g, c.b, opacity);
    i %= no_points;
    auto i2 = ( i + 1 == no_points ) ? 0 : i + 1;
    auto x1 = points[ i ].x % SCREEN_WIDTH;
    auto x2 = points[ i2 ].x % SCREEN_WIDTH;
    auto y1 = points[ i ].y;
    auto y2 = points[ i2 ].y;
    if( x1 <= x2 && abs(y1) < MAX_AMPLITUDE && abs(y2) < MAX_AMPLITUDE ) {
        SDL_RenderDrawLine( r, x1, ZERO_LEVEL - points[ i ].y, x2,
                ZERO_LEVEL - points[ i2 ].y );
    }
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
}

void Signal::add_wave( double *frequency, double *phase, double *amplitude ) {
    frequencies.push_back( frequency );
    phases.push_back( phase );
    amplitudes.push_back( amplitude );
}

Signal operator+(const Signal &s1, const Signal &s2) {
    Signal ret ( s1 );
    auto no_frequencies = std::size(s2.frequencies);
    for(unsigned int i = 0; i < no_frequencies; i++) {
        ret.add_wave( s2.frequencies[i], s2.phases[i], s2.amplitudes[i] );
    }

    ret.c = { (uint8_t)((s1.c.r + s2.c.r) / 2),
            (uint8_t)((s1.c.g + s2.c.g) / 2),
            (uint8_t)((s1.c.b + s2.c.b) / 2) };

    return ret;
}
