#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

class Child {

  protected:

    int w = 0;
    int h = 0;

  public:

    virtual void render( SDL_Renderer *, int, int ) { }
    int get_width( ) { return w; }
    int get_height( ) { return h; }
};

class Label : public Child {
    std::string text;
    TTF_Font *font;
    SDL_Colour colour;

  public:

    Label( std::string text, TTF_Font *font,
              SDL_Colour colour = {255, 255, 255} )
            : text { text }, font { font }, colour { colour } {
        char cstr[text.size() + 1];
        strcpy( cstr, text.c_str());
        TTF_SizeText( font, cstr, &w, &h );
    }

    Label( std::string text, TTF_Font *font, int max_w, int max_h,
              SDL_Colour colour = {255, 255, 255} )
            : Label( text, font, colour ) {
        if( w > max_w ) w = max_w;
        if( h > max_h ) h = max_h;
    }

    virtual void render( SDL_Renderer *r, int x, int y ) override {

        char cstr[text.size() + 1];
        strcpy( cstr, text.c_str() );

        SDL_Surface *text_surface;
        text_surface = TTF_RenderText_Blended(font, cstr, colour);

        SDL_Texture *text_texture =
                SDL_CreateTextureFromSurface(r, text_surface);

        SDL_Rect text_rect { x, y, w, h };
        SDL_RenderCopy( r, text_texture, NULL, &text_rect );
        SDL_FreeSurface( text_surface );

    }
};

class Button : public Child {

    SDL_Rect background;

    void *(*callback)(void *data);
    void *data;

    static int button_event_filter(void *user_data, SDL_Event *e) {

        Button *self = (Button *)user_data;
        if(e->type != SDL_MOUSEBUTTONUP) return 1;

        SDL_MouseButtonEvent btn = e->button;
        SDL_Point pnt { btn.x, btn.y };

        if( SDL_PointInRect( &pnt, &(self->background) ) ) {
            self->callback(self->data);
        }

        return 0;

    }

  protected:
    Label label;

  public:

    Button( int w, int h, std::string text, TTF_Font *font,
                void *(*callback)(void *), void *data)
            : background { 0, 0, w, h }, callback { callback }, data { data },
              label { text, font, w, h } {
        Child::w = w;
        Child::h = h;

        SDL_AddEventWatch( button_event_filter, this );
    }

    virtual void render(SDL_Renderer *r, int x, int y) override {

        background.x = x;
        background.y = y;
        SDL_SetRenderDrawColor( r, 255, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r, &background );

        label.render( r, x, y );
    }

};

template <class T>
class Slider : public Child {

    SDL_Rect boundary;

    double proportion;

    T *bound_value;
    T (*calculate_value)(double proportion);

    Label label;

    void update_proportion(double new_proportion) {
        if(new_proportion < 0) proportion = 0;
        else if(new_proportion > 1) proportion = 1;
        else proportion = new_proportion;

        *bound_value = calculate_value(proportion);
    }

    void handle_mouseclick(SDL_MouseButtonEvent e) {
        SDL_Point clicked_point = { e.x, e.y };

        if( !SDL_PointInRect( &clicked_point, &boundary ) ) return;

        double new_proportion = (clicked_point.x - boundary.x) /
                (double)boundary.w;

        update_proportion(new_proportion);
    }

    void handle_mousewheel(SDL_MouseWheelEvent e) {

        SDL_Point mouse_location;
        SDL_GetMouseState(&(mouse_location.x), &(mouse_location.y));

        if( !SDL_PointInRect( &mouse_location, &boundary ) ) return;

        if(e.y == 0) return;

        update_proportion( proportion + (0.01 * e.y) );
    }

    static int slider_event_filter( void *user_data, SDL_Event *e ) {
        Slider *self = (Slider *)user_data;
        if(e->type == SDL_MOUSEBUTTONUP)
            self->handle_mouseclick( e->button );
        else if( e-> type == SDL_MOUSEWHEEL )
            self->handle_mousewheel( e->wheel );

        return 0;
    }

  public:

    Slider( int w, int h, T *bound_value, T (*calculate_value)(double),
               double start, std::string label, TTF_Font *font )
            : proportion { start }, bound_value { bound_value },
             calculate_value { calculate_value }, label { label, font, w, h } {
        Child::w = w;
        Child::h = h;

        boundary.w = w;
        boundary.h = h;
        boundary.x = 0;
        boundary.y = 0;

        SDL_AddEventWatch( slider_event_filter, this );

        update_proportion( proportion );
    }

    virtual void render( SDL_Renderer *r, int x, int y ) override {

        boundary.x = x;
        boundary.y = y;
        SDL_SetRenderDrawColor( r, 255, 255, 255, SDL_ALPHA_OPAQUE );
        SDL_RenderDrawRect( r, &boundary );

        SDL_Rect background { x + 1, y + 1, w - 2, h - 2 };
        SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r, &background );

        SDL_Rect slider { x + 1, y + 1, (int)((w - 2) * proportion), h - 2 };
        SDL_SetRenderDrawColor( r, 200, 200, 200, SDL_ALPHA_OPAQUE );
        SDL_RenderFillRect( r, &slider );

        label.render( r, x + 1, y + 1 );
    }
};

class Pane : public Child {

    /*struct ChildNode {
        Child *c;
        int x;
        int y;
    };*/

  protected:

    std::vector<Child*> children;

  public:

    virtual void add_child( Child *c ) {
        children.push_back( c );
    }


    /*virtual void render( SDL_Renderer *r, int x, int y ) override {
        std::vector<Child*>::iterator it;
        it = children.begin();
        while( it != children.end() ) {
            (*it)->render( r, x, y );
            ++it;
        }
    }*/
};

class VerticalPane : public Pane {

    int v_gap = 0;

  public:

    VerticalPane( int v_gap = 0 ) : v_gap { v_gap } {}

    virtual void add_child( Child *c ) override {
        Pane::add_child( c );
        h += c->get_height() + v_gap;
        if( c->get_width() > w ) w = c->get_width();
    }

    virtual void render( SDL_Renderer *r, int x, int y ) {
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
};

class HorizontalPane : public Pane {
   //int w = 0;
   //int h = 0;
   int h_gap = 0;

  public:

    HorizontalPane( int h_gap = 0 ) : h_gap { h_gap } {}

    virtual void add_child( Child *c ) override {
        Pane::add_child( c );
        w += c->get_width() + h_gap;
        if( c->get_height() > h ) h = c->get_height();
    }

    virtual void render( SDL_Renderer *r, int x, int y ) {
        std::vector<Child*>::iterator it = children.begin();
        h = 0;
        w = 0;
        while(it != children.end()) {
            (*it)->render( r, x + w, y );
            w += (*it)->get_width() + h_gap;
            if((*it)->get_height() > h) h = (*it)->get_height();
            it++;
        }
    }
};

#endif
