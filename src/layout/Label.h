#ifndef LABEL_H
#define LABEL_H

#include "Child.h"
#include <SDL2/SDL_ttf.h>

class Label : public Child {
    std::string text;
    TTF_Font *font;
    SDL_Colour colour;

    SDL_Texture *outline_texture = NULL;
    SDL_Texture *inner_texture = NULL;

  public:

    Label( std::string text, TTF_Font *font,
              SDL_Colour colour = {255, 255, 255} );

    Label( std::string text, TTF_Font *font, int max_w, int max_h,
              SDL_Colour colour = {255, 255, 255} );

    virtual ~Label() override;

    void update_text( std::string new_text );

    virtual void render( SDL_Renderer *r, int x, int y ) override;

    void render( SDL_Renderer *r, int x, int y, bool centered );
};

#endif
