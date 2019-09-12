#include "HorizontalPane.h"

HorizontalPane::HorizontalPane( int h_gap ) : h_gap { h_gap } {}

void HorizontalPane::add_child( Child *c ) {
    Pane::add_child( c );
    w += c->get_width() + h_gap;
    if( c->get_height() > h ) h = c->get_height();
}

void HorizontalPane::render( SDL_Renderer *r, int x, int y ) {
    std::vector<Child*>::iterator it = children.begin();
    w = 0;
    while(it != children.end()) {
        (*it)->render( r, x + w, y );
        w += (*it)->get_width() + h_gap;
        if((*it)->get_height() > h) h = (*it)->get_height();
        it++;
    }
}
