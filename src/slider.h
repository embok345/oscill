#ifndef SLIDER_H
#define SLIDER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


/* A class to represent a slider control. */
class SDL_Slider {
    SDL_Renderer *r;
    SDL_Rect boundary;
    SDL_Rect background;
    SDL_Rect slider;
    double proportion;
    double *bound_value;
    double (*calculate_value)(double);
    struct { TTF_Font *font; const char *label; } label;

    void update_proportion(double);
    void handle_mouseclick(SDL_MouseButtonEvent);
    void handle_mousewheel(SDL_MouseWheelEvent);

    static int slider_event_filter(void *, SDL_Event *);
  public:
    SDL_Slider( SDL_Renderer *r, int x, int y, int w, int h, double *bind,
                   double (*calculate_value)(double),
                   double start_state = 0.5,  TTF_Font *font = NULL,
                   const char *label = NULL);
};

#endif
