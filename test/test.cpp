#include <SDL2/SDL.h>
#include "ScrollPane.h"
#include "VerticalPane.h"
#include "Button.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main() {

    SDL_Init( SDL_INIT_VIDEO );

    auto window = SDL_CreateWindow( "Test", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

    auto root_pane = new VerticalPane( 5 );
    auto scroll_pane1 = new ScrollPane( WINDOW_HEIGHT / 2, WINDOW_WIDTH - 5,
            new VerticalPane( 5 ) );
    auto scroll_pane2 = new ScrollPane( WINDOW_HEIGHT / 2, WINDOW_WIDTH - 5,
            new VerticalPane( 5 ) );
    root_pane->add_child(scroll_pane1);
    root_pane->add_child(scroll_pane2);

    int no_btns = 10;
    for(int i = 0; i < no_btns; i++) {
        scroll_pane1->add_child( new Button( WINDOW_WIDTH - 10, 40,
                "", NULL,
                [](void *data) {
                    printf("red button pressed\n");
                    auto pane = (ScrollPane *)data;
                    pane->add_child( new Button( WINDOW_WIDTH - 10, 40, "",
                            NULL, [](void*){}, NULL, 0 ) );
                }, (void*)scroll_pane1, SDL_GetWindowID( window ) ) );
        scroll_pane2->add_child( new Button( WINDOW_WIDTH - 10, 40,
                "", NULL,
                [](void *data) {
                    printf("green button clicked\n");
                    auto pane = (ScrollPane *)data;
                    pane->add_child( new Button( WINDOW_WIDTH - 10, 40, "",
                            NULL, [](void*){}, NULL, 0, {0,255,0} ) );
                }, (void*)scroll_pane2, SDL_GetWindowID( window ), {0,255,0}));
    }

    auto quit = false;
    int scroll_speed = 10;
    while( !quit ) {
        SDL_Event e;
        while( SDL_PollEvent(&e) != 0 ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
                break;
            }
        }

        SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
        SDL_RenderClear( renderer );
        root_pane->render( renderer, 0, 0 );
        SDL_RenderPresent( renderer );

        SDL_Delay( 10 );
    }

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );


    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow(window);
    SDL_Quit( );

}
