#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/* Initialises SDL library, and the main window in which to draw.
 * ----------------------------------------------------------------------------
 * SDL_Window **w   -- A pointer to the SDL window to be initialised.
 * SDL_Renderer **r -- A pointer to the SDL renderer to be initialised.
 * int width        -- The width of the window to be created.
 * int height       -- The height of the window to be created.
 *
 * return           -- 1 if everything initialised fine, 0 if not.
 */
int init(SDL_Window **w, SDL_Renderer **r, int width, int height) {
    /* Try to init SDL. */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0) {
        /* If we couldn't init SDL, print the error and return failure. */
        printf("Could not init SDL. %s\n", SDL_GetError());
        return 0;
    }

    /* Try to create the main window. */
    *w = SDL_CreateWindow( "Test", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, width, height,
            SDL_WINDOW_SHOWN );

    if( !(*w) ) {
        /* If we can't create the window, print the error, return falure. */
        printf("Could not create window. %s\n", SDL_GetError() );
        return 0;
    }

    /* Try to create the renderer of the window. */
    *r = SDL_CreateRenderer( *w, -1, SDL_RENDERER_ACCELERATED );
    if( !(*r) ) {
        /* If we can't create the renderer, print the error, return failure. */
        printf("Could not create renderer. %s\n", SDL_GetError() );
        return 0;
    }

    SDL_SetRenderDrawBlendMode( *r, SDL_BLENDMODE_BLEND );

    /* Try to init text. */
    if( !TTF_WasInit() && TTF_Init() == -1 ) {
        /* If we couldn't init text, return failure. */
        printf( "Couldn't init text. %s\n", TTF_GetError() );
        return 0;
    }

    return 1;
}

TTF_Font *get_font(const char *font_name) {
    TTF_Font *font;
    const char *path = "../res/font/";
    char *font_path = (char *)malloc( strlen(font_name) + strlen(path) + 1 );
    sprintf(font_path, "%s%s.ttf", path, font_name);
    font = TTF_OpenFont( font_path, 24 );

    if( !font ) {
        printf("Could not get font. %s\n", TTF_GetError() );
    }

    free( font_path );

    return font;
}

int render_text( SDL_Renderer *r, TTF_Font *font, const char *text,
                    int x, int y, bool centered) {
    SDL_Color white = { 255, 255, 255 }, black = { 0, 0, 0 };

    int outline = 1;

    TTF_SetFontOutline( font, outline );

    SDL_Surface *text_surface;
    SDL_Color text_color = black;
    if( !( text_surface = TTF_RenderText_Blended( font, text, text_color ) ) ) {
        printf( "Could not draw text. %s\n", TTF_GetError() );
        return 0;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(r, text_surface);
    int text_width, text_height;
    if( TTF_SizeText( font, text, &text_width, &text_height ) == -1 ) {
        printf( "Could not get text size. %s\n", SDL_GetError() );
        return 0;
    }
    SDL_Rect text_rect;
    if( centered ) {
        text_rect = { x - (text_width / 2), y - (text_height / 2),
                text_width, text_height };
        if(text_width % 2 == 1)
            text_rect.x -= 1;
    } else {
        text_rect = { x, y, text_width, text_height };
    }
    SDL_RenderCopy( r, text_texture, NULL, &text_rect );
    SDL_FreeSurface( text_surface );

    TTF_SetFontOutline( font, 0 );
    text_color = white;
    if( !( text_surface = TTF_RenderText_Blended( font, text, text_color ) ) ) {        printf( "Could not draw text. %s\n", TTF_GetError() );
        return 0;
    }
    text_texture = SDL_CreateTextureFromSurface( r, text_surface );

    if( TTF_SizeText( font, text, &text_width, &text_height ) == -1 ) {
        printf( "Could not get text size. %s\n", SDL_GetError() );
        return 0;
    }
    if( centered ) {
        text_rect = { x - (text_width / 2), y - (text_height / 2),
                text_width, text_height };
    } else {
        text_rect = { x, y, text_width, text_height };
    }

    SDL_RenderCopy( r, text_texture, NULL, &text_rect );

    SDL_FreeSurface( text_surface );

    return 1;
}

/* Free the SDL resources which have been used.
 * ----------------------------------------------------------------------------
 * SDL_Window **w   -- The SDL window to be freed.
 * SDL_Rednerer **r -- The SDL renderer to be freed.
 */
void close( SDL_Window **w, SDL_Renderer **r ) {
    SDL_DestroyRenderer( *r );
    SDL_DestroyWindow( *w );
    *r = NULL;
    *w = NULL;

    if( TTF_WasInit() )
        TTF_Quit();

    SDL_Quit();
}

