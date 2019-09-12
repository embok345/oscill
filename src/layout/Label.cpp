#include "Label.h"

Label::Label( std::string text, TTF_Font *font, SDL_Colour colour )
        : text { text }, font { font }, colour { colour } {

    if( font )
        TTF_SizeText( font, text.c_str(), &w, &h );

}

Label::Label( std::string text, TTF_Font *font, int max_w, int max_h,
                 SDL_Colour colour)
        : Label( text, font, colour ) {

    if( w > max_w ) w = max_w;
    if( h > max_h ) h = max_h;

}

Label::~Label() {
    SDL_DestroyTexture( inner_texture );
    SDL_DestroyTexture( outline_texture );
    inner_texture = NULL;
    outline_texture = NULL;
}

void Label::update_text( std::string new_text ) {

    text = new_text;

    SDL_DestroyTexture( inner_texture );
    SDL_DestroyTexture( outline_texture );
    inner_texture = NULL;
    outline_texture = NULL;

    if( font )
        TTF_SizeText( font, text.c_str(), &w, &h );

}

void Label::render( SDL_Renderer *r, int x, int y ) {

    if( !font || text.size() == 0 ) return;

    if( !inner_texture ) {
        SDL_Surface *text_surface = TTF_RenderText_Blended(font,
                text.c_str(), colour);
        inner_texture = SDL_CreateTextureFromSurface( r, text_surface );
        SDL_FreeSurface( text_surface );
    }

    SDL_Rect text_rect { x, y, w, h };
    SDL_RenderCopy( r, inner_texture, NULL, &text_rect );

}

void Label::render( SDL_Renderer *r, int x, int y, bool centered ) {

    if( !font || text.size() == 0 ) return;

    if(!outline_texture) {

        int outline = 1;
        TTF_SetFontOutline( font, outline );

        SDL_Surface *text_surface;
        SDL_Color text_color = { 0, 0, 0 };
        if( !( text_surface = TTF_RenderText_Blended( font, text.c_str(),
                text_color ) ) ) {
            printf( "Could not draw text. %s\n", TTF_GetError() );
            return;
        }
        outline_texture = SDL_CreateTextureFromSurface( r, text_surface );

        SDL_FreeSurface( text_surface );
        TTF_SetFontOutline( font, 0 );
    }

    if( !inner_texture ) {
        TTF_SetFontOutline( font, 0 );

        SDL_Surface *text_surface;
        SDL_Color text_color = { 255, 255, 255 };
        if( !( text_surface = TTF_RenderText_Blended( font, text.c_str(),
                text_color ) ) ) {
            printf( "Could not draw text. %s\n", TTF_GetError() );
            return;
        }
        inner_texture = SDL_CreateTextureFromSurface( r, text_surface );

        SDL_FreeSurface( text_surface );
    }

    SDL_Rect text_rect;
    if( centered ) {
        text_rect = { x - ((w + 2) / 2), y - ((h + 2) / 2),
                w + 2, h + 2 };
    } else {
        text_rect = { x, y, w + 2, h + 2 };
    }

    SDL_RenderCopy( r, outline_texture, NULL, &text_rect );

    text_rect.x += 1;
    text_rect.y += 1;
    text_rect.w = w;
    text_rect.h = h;

    SDL_RenderCopy( r, inner_texture, NULL, &text_rect );
}
