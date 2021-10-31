#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "FontCache.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int, char*[]){

    // Initialize SDL2 stuff
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Cached Font in SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    // Initialize cached font
    const char* fontname = "verdana.ttf";
    int fontsize = 50;
    SDL_Color fontcolor = {255, 255, 255, 255};

    CachedFont* font = new CachedFont(renderer, fontname, fontsize, fontcolor, false);

    // Main loop
    bool quit = false;
    while(!quit){
        // Events
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        // Initialize some strings
        std::stringstream ss;
        ss << (int)SDL_GetTicks();
        std::string text = u8"number: " + ss.str();
        std::string text2 = "This is some text that I just wrote, test test test test test test test test test";

        // Draw strings on screen buffer with cached font
        font->DrawText(30, 10, text.c_str());
        font->DrawWrappedText(0, 70, 800, text2);

        // Present screen
        SDL_RenderPresent(renderer);
    }

    // Free memory
    delete font; font = nullptr;

    // Quit SDL2
    SDL_DestroyRenderer(renderer); renderer = nullptr;
    SDL_DestroyWindow(window); window = nullptr;
    TTF_Quit();
    SDL_Quit();

    return 0;
}
