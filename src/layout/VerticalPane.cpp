#include "VerticalPane.h"

VerticalPane::VerticalPane( int v_gap ) : v_gap { v_gap } {}

void VerticalPane::add_child( Child *c ) {
    Pane::add_child( c );
    h += c->get_height() + v_gap;
    if( c->get_width() > w ) w = c->get_width();
}

void VerticalPane::render( SDL_Renderer *r, int x, int y ) {
    std::vector<Child*>::iterator it = children.begin();
    this->h = 0;
    this->w = 0;
    while(it != children.end()) {
        (*it)->render( r, x, y + this->h );
        this->h += (*it)->get_height() + v_gap;
        if((*it)->get_width() > this->w) this->w = (*it)->get_width();
        it++;
    }
}
