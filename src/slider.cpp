#include "slider.h"

template <class T>
SDL_Slider<T>::SDL_Slider( int x) {
    this->x = x;
}

/* Create a slider with the given position and size.
 * ----------------------------------------------------------------------------
 * SDL_Renderer *r    -- The renderer to draw the slider with.
 * int x              -- The x coordinate of the top left corner of the slider.
 * int y              -- The y cooridnate of the top left corner of the slider.
 * int w              -- The width of the slider.
 * int h              -- The height of the slider.
 * double start_state -- The initial position of the slider, from 0 to 1,
 *                         0 being empty, 1 being full.
 * T *bind            -- The variable to bind the slider to.
 */
/*template <class T>
SDL_Slider<T>::SDL_Slider( SDL_Renderer *r,
                           int x,
                           int y,
                           int w,
                           int h,
                           double start_state,
                           T *bind,
                           T (*proportion)(double)) {
    this->r = r;
    this->proportion = proportion;

    /* Create the boundary box of the slider. 
    boundary.x = x;
    boundary.y = y;
    boundary.w = w;
    boundary.h = h;
    SDL_SetRenderDrawColor( this->r, 255, 255, 255, SDL_ALPHA_OPAQUE );
    SDL_RenderDrawRect( this->r, &(boundary) );

    /* Create the background fill of the slider. 
    background.x = x + 1;
    background.y = y + 1;
    background.w = w - 2;
    background.h = h - 2;
    SDL_SetRenderDrawColor( this->r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( this->r, &(background) );

    /* Create the slider itself. 
    slider.x = x + 1;
    slider.y = y + 1;
    /* If the start state is out of bounds, set it to be minimal or maximal. 
    if(start_state < 0) start_state = 0;
    else if(start_state > 1) start_state = 1;
    slider.w = start_state * (w - 2);
    slider.h = h - 2;
    SDL_SetRenderDrawColor( this->r, 200, 200, 200, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( this->r, &(slider) );

    bound_value = bind;
}*/

/* Update the proportion of the width occupied by the slider.
 * ----------------------------------------------------------------------------
 * double proportion         -- The new proportion of the width the slider
 *                                should take up, from 0 to 1.
 */
template <class T>
void SDL_Slider<T>::update_proportion(double proportion) {
    /* If the proportion is too small (resp. too big), set it to be the minimal
     * (resp. maximal) value. */
    if(proportion < 0) proportion = 0;
    else if(proportion > 1) proportion = 1;

    /* Redraw the background of the slider. */
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &background);

    /* Update the width of the slider, and redraw it. */
    slider.w = proportion * background.w;
    SDL_SetRenderDrawColor( r, 200, 200, 200, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &slider );
}
