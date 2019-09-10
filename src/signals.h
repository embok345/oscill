#ifndef SIGNALS_H
#define SIGNALS_H

#include <vector>

#include <SDL2/SDL.h>
#include "consts.h"
#include "sdl_util.h"
#include "slider.h"
#include "elements.h"

class Wave {

  public:

    double frequency;
    double phase;
    double amplitude;

    Wave(double frequency, double phase, double amplitude)
        : frequency { frequency }, phase { phase }, amplitude { amplitude } {}
};

class Signal {

    SDL_WindowRenderer& signal_window;
    Pane& root_settings_pane;
    Pane* signal_settings_pane;
    std::vector<Wave *> waves;

  protected:
    SDL_Point *points;
    SDL_Colour c;
    int no_points;

  private:

    Signal( SDL_WindowRenderer& signal_window, Pane &root_settings_pane,
               int no_points, std::vector<Wave *> waves, SDL_Colour c,
               TTF_Font *font )
            : signal_window { signal_window },
              root_settings_pane { root_settings_pane }, waves { waves },
              c { c },no_points { no_points } {
        points = new SDL_Point[no_points];
        create_sliders( font );
    }

    void create_sliders( TTF_Font *font );

  public:

    Signal( SDL_WindowRenderer& signal_window, Pane& root_settings_pane,
               int no_points, Wave *wave, SDL_Colour c, TTF_Font *font )
            : signal_window { signal_window },
              root_settings_pane { root_settings_pane }, c { c },
              no_points { no_points } {
        points = new SDL_Point[no_points];
        waves.push_back(wave);
        create_sliders( font );
    }

    Signal( const Signal &s ) : Signal( s.signal_window, s.root_settings_pane,
            s.no_points, s.waves, s.c, NULL ) { }

    virtual SDL_Point update_point( int, time_t );

    void add_wave(Wave *);
    void draw_line( int, int );

    friend Signal operator+(const Signal &, const Signal &);
    friend Signal operator+(const Signal &, Wave *);
    friend Signal operator+(Wave *, const Signal &);

};

class ModulatedSignal: public Signal {

    double *modulation_sensitivity;
    double *modulation_frequency;
    double *modulation_phase;

  public:
    ModulatedSignal( Signal s, double *sensitivity, double *frequency,
                        double *phase, SDL_Colour c )
            : Signal( s ), modulation_sensitivity { sensitivity },
            modulation_frequency { frequency }, modulation_phase { phase } {
        Signal::c = c;
    }

    virtual SDL_Point update_point( int, time_t ) override;
};

#endif
