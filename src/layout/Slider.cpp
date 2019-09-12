#include "Slider.h"

template<>
void Slider<int>::update_proportion( double new_proportion ) {
    if(new_proportion < 0) new_proportion = 0;
    else if(new_proportion > 1) new_proportion = 1;

    proportion = new_proportion;

    *bound_value = calculate_value(proportion);

    value_label.update_text( std::to_string( *bound_value ) );
}

template<>
void Slider<double>::update_proportion( double new_proportion ) {
    if(new_proportion < 0) new_proportion = 0;
    else if(new_proportion > 1) new_proportion = 1;

    proportion = new_proportion;

    *bound_value = calculate_value(proportion);

    value_label.update_text( std::to_string( *bound_value ) );
}


template<>
void Slider<SDL_Colour>::render( SDL_Renderer *r, int x, int y ) {
    boundary.x = x;
    boundary.y = y;
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderDrawRect( r, &boundary );

    SDL_Rect slider { x + 1, y + 1, w - 2, h - 2 };
    SDL_SetRenderDrawColor( r, bound_value->r, bound_value->g, bound_value->b,
            SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &slider );
}

template<>
void Slider<SDL_Colour>::handle_mousewheel(SDL_MouseWheelEvent e) {
    SDL_Point mouse_location;
    SDL_GetMouseState(&(mouse_location.x), &(mouse_location.y));

    if( !SDL_PointInRect( &mouse_location, &boundary ) ) return;

    if(e.y == 0) return;

    update_proportion( proportion + ( e.y / (double)0xff ) );
}
