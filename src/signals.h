#ifndef SIGNALS_H
#define SIGNALS

#include <vector>

#include <SDL2/SDL.h>
#include "consts.h"

struct Wave {
    double frequency;
    double phase;
    double amplitude;
};

class Signal {

    SDL_Renderer *r;

    std::vector<double *> frequencies;
    std::vector<double *> phases;
    std::vector<double *> amplitudes;

  protected:
    SDL_Point *points;
    SDL_Color c;
    int no_points;

  private:
    Signal( SDL_Renderer *r, int no_points, std::vector<double *> frequencies, std::vector<double *> phases, std::vector<double *> amplitudes, SDL_Color c) : r { r }, frequencies { frequencies }, phases { phases }, amplitudes { amplitudes }, c { c }, no_points { no_points } {
        points = new SDL_Point[this->no_points];
    }

  public:

    Signal(SDL_Renderer *r, int no_points, double *frequency, double *phase, double *amplitude, SDL_Color c) : r { r }, c { c }, no_points { no_points } {
        points = new SDL_Point[this->no_points];
        frequencies.push_back(frequency);
        phases.push_back(phase);
        amplitudes.push_back(amplitude);
    }

    Signal( const Signal &s ) : Signal( s.r, s.no_points, s.frequencies, s.phases, s.amplitudes, s.c ) {}

    virtual SDL_Point update_point( int, time_t );

    void add_wave( double *, double *, double * );

    void draw_line( int, int );

    friend Signal operator+(const Signal &s1, const Signal &s2);

};

class ModulatedSignal: public Signal {

    double *modulation_sensitivity;
    double *modulation_frequency;
    double *modulation_phase;

  public:
    ModulatedSignal( Signal s, double *sensitivity, double *frequency, double *phase, SDL_Color c ) : Signal( s ), modulation_sensitivity { sensitivity }, modulation_frequency { frequency }, modulation_phase { phase } {
        this->c = c;
    }

    virtual SDL_Point update_point( int, time_t ) override;

};

#endif
