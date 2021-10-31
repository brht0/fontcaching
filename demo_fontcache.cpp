#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "FontCache.h"


// FONT SETTINGS
const char*         fontname = "verdana.ttf";
const int           fontsize = 30;
const SDL_Color     fontcolor = {255, 255, 255, 255}; // RGBA



const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

SDL_Window* window;
SDL_Renderer* renderer;
CachedFont* font;

bool InitializeDemo(){
    if(!SDL_Init(SDL_INIT_VIDEO) == -1 || TTF_Init() == -1){
        return false;
    }

    window = SDL_CreateWindow("Cached Font in SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!window || !renderer){
        return false;
    }

    // Initialize cached font
    font = new CachedFont(renderer, fontname, fontsize, fontcolor, false);
    if(!font){
        return false;
    }

    return true;
}

void QuitDemo(){
    // Free font
    delete font; font = nullptr;

    // Quit SDL2
    SDL_DestroyRenderer(renderer); renderer = nullptr;
    SDL_DestroyWindow(window); window = nullptr;
    TTF_Quit();
    SDL_Quit();
}

int main(int, char*[]){
    if(!InitializeDemo()){
        return 1;
    }

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

        // Initialize strings
        std::stringstream ss;
        ss << (int)SDL_GetTicks();
        std::string text_ticks = u8"SDL_GetTicks() == " + ss.str();
        std::string text_centered = "Centered text.";
        for(int i=0; i<(SDL_GetTicks() / 150) % 10; i++){
            text_centered += '.';
        }
        std::string text_wrapped = "This text should be wrapped around this box!";

        // Draw strings
        font->DrawText(30, 10, text_ticks);
        font->DrawWrappedText(30, 200, 150, text_wrapped);
        font->DrawTextCentered(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, text_centered);

        // Draw box
        SDL_Rect rect = {30, 200, 150, 300};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderDrawRect(renderer, &rect);
        
        // Draw central axis
        SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

        // Present screen
        SDL_RenderPresent(renderer);
    }

    QuitDemo();

    return 0;
}
