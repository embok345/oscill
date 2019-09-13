#include "Button.h"

Button::Button( int w, int h, std::string text, TTF_Font *font,
                   void (*callback)(void *), void *data, int listening_window,
                   SDL_Colour c )
        : callback { callback }, data { data },
          listening_window { listening_window }, label { text, font, w, h },
          c { c } {

    Child::w = w;
    Child::h = h;

    click_location.w = w;
    click_location.h = h;

    SDL_AddEventWatch( button_event_filter, this );
}

int Button::button_event_filter(void *user_data, SDL_Event *e) {

    /* Get the button which triggered the event. */
    Button *self = (Button *)user_data;

    if(!self->is_enabled()) return 1;

    /* If the event was not of the correct type, there's nothing to do. */
    if(e->type != SDL_MOUSEBUTTONUP) return 1;

    /* Get the location of the mouse click. */
    SDL_MouseButtonEvent btn = e->button;
    SDL_Point pnt { btn.x, btn.y };

    /* If the mouse click occured in the correct window, and within the given
     * Button, trigger the Button effect. */
    if( btn.windowID == self->listening_window &&
        SDL_PointInRect( &pnt, &(self->click_location) ) ) {
        self->callback(self->data);
    }

    return 0;
}

bool Button::is_enabled() {
    return click_enabled;
}

void Button::render(SDL_Renderer *r, int x, int y) {


    /* Draw the Button. */
    SDL_SetRenderDrawColor( r, c.r, c.g, c.b, SDL_ALPHA_OPAQUE );
    SDL_Rect fill_rect = { x, y, w, h };
    SDL_RenderFillRect( r, &fill_rect );

    /* Draw the label in the middle of the Button. */
    label.render( r, x + (w / 2.0), y + (h / 2.0), true );

    SDL_Rect viewport;
    SDL_RenderGetViewport( r, &viewport );
    click_location.x = x + viewport.x;
    click_location.y = y + viewport.y;

    click_enabled = ( y < viewport.h && x < viewport.w );

}
