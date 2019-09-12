#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "consts.h"
#include "sdl_util.h"

/* Initialises the SDL library.
 * ----------------------------------------------------------------------------
 * return           -- 1 if everything initialised fine, 0 if not.
 */
int init(  ) {
    /* Try to init SDL. */
    if( !SDL_WasInit( SDL_INIT_VIDEO ) && SDL_Init( SDL_INIT_VIDEO ) < 0) {
        /* If we couldn't init SDL, print the error and return failure. */
        printf("Could not init SDL. %s\n", SDL_GetError());
        return 0;
    }

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
    if(! chdir( dir_name ) ) return;

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
    if( !chdir("..") );
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

    static const char *default_font =
            "/usr/share/fonts/truetype/freefont/FreeMono.ttf";

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

SDL_Colour colour_proportion(double proportion) {
    if(proportion < 0) proportion = 0;
    else if(proportion > 1) proportion = 1;

    double divisions = 8.0;

    SDL_Colour ret { 0, 0, 0};
    if( proportion < 1 / divisions ) {
        ret.r = 255;
        ret.b = 255 * divisions * proportion;
    } else if( proportion < 2 / divisions ) {
        ret.r = 255 * divisions * ( ( 2.0 / divisions ) - proportion );
        ret.b = 255;
    } else if( proportion < 3 / divisions ) {
        ret.g = 255 * divisions * ( proportion - (2.0 / divisions) );
        ret.b = 255;
    } else if( proportion < 4 / divisions ) {
        ret.g = 255;
        ret.b = 255 * divisions * ( ( 4 / divisions ) - proportion );
    } else if( proportion < 5 / divisions ) {
        ret.r = 255 * divisions * ( proportion - ( 4 / divisions ) );
        ret.g = 255;
    } else if( proportion < 6 / divisions ) {
        ret.r = 255;
        ret.g = 255 * divisions * ( ( 6 / divisions ) - proportion );
    } else if( proportion < 7 / divisions ) {
        ret.r = 255 * divisions * ( ( 7 / divisions ) - proportion );
    } else {
        ret.r = 255 * divisions * ( proportion - ( 7 / divisions ) );
        ret.g = 255 * divisions * ( proportion - ( 7 / divisions ) );
        ret.b = 255 * divisions * ( proportion - ( 7 / divisions ) );
    }
    return ret;
}

double average(double min_value, double max_value, double proportion) {
    if(proportion < 0) proportion = 0;
    else if(proportion > 1) proportion = 1;

    return ( proportion * max_value ) + ( ( 1 - proportion ) * min_value );
}

double log_average( double min_value, double max_value, int base,
                       double proportion ) {
    if(proportion < 0) proportion = 0;
    else if(proportion > 1) proportion = 1;

    double first, second, power;

    switch(base) {
        case 0:
        case 1:
            first = log( min_value );
            second = log( max_value );
            power = M_E;
            break;
        case 2:
            first = log2( min_value );
            second = log2( max_value );
            power = 2;
            break;
        case 10:
            first = log10( min_value );
            second = log10( max_value );
            power = 10;
            break;
        default:
            first = log( min_value ) / log( base );
            second = log( max_value ) / log( base );
            power = base;
    }
    return pow(power, ( ( 1 - proportion ) * first ) + ( proportion * second ) );
}

/* Free the SDL resources which have been used.
 * ----------------------------------------------------------------------------
 * SDL_Window **w   -- The SDL window to be freed.
 * SDL_Rednerer **r -- The SDL renderer to be freed.
 */
void close( ) {

    if( TTF_WasInit() )
        TTF_Quit();

    if( SDL_WasInit(SDL_INIT_VIDEO) )
        SDL_Quit();
}
