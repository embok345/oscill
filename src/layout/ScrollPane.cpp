#include "ScrollPane.h"


void ScrollPane::handle_wheel( SDL_MouseWheelEvent e ) {
    SDL_Point mouse_location;
    SDL_GetMouseState( &(mouse_location.x), &(mouse_location.y) );
    SDL_Rect boundary = { x, y, w, h };

    if( ! SDL_PointInRect( &mouse_location, &boundary ) ) return;

    scroll( e.y * 10 );
}

int ScrollPane::scroll_event_filter( void *user_data, SDL_Event *e ) {
    ScrollPane *self = (ScrollPane*)user_data;

    if( ! ( e->type == SDL_MOUSEWHEEL ) ) return 1;

    self->handle_wheel( e->wheel );

    return 0;
}

ScrollPane::ScrollPane( int h, int w, Pane *layout_pane )
            : layout_pane { layout_pane } {
    Child::h = h;
    Child::w = w;
    SDL_AddEventWatch( ScrollPane::scroll_event_filter, this );
}

void ScrollPane::render( SDL_Renderer *r, int x, int y ) {

    SDL_Rect viewport = { x, y, w, h };
    SDL_RenderSetViewport( r, &viewport );
    layout_pane->render( r, x, - camera_y );

    this->x = x;
    this->y = y;

    SDL_RenderSetViewport( r, NULL );
}

void ScrollPane::add_child( Child *c ) {
    layout_pane->add_child( c );
}

void ScrollPane::scroll( int amount ) {
    if( amount < 0 && h + camera_y > layout_pane->get_height()) return;
    camera_y -= amount;
    if(camera_y < 0) camera_y = 0;
}
