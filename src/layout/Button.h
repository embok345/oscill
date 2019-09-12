#ifndef BUTTON_H
#define BUTTON_H

#include "Child.h"
#include "Label.h"

class Button : public Child {

    SDL_Rect background;

    void (*callback)(void *data);
    void *data;
    int listening_window;

    Label label;
    SDL_Colour c;

    static int button_event_filter( void *, SDL_Event * );

  public:

    Button( int w, int h, std::string text, TTF_Font *font,
               void (*callback)(void *), void *data, int listening_window,
               SDL_Colour c = { 255, 0, 0 });

    virtual void render( SDL_Renderer *, int, int ) override;

};

#endif
