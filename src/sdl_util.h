#ifndef SDL_UTIL_H
#define SDL_UTIL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int init(SDL_Window **, SDL_Renderer **, int, int);
void close(SDL_Window **, SDL_Renderer **);
TTF_Font *get_font( );
int render_outlined_text( SDL_Renderer *, TTF_Font *, const char *, int, int, bool = false);
void set_scene( SDL_Renderer *, SDL_Rect * );

#endif
