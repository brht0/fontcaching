#ifndef __FONTCACHE_H__
#define __FONTCACHE_H__

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <cstring>
#include <sstream>

struct CFontGlyph{
    SDL_Texture* texture;
};

struct CFontGlyphInfo{
    SDL_Rect rect;
};

struct CFontNode{
    CFontGlyph glyph;
    CFontGlyphInfo glyphInfo;
    CFontNode* next = nullptr;
};

struct CFontMap{
    int map_length;
    CFontNode* start = nullptr;

    CFontNode* operator[](int index);
};

class CFont{
private:
    const char* fontfile;
    int fontsize;
    SDL_Color color;

    SDL_Renderer* renderer;
    TTF_Font* font;

    CFontMap nodeMap;

    CFontGlyph CreateGlyph(char c, CFontGlyphInfo* info);

    CFontGlyph* GetGlyph(char c, CFontGlyphInfo* info);

    SDL_Rect GetTextSize(std::string text);

public:
    CFont(SDL_Renderer* _renderer, const char* _fontfile, int _fontsize, SDL_Color _color);
    ~CFont();

    void DrawText(int x, int y, const char* text);
    void DrawTextCentered(int center_x, int center_y, const char* text);
    void DrawWrappedText(int x, int y, int w, std::string text);
};

#endif // __FONTCACHE_H__