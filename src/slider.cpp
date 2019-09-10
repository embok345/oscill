//#include "slider.h"
#include "elements.h"

/*template<>
void Slider<int>::draw_label() {
    if(label.font && label.label) {
        auto text = new char[ strlen( label.label ) + 11 ];
        snprintf( text, strlen( label.label ) + 10, "%s: %d", label.label,
                *bound_value );
        render_outlined_text( r, label.font, text,
                boundary.x + (boundary.w / 2), boundary.y + (boundary.h / 2),
                true );
        delete[] text;
    }
}*/

/*template<>
void SDL_Slider<double>::draw_label() {
    if(label.font && label.label) {
        auto text = new char[ strlen( label.label ) + 11 ];
        snprintf( text, strlen( label.label ) + 10, "%s: %8lf", label.label,
                *bound_value );
        render_outlined_text( r, label.font, text,
                boundary.x + (boundary.w / 2), boundary.y + (boundary.h / 2),
                true );
        delete[] text;
    }
}*/

/*template<>
void SDL_Slider<SDL_Colour>::update_proportion(double new_proportion) {
    if(new_proportion < 0) proportion = 0;
    else if(new_proportion > 1) proportion = 1;
    else proportion = new_proportion;

    slider.w = background.w;

    *bound_value = calculate_value(proportion);

    SDL_SetRenderDrawColor( r.get_renderer(), bound_value->r, bound_value->g,
            bound_value->b, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r.get_renderer(), &slider );

    draw_label( );
}*/

template<>
void Slider<SDL_Colour>::render( SDL_Renderer *r, int x, int y ) {
    boundary.x = x;
    boundary.y = y;
    SDL_SetRenderDrawColor( r, 255, 255, 255, SDL_ALPHA_OPAQUE );
    SDL_RenderDrawRect( r, &boundary );

    /*SDL_Rect background { x + 1, y + 1, w - 2, h - 2 };
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &background );*/

    SDL_Rect slider { x + 1, y + 1, w - 2, h - 2 };
    SDL_SetRenderDrawColor( r, bound_value->r, bound_value->g, bound_value->b,
            SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &slider );

    label.render( r, x + 1, y + 1 );
}

template<>
void Slider<SDL_Colour>::handle_mousewheel(SDL_MouseWheelEvent e) {
    SDL_Point mouse_location;
    SDL_GetMouseState(&(mouse_location.x), &(mouse_location.y));

    if( !SDL_PointInRect( &mouse_location, &boundary ) ) return;

    if(e.y == 0) return;

    update_proportion( proportion + ( e.y / (double)0xff ) );
}
