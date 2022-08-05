#include <vector>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <math.h>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <windows.h>


#include <stdlib.h>

#include "Program.h"

#define SZELES 320
#define MAGAS 240

using namespace std;

int main( int argc, char * argv[] )
{

    /// Inicializálás
    srand(time(NULL));
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer( SZELES, MAGAS, SDL_WINDOW_SHOWN, &window, &renderer );
    TTF_Init();
    SDL_SetRenderDrawColor( renderer, 0, 200, 0, 255 );
    SDL_RenderClear( renderer );
    SDL_RenderPresent( renderer );
    Sleep(1000);
    /// Inicializálás vége
    ///Sleep(1000);
    jatek( window, renderer);
    TTF_Quit();
    return 0;
}
