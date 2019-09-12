#ifndef CHILD_H
#define CHILD_H

#include "layout.h"

class Child {

  protected:

    int w = 0;
    int h = 0;

  public:

    virtual ~Child() { }

    virtual void render( SDL_Renderer *, int, int ) { }
    int get_width( ) { return w; }
    int get_height( ) { return h; }
};

#endif
