#ifndef SLIDER_H
#define SLIDER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_util.h"

template <class T>
class SDL_Slider {
    SDL_WindowRenderer &r;
    SDL_Rect boundary;
    SDL_Rect background;
    SDL_Rect slider;
    double proportion;
    T *bound_value;
    T (*calculate_value)(double);
    struct { TTF_Font *font; const char *label; } label;

    void draw_label( ) {
        if(label.font && label.label) {
            render_outlined_text( r, label.font, label.label,
                boundary.x + (boundary.w / 2), boundary.y + (boundary.h / 2),
                true );
        }
    }

    void update_proportion(double new_proportion) {
        /* If the proportion is too small (resp. too big), set it to be the
         * minimal (resp. maximal) value. */
        if(new_proportion < 0) proportion = 0;
        else if(new_proportion > 1) proportion = 1;
        else proportion = new_proportion;

        /* Redraw the background of the slider. */
        SDL_SetRenderDrawColor( r.get_renderer(), 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r.get_renderer(), &background);

        /* Update the width of the slider, and redraw it. */
        slider.w = proportion * background.w;
        SDL_SetRenderDrawColor( r.get_renderer(), 200, 200, 200,
                SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r.get_renderer(), &slider );

        *bound_value = calculate_value(proportion);

        /* Rerender the text onto the slider. */
        draw_label( );
    }

    void handle_mouseclick(SDL_MouseButtonEvent e) {
        SDL_Point clicked_point = { e.x, e.y };

        if( !SDL_PointInRect( &clicked_point, &boundary ) ) return;

        double new_proportion = (clicked_point.x - boundary.x) /
                (double)background.w;

        update_proportion(new_proportion);
    }

    void handle_mousewheel(SDL_MouseWheelEvent e) {

        SDL_Point mouse_location;
        SDL_GetMouseState(&(mouse_location.x), &(mouse_location.y));

        if( !SDL_PointInRect( &mouse_location, &boundary ) ) return;

        if(e.y == 0) return;

        update_proportion( proportion + (0.01 * e.y) );
    }

    static int slider_event_filter(void *userdata, SDL_Event *e) {

        SDL_Slider *self = (SDL_Slider *)userdata;

        if( e->type == SDL_MOUSEBUTTONUP ) self->handle_mouseclick(e->button);

        if( e->type == SDL_MOUSEWHEEL ) self->handle_mousewheel(e->wheel);

        return 0;
    }
  public:
    SDL_Slider( SDL_WindowRenderer &r, int x, int y, int w, int h,
                    T *bind, T (*calculate_value)(double),
                    double start_state = 0.5,  TTF_Font *font = NULL,
                    const char *label = NULL) : r { r } {

        this->calculate_value = calculate_value;
        bound_value = bind;
        proportion = start_state;

        /* Create the boundary box of the slider. */
        boundary.x = x;
        boundary.y = y;
        boundary.w = w;
        boundary.h = h;
        SDL_SetRenderDrawColor( this->r.get_renderer(), 255, 255, 255,
                SDL_ALPHA_OPAQUE );
        SDL_RenderDrawRect( this->r.get_renderer(), &boundary );

        /* Create the background fill of the slider. */
        background.x = x + 1;
        background.y = y + 1;
        background.w = w - 2;
        background.h = h - 2;
        SDL_SetRenderDrawColor( this->r.get_renderer(), 0, 0, 0,
                SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( this->r.get_renderer(), &(background) );

        /* Create the slider itself. */
        slider.x = x + 1;
        slider.y = y + 1;
        /* If the start state is out of bounds, set it to be minimal or
           maximal. */
        if(start_state < 0) start_state = 0;
        else if(start_state > 1) start_state = 1;
        slider.w = start_state * (w - 2);
        slider.h = h - 2;
        SDL_SetRenderDrawColor( this->r.get_renderer(), 200, 200, 200,
                SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( this->r.get_renderer(), &(slider) );

        this->label = { font, label };
        draw_label( );

        SDL_AddEventWatch( SDL_Slider::slider_event_filter, this );

        update_proportion( proportion );
    }
};

#endif
