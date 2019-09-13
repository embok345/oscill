#ifndef BUTTON_H
#define BUTTON_H

#include "Child.h"
#include "Label.h"

class Button : public Child {

    void (*callback)(void *data);
    void *data;
    int listening_window;
    SDL_Rect click_location = { 0, 0, 0, 0 };
    bool click_enabled = false;

    Label label;
    SDL_Colour c;

    static int button_event_filter( void *, SDL_Event * );

  public:

    Button( int w, int h, std::string text, TTF_Font *font,
               void (*callback)(void *), void *data, int listening_window,
               SDL_Colour c = { 255, 0, 0 });

    virtual void render( SDL_Renderer *, int, int ) override;

    bool is_enabled();

};

#endif
