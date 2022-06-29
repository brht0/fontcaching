#ifndef __FONT_H__
#define __FONT_H__

#define GL_GLEXT_PROTOTYPES
#include "GLFW/glfw3.h"
#include "SDL2/SDL_ttf.h"
#include <string>
#include <iostream>

class Font{
public:
    Font(const char* filename, int resolution);
    void RenderText(std::string text, double x, double y, double textHeight, SDL_Color color);

private:
    static constexpr unsigned maxChar = 256;
    unsigned texture;
    int w, h;
    SDL_FRect rects[maxChar];
};

Font::Font(const char* filename, int resolution):
    w(0), h(0)
{
    // This is here for convenience, likely the only file using SDL_TTF anyway
    if(!TTF_WasInit())
        TTF_Init();

    // Render glyphs
    TTF_Font* font = TTF_OpenFont(filename, resolution);
    if(!font){
        std::cout << "Error: Could not open font file: " << filename << "\n";
    }

    SDL_Surface* surfaces[maxChar];
    for(unsigned c=0; c<maxChar; c++){
        SDL_Surface* s1 = TTF_RenderGlyph_Shaded(font, c, {0xFF, 0xFF, 0xFF, 0xFF}, {0, 0, 0, 0});
        if(!s1){
            surfaces[c] = nullptr;
            rects[c] = {};
        }
        else{
            SDL_Surface* s2 = SDL_ConvertSurfaceFormat(s1, SDL_PIXELFORMAT_RGBA8888, 0);
            SDL_FreeSurface(s1);

            surfaces[c] = s2;
            rects[c] = {(float)w, (float)s2->h, (float)s2->w, (float)s2->h};

            w += s2->w;
            h = std::max(h, s2->h);
        }
    }
    TTF_CloseFont(font);

    // Combine glyphs
    SDL_Surface* finalSurface = nullptr;
    SDL_Rect dst = {0, 0, 0, 0};
    for(int i=0; i<maxChar; i++){
        auto surface = surfaces[i];
        if(!surface){
            continue;
        }
        if(!finalSurface){
            finalSurface = SDL_CreateRGBSurface(surface->flags, w, h, 32, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
        }
        dst.w = surface->w;
        dst.h = surface->h;
        SDL_BlitSurface(surface, 0, finalSurface, &dst);
        dst.x += surface->w;
        SDL_FreeSurface(surface);
    }
    
    // Create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)finalSurface->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(finalSurface);
}

void Font::RenderText(std::string text, double x, double y, double textHeight, SDL_Color color){
    int screenW, screenH;
    glfwGetWindowSize(glfwGetCurrentContext(), &screenW, &screenH);
    float aspectRatio = screenH / (float)screenW;

    glBindTexture(GL_TEXTURE_2D, texture);
    float vx2 = x;
    for(char c : text){
        auto rect = rects[(unsigned char)c];
        if(rect.w == 0 || rect.h == 0)
            continue;

        float tx1 = rect.x / (float)w;
        float ty1 = rect.y / (float)h;
        float tx2 = (rect.x + rect.w) / (float)w;
        float ty2 = (rect.y + rect.h) / (float)h;

        float vx1 = vx2;
        vx2 += (rect.w * (2.0*textHeight / rect.h)) * aspectRatio;
        float vy1 = y;
        float vy2 = y + 2.0*textHeight;

        glBegin(GL_QUADS);
            glColor4f(color.r/(float)256, color.g/(float)256, color.b/(float)256, color.a/(float)256);
            glTexCoord2f    (tx2, ty2);
            glVertex2f      (vx2, vy1);
            glTexCoord2f    (tx1, ty2);
            glVertex2f      (vx1, vy1);
            glTexCoord2f    (tx1, ty1);
            glVertex2f      (vx1, vy2);
            glTexCoord2f    (tx2, ty1);
            glVertex2f      (vx2, vy2);
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // __FONT_H__