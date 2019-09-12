#ifndef VPANE_H
#define VPANE_H

#include "Child.h"
#include "Pane.h"

class VerticalPane : public Pane {

    int v_gap = 0;

  public:

    VerticalPane( int v_gap = 0 );

    virtual void add_child( Child *c ) override;

    virtual void render( SDL_Renderer *r, int x, int y ) override;
};

#endif
