#ifndef __FONTCACHE_H__
#define __FONTCACHE_H__

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <vector>

struct CachedFontGlyph{
    SDL_Texture* texture;
    int w, h;
};

class CachedFont{
public:
    CachedFont(SDL_Renderer* _renderer, const char* _fontfile, int _fontsize, SDL_Color _fontcolor, bool load_all = true);
    ~CachedFont();

    void DrawText(int x, int y, const char* text);
    void DrawTextCentered(int center_x, int center_y, const char* text);
    void DrawWrappedText(int x, int y, int w, std::string text);

private:
    std::unordered_map<char, CachedFontGlyph*> glyphMap;
    CachedFontGlyph* CreateNewGlyph(char c);
    CachedFontGlyph* GetGlyph(char c);

    SDL_Rect GetTextSize(std::string text);

private:
    const char* fontfile;
    int fontsize;
    SDL_Color fontcolor;

    std::vector<CachedFontGlyph> glyphs;
    SDL_Renderer* renderer;
    TTF_Font* font;

};

#endif // __FONTCACHE_H__