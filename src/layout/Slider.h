#ifndef SLIDER_H
#define SLIDER_H

#include "Child.h"
#include "Label.h"

template <class T>
class Slider : public Child {

    SDL_Rect boundary;

    double proportion;

    T *bound_value;
    T (*calculate_value)(double proportion);

    int listening_window;

    Label name_label;
    Label value_label;

    void update_proportion(double new_proportion) {
        if(new_proportion < 0) new_proportion = 0;
        else if(new_proportion > 1) new_proportion = 1;

        proportion = new_proportion;

        *bound_value = calculate_value(proportion);
    }

    void handle_mouseclick(SDL_MouseButtonEvent e) {
        SDL_Point clicked_point = { e.x, e.y };

        if( listening_window != e.windowID ||
                !SDL_PointInRect( &clicked_point, &boundary ) )
            return;

        double new_proportion = (clicked_point.x - boundary.x) /
                (double)boundary.w;

        update_proportion(new_proportion);
    }

    void handle_mousewheel(SDL_MouseWheelEvent e) {

        SDL_Point mouse_location;
        SDL_GetMouseState(&(mouse_location.x), &(mouse_location.y));

        if( listening_window != e.windowID ||
                !SDL_PointInRect( &mouse_location, &boundary ) )
            return;

        if(e.y == 0) return;

        update_proportion( proportion + (0.01 * e.y) );
    }

    static int slider_event_filter( void *user_data, SDL_Event *e ) {
        Slider *self = (Slider *)user_data;
        if(e->type == SDL_MOUSEBUTTONUP)
            self->handle_mouseclick( e->button );
        else if( e-> type == SDL_MOUSEWHEEL )
            self->handle_mousewheel( e->wheel );

        return 0;
    }

  public:

    Slider( int w, int h, T *bound_value, T (*calculate_value)(double),
               int listening_window, double start=0.5, std::string label="",
               TTF_Font *font=NULL )
            : proportion { start }, bound_value { bound_value },
              calculate_value { calculate_value },
              listening_window { listening_window },
              name_label { label, font, w / 2, h / 2 },
              value_label { "", font, w / 2, h / 2 } {
        Child::w = w;
        Child::h = h;

        boundary.w = w;
        boundary.h = h;
        boundary.x = 0;
        boundary.y = 0;

        SDL_AddEventWatch( slider_event_filter, this );

        update_proportion( proportion );
    }

    virtual void render( SDL_Renderer *r, int x, int y ) override {

        boundary.x = x;
        boundary.y = y;
        SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderDrawRect( r, &boundary );

        SDL_Rect background { x + 1, y + 1, w - 2, h - 2 };
        SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r, &background );

        SDL_Rect slider { x + 1, y + 1, (int)((w - 2) * proportion), h - 2 };
        SDL_SetRenderDrawColor( r, 200, 200, 200, SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r, &slider );

        name_label.render( r, x + 1 + ( w / 4.0 ), y + 1 + ( h / 2.0 ), true);
        value_label.render( r, x + 1 + (3 * ( w / 4.0 )),
                y + 1 + ( h / 2.0 ), true );
    }
};

#endif
