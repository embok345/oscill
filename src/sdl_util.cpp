#include <SDL2/SDL.h>

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

    SDL_Quit();
}

