#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "FontCache.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int, char*[]){

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("FontCache Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    SDL_Color white = {255, 255, 255, 255};

    {
        CFont font(renderer, "verdana.ttf", 65, white);

        bool quit = false;
        while(!quit){
            SDL_Event e;
            while(SDL_PollEvent(&e)){
                if(e.type == SDL_QUIT){
                    quit = true;
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(renderer);

            std::stringstream ss;
            ss << (int)SDL_GetTicks();

            std::string text = u8"number: " + ss.str();

            font.DrawText(30, 10, text.c_str());

            std::string text2 = "This is some text that I just wrote, test test test test test test test test test";
            font.DrawWrappedText(0, 70, 800, text2);

            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
