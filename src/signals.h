#ifndef SIGNALS_H
#define SIGNALS

#include <vector>

#include <SDL2/SDL.h>
#include "consts.h"
#include "sdl_util.h"
#include "slider.h"

struct Wave {
    double frequency;
    double phase;
    double amplitude;
};

class Signal {

    SDL_WindowRenderer& signal_window;
    VerticalPane& settings_window;
    std::vector<Wave *> waves;

    SDL_Slider<double> *frequency = NULL;
    SDL_Slider<double> *amplitude = NULL;
    SDL_Slider<SDL_Colour> *colour = NULL;

  protected:
    SDL_Point *points;
    SDL_Colour c;
    int no_points;

  private:

    Signal( SDL_WindowRenderer& signal_window,
               VerticalPane& settings_window, int no_points,
               std::vector<Wave *> waves, SDL_Colour c, TTF_Font *font )
            : signal_window { signal_window },
              settings_window { settings_window }, waves { waves },
              c { c },no_points { no_points } {
        points = new SDL_Point[no_points];
        create_sliders( font, settings_window.get_used_height() );
    }

    void create_sliders( TTF_Font *, int );

  public:

    Signal( SDL_WindowRenderer& signal_window,
               VerticalPane& settings_window, int no_points, Wave *wave,
               SDL_Colour c, TTF_Font *font )
            : signal_window { signal_window },
              settings_window { settings_window }, c { c },
              no_points { no_points } {
        points = new SDL_Point[no_points];
        waves.push_back(wave);
        create_sliders( font, settings_window.get_used_height() );
    }

    Signal( const Signal &s ) : Signal( s.signal_window, s.settings_window,
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
