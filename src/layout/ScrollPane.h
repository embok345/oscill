#ifndef SCROLLPANE_H
#define SCROLLPANE_H

#include "Pane.h"

class ScrollPane : public Pane {

    int camera_y = 0;
    Pane *layout_pane;

    int x = 0;
    int y = 0;

    void handle_wheel( SDL_MouseWheelEvent e );

    static int scroll_event_filter( void *user_data, SDL_Event *e );

  public:

    ScrollPane( int h, int w, Pane *layout_pane );

    virtual void render( SDL_Renderer *r, int x, int y ) override;

    virtual void add_child( Child *c ) override;

    void scroll( int amount );
};

#endif
