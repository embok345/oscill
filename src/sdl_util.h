#ifndef SDL_UTIL_H
#define SDL_UTIL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class SDL_WindowRenderer {

  protected:
    SDL_Window *w;
    SDL_Renderer *r;

  public:

    SDL_WindowRenderer( int width, int height, const char* title ) {
        if( ! SDL_WasInit( SDL_INIT_VIDEO ) ) throw -1;

        /* Try to create the window. */
        w = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );

        if( !w ) {
            /* If we can't create the window, print the error, return falure.*/
            printf("Could not create window. %s\n", SDL_GetError() );
            throw -1;
        }

        /* Try to create the renderer of the window. */
        r = SDL_CreateRenderer( w, -1, SDL_RENDERER_ACCELERATED );
        if( !r ) {
            /* If we can't create the renderer, print the error, return failure. */
            printf("Could not create renderer. %s\n", SDL_GetError() );
            throw -1;
        }

        SDL_SetRenderDrawBlendMode( r, SDL_BLENDMODE_BLEND );

        SDL_SetRenderDrawColor( r, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderClear( r );

    }

    ~SDL_WindowRenderer() {
        SDL_DestroyRenderer( r );
        SDL_DestroyWindow( w );
    }

    SDL_Renderer *get_renderer( ) { return r; }
    SDL_Window *get_window( ) { return w; }
};

int init();
void close();
TTF_Font *get_font( );
int render_outlined_text( SDL_WindowRenderer&, TTF_Font *, const char *, int, int, bool = false);
SDL_Colour colour_proportion( double );

double average( double, double, double );
double log_average( double, double, int, double );

#endif
