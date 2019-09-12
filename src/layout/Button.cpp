#include "Button.h"

Button::Button( int w, int h, std::string text, TTF_Font *font,
                   void (*callback)(void *), void *data, int listening_window,
                   SDL_Colour c )
        : background { 0, 0, w, h }, callback { callback }, data { data },
              listening_window { listening_window },
              label { text, font, w, h }, c { c } {

    Child::w = w;
    Child::h = h;

    SDL_AddEventWatch( button_event_filter, this );
}

int Button::button_event_filter(void *user_data, SDL_Event *e) {

    /* Get the button which triggered the event. */
    Button *self = (Button *)user_data;

    /* If the event was not of the correct type, there's nothing to do. */
    if(e->type != SDL_MOUSEBUTTONUP) return 1;

    /* Get the location of the mouse click. */
    SDL_MouseButtonEvent btn = e->button;
    SDL_Point pnt { btn.x, btn.y };

    /* If the mouse click occured in the correct window, and within the given
     * Button, trigger the Button effect. */
    if( btn.windowID == self->listening_window &&
        SDL_PointInRect( &pnt, &(self->background) ) ) {
        self->callback(self->data);
    }

    return 0;
}

void Button::render(SDL_Renderer *r, int x, int y) {

    /* Update the relative location of the button. */
    background.x = x;
    background.y = y;

    /* Draw the Button. */
    SDL_SetRenderDrawColor( r, c.r, c.g, c.b, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r, &background );

    /* Draw the label in the middle of the Button. */
    label.render( r, x + (w / 2.0), y + (h / 2.0), true );
}
