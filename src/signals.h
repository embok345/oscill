#ifndef SIGNALS_H
#define SIGNALS_H

#include <vector>

#include <SDL2/SDL.h>
#include "consts.h"
#include "sdl_util.h"
#include "layout/Button.h"
#include "layout/Slider.h"
#include "layout/Pane.h"
#include "layout/VerticalPane.h"
#include "layout/HorizontalPane.h"

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
    SDL_WindowRenderer& settings_window;
    Pane* root_settings_pane;
    Pane* signal_settings_pane;
    Pane* wave_settings_pane;
    std::vector<Wave *> waves;
    TTF_Font *font;

  protected:
    SDL_Colour c;
    int no_points;
    std::vector<SDL_Point> points;

  private:

    Signal( SDL_WindowRenderer &signal_window,
               SDL_WindowRenderer &settings_window, Pane *root_settings_pane,
               int no_points, std::vector<Wave *> waves, SDL_Colour c,
               TTF_Font *font )
            : signal_window { signal_window },
              settings_window { settings_window },
              root_settings_pane { root_settings_pane }, waves { waves },
              c { c },no_points { no_points }, font { font },
              points(no_points,{0,0}) {
        create_sliders( );
    }

    void create_sliders( );

  public:

    Signal( SDL_WindowRenderer& signal_window,
               SDL_WindowRenderer& settings_window, Pane *root_settings_pane,
               int no_points, Wave *wave, SDL_Colour c, TTF_Font *font )
            : signal_window { signal_window },
              settings_window { settings_window },
              root_settings_pane { root_settings_pane }, c { c },
              no_points { no_points }, font { font },
              points(no_points, {0,0}) {
        waves.push_back(wave);
        create_sliders( );
    }

    Signal( const Signal &s ) : Signal( s.signal_window, s.settings_window,
            s.root_settings_pane, s.no_points, s.waves, s.c, s.font ) { }

    virtual ~Signal() {
        std::vector<Wave*>::iterator it = waves.begin();
        while( it != waves.end() ) {
            delete *it;
            it++;
        }
    }

    virtual SDL_Point update_point( int, time_t, int );

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

    virtual SDL_Point update_point( int, time_t, int ) override;
};

#endif
