#ifndef HPANE_H
#define HPANE_H

#include "Child.h"
#include "Pane.h"

class HorizontalPane : public Pane {
   int h_gap = 0;

  public:

    HorizontalPane( int h_gap = 0 );

    virtual void add_child( Child *c ) override;

    virtual void render( SDL_Renderer *r, int x, int y ) override;
};

#endif
