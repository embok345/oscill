#ifndef SLIDER_H
#define SLIDER_H
#include <SDL2/SDL.h>

/* A struct to represent a slider control. */
template <class T>
class SDL_Slider {
    SDL_Renderer *r;
    SDL_Rect boundary;
    SDL_Rect background;
    SDL_Rect slider;
    T *bound_value;
    T proportion(double proportion);
    void update_proportion(double);
  public:
    SDL_Slider( int );
    SDL_Slider( SDL_Renderer *, int, int, int, int, int,
                   T *, T (*)(double) ) { }
    SDL_Slider( SDL_Renderer *, int, int, int, int, double,
                   T *, T(*)(double) ) { };
};

#endif
