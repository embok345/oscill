#ifndef SLIDER_H
#define SLIDER_H
#include <SDL2/SDL.h>

/* A class to represent a slider control. */
class SDL_Slider {
    SDL_Renderer *r;
    SDL_Rect boundary;
    SDL_Rect background;
    SDL_Rect slider;
    double proportion;
    double *bound_value;
    double (*calculate_value)(double);

    void update_proportion(double);
    void handle_mouseclick(SDL_MouseButtonEvent);
    void handle_mousewheel(SDL_MouseWheelEvent);

    static int slider_event_filter(void *, SDL_Event *);
  public:
    SDL_Slider( SDL_Renderer *, int, int, int, int, double, double *,
                   double (*)(double) );
};

#endif
