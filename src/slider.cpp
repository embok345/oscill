#include "slider.h"
#include "sdl_util.h"

/* Create a slider with the given position and size.
 * ----------------------------------------------------------------------------
 * SDL_Renderer *r    -- The renderer to draw the slider with.
 * int x              -- The x coordinate of the top left corner of the slider.
 * int y              -- The y cooridnate of the top left corner of the slider.
 * int w              -- The width of the slider.
 * int h              -- The height of the slider.
 * double start_state -- The initial position of the slider, from 0 to 1,
 *                         0 being empty, 1 being full.
 * double *bind       -- The variable to bind the slider to.
 * double (*)(double) -- The function to calculate the new value of the bound
 *                         value, given some proportion.
 */
SDL_Slider::SDL_Slider( SDL_Renderer *r, int x, int y, int w, int h,
                           double *bind, double (*calculate_value)(double),
                           double start_state, TTF_Font *font,
                           const char *label) {
    this->r = r;
    this->calculate_value = calculate_value;
    bound_value = bind;
    proportion = start_state;

    /* Create the boundary box of the slider. */
    boundary.x = x;
    boundary.y = y;
    boundary.w = w;
    boundary.h = h;
    SDL_SetRenderDrawColor( this->r, 255, 255, 255, SDL_ALPHA_OPAQUE );
    SDL_RenderDrawRect( this->r, &(boundary) );

    /* Create the background fill of the slider. */
    background.x = x + 1;
    background.y = y + 1;
    background.w = w - 2;
    background.h = h - 2;
    SDL_SetRenderDrawColor( this->r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( this->r, &(background) );

    /* Create the slider itself. */
    slider.x = x + 1;
    slider.y = y + 1;
    /* If the start state is out of bounds, set it to be minimal or maximal. */
    if(start_state < 0) start_state = 0;
    else if(start_state > 1) start_state = 1;
    slider.w = start_state * (w - 2);
    slider.h = h - 2;
    SDL_SetRenderDrawColor( this->r, 200, 200, 200, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( this->r, &(slider) );

    this->label = { font, label };
    if( font && label ) {
        render_text( r, font, label, x + (w / 2), y + (h / 2), true );
    }

    SDL_AddEventWatch( SDL_Slider::slider_event_filter, this );
}

/* Update the proportion of the width occupied by the slider.
 * ----------------------------------------------------------------------------
 * double proportion         -- The new proportion of the width the slider
 *                                should take up, from 0 to 1.
 */
void SDL_Slider::update_proportion(double new_proportion) {
    /* If the proportion is too small (resp. too big), set it to be the minimal
     * (resp. maximal) value. */
    if(new_proportion < 0) proportion = 0;
    else if(new_proportion > 1) proportion = 1;
    else proportion = new_proportion;

    /* Redraw the background of the slider. */
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &background);

    /* Update the width of the slider, and redraw it. */
    slider.w = proportion * background.w;
    SDL_SetRenderDrawColor( r, 200, 200, 200, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &slider );

    if(label.font && label.label) {
        render_text( r, label.font, label.label, boundary.x + (boundary.w / 2),
                boundary.y + (boundary.h / 2), true );
    }

    *bound_value = calculate_value(proportion);
}

int SDL_Slider::slider_event_filter(void *userdata, SDL_Event *e) {

    SDL_Slider *self = (SDL_Slider *)userdata;

    if( e->type == SDL_MOUSEBUTTONUP ) self->handle_mouseclick(e->button);

    if( e->type == SDL_MOUSEWHEEL ) self->handle_mousewheel(e->wheel);

    return 0;
}

void SDL_Slider::handle_mouseclick(SDL_MouseButtonEvent e) {
    SDL_Point clicked_point = { e.x, e.y };

    if( !SDL_PointInRect( &clicked_point, &boundary ) ) return;

    double new_proportion = (clicked_point.x - boundary.x) /
            (double)background.w;

    update_proportion(new_proportion);
}

void SDL_Slider::handle_mousewheel(SDL_MouseWheelEvent e) {
    SDL_Point mouse_location;
    SDL_GetMouseState(&(mouse_location.x), &(mouse_location.y));

    if( !SDL_PointInRect( &mouse_location, &boundary ) ) return;

    if(e.y == 0) return;

    update_proportion( proportion + (0.01 * e.y) );
}
