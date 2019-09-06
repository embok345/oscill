#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "consts.h"

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


void find_mono_fonts( const char *dir_name, char ***fonts, int *current_fonts ) {

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if( ( dp = opendir( dir_name ) ) == NULL ) {
        printf("Couldn't open directory\n");
        return;
    }
    chdir( dir_name );

    while( ( entry = readdir( dp ) ) != NULL ) {
        lstat( entry->d_name, &statbuf );
        if( S_ISDIR( statbuf.st_mode ) ) {
            if( strcmp( ".", entry->d_name ) == 0 ||
                    strcmp( "..", entry->d_name ) == 0 )
                continue;
            find_mono_fonts( entry->d_name, fonts, current_fonts );
        } else {
            if( strstr( entry->d_name, "Mono" ) != NULL &&
                    strcasestr( entry->d_name, ".ttf" ) != NULL ) {

                size_t cwd_len = 128;
                char *cwd = NULL;
                while(true) {
                    cwd = (char *)malloc(cwd_len);
                    errno = 0;
                    if( !(cwd = getcwd(cwd, cwd_len)) ) {
                        if(errno == ERANGE) {
                            cwd_len += 128;
                            if(cwd_len >= PATH_MAX) {
                                free(cwd);
                                return;
                            }
                        }
                        continue;
                    }
                    break;
                }

                *fonts = (char **)realloc( *fonts, ((*current_fonts) + 1) * sizeof(char *));
                (*fonts)[*current_fonts] = (char *)malloc(strlen(cwd) + 1 + strlen(entry->d_name) + 1);
                sprintf((*fonts)[*current_fonts], "%s/%s", cwd, entry->d_name);
                free( cwd );
                (*current_fonts)++;
            }
        }
    }
    chdir("..");
    closedir(dp);

}

/* Load the font with the given name.
 * ----------------------------------------------------------------------------
 * const char *font_name -- The name of the true type font to load. This should
 *                          be a valid fonot name located within the res sub
 *                          directory.
 *
 * return                -- A pointer to the loaded font, or NULL if the
 *                          requested font couldn't be loaded.
 */
TTF_Font *get_font( void ) {
    TTF_Font *font;

    static const char *default_font = "/usr/share/fonts/truetype/freefont/FreeMono.ttf";

    /* Try to open the default font. */
    font = TTF_OpenFont( default_font, 24 );
    if( !font ) {
        printf("Couldn't open font. %s\n", TTF_GetError() );
        /* If we couldn't get the default font, search for a different one. */
        char **mono_fonts = NULL;
        int no_mono_fonts = 0;
        find_mono_fonts( "/usr/share/fonts/truetype", &mono_fonts,
                &no_mono_fonts);
        if( no_mono_fonts == 0 ) {
            printf("Could not get any fonts.\n");
            return NULL;
        }
        for(int i = 0; i < no_mono_fonts; i++ ) {
            if( !font ) {
                if( !(font = TTF_OpenFont( mono_fonts[i], 24 ) ) ) {
                    printf("Couldn't open font. %s\n", TTF_GetError() );
                }
            }
            free(mono_fonts[i]);
        }
        free( mono_fonts );
        if( !font ) {
            printf("Could not get any font.\n");
            return NULL;
        }
    }

    return font;
}

/* Render the text with an outline on screen. It is drawn in the given font at
 * the specified position, in white with a black outline. If centered is set to
 * true, the text will be drawn with center at the given x and y coordinates,
 * if false, the top left corner of the text shall be at the given coords.
 * ----------------------------------------------------------------------------
 * SDL_Renderer *r  -- The renderer to draw the text with.
 * TTF_Font *font   -- The font to render the text in.
 * const char *text -- The text to render.
 * int x            -- The x coordinate to render the text at.
 * int y            -- The y coordinate to render the text at.
 * bool centered    -- Determines if the coordinates define the center of the
 *                     text, or the top left of the text.
 *
 * return           -- 1 if the text was successfully renderer, 0 otherwise.
 */
int render_outlined_text( SDL_Renderer *r, TTF_Font *font, const char *text,
                    int x, int y, bool centered) {
    SDL_Color white = { 255, 255, 255 }, black = { 0, 0, 0 };

    /* Set the rendered text to have an outline. */
    int outline = 1;
    TTF_SetFontOutline( font, outline );

    /* Try to create the outline text. */
    SDL_Surface *text_surface;
    SDL_Color text_color = black;
    if( !( text_surface = TTF_RenderText_Blended(font, text, text_color) ) ) {
        printf( "Could not draw text. %s\n", TTF_GetError() );
        return 0;
    }
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(r, text_surface);

    /* Get the size of text. */
    int text_width, text_height;
    if( TTF_SizeText( font, text, &text_width, &text_height ) == -1 ) {
        printf( "Could not get text size. %s\n", SDL_GetError() );
        return 0;
    }

    /* Determine the location to render the text. */
    SDL_Rect text_rect;
    if( centered ) {
        /* If the text is centered, the upper left is the x coordinate minus
         * half the height, (resp. y). */
        text_rect = { x - (text_width / 2), y - (text_height / 2),
                text_width, text_height };

        /* Have to fudge it if the width isn't a multiple of 2. */
        if(text_width % 2 == 1)
            text_rect.x -= 1;
    } else {
        /* If the text isn't centered, the x and y coordinates are just they.*/
        text_rect = { x, y, text_width, text_height };
    }

    /* Render the outline text. */
    SDL_RenderCopy( r, text_texture, NULL, &text_rect );
    SDL_FreeSurface( text_surface );

    /* Draw now the inner text, in the same way. */
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

void set_scene( SDL_Renderer *r, SDL_Rect *graph_rect ) {

    /* Clear the screen. */
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect( r, graph_rect );

    /* Draw the top and bottom guide lines. */
    SDL_SetRenderDrawColor( r, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine( r, 0, ZERO_LEVEL - MAX_AMPLITUDE, SCREEN_WIDTH,
            ZERO_LEVEL - MAX_AMPLITUDE );
    SDL_RenderDrawLine( r, 0, ZERO_LEVEL + MAX_AMPLITUDE, SCREEN_WIDTH,
            ZERO_LEVEL + MAX_AMPLITUDE );

    /* Draw the center guide line. */
    SDL_SetRenderDrawColor( r, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine( r, 0, ZERO_LEVEL, SCREEN_WIDTH, ZERO_LEVEL );
    /* Draw a black line, otherwise the alpha goes weird.
     * TODO i'm sure there is a correct way to make sure the blending
     * doesn't go weird. */
    SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderDrawLine( r, 0, 0, 2, 0 );
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

