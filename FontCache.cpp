#include "FontCache.h"

SDL_Rect CachedFont::GetTextSize(std::string text){
    SDL_Rect rect = {0, 0, 0, 0};

    for(char c : text){
        CachedFontGlyph* glyph = GetGlyph(c);
        if(!glyph)
            continue;

        rect.w += glyph->w;
        rect.h = std::max(rect.h, glyph->h);
    }

    return rect;
}

CachedFont::CachedFont(SDL_Renderer* _renderer, const char* _fontfile, int _fontsize, SDL_Color _fontcolor, bool load_all)
    : fontfile(_fontfile),
    fontsize(_fontsize),
    fontcolor(_fontcolor),
    renderer(_renderer)
{
    font = TTF_OpenFont(fontfile, fontsize);
    if(!font){
        std::cout << "Font could not be loaded: " << font << "\n";
    }

    int start = 0;
    int end = 0xFF;

    if(load_all){
        for(int c = start; c <= end; c++){
            glyphMap[c] = CreateNewGlyph(c);
        }
    }
}

CachedFont::~CachedFont(){
    TTF_CloseFont(font);

    for(auto x : glyphMap){
        CachedFontGlyph* glyph = x.second;

        if(glyph){
            if(glyph->texture)
                SDL_DestroyTexture(glyph->texture);
            delete glyph;
        }
    }
}

void CachedFont::DrawText(int x, int y, std::string text){
    int w = 0;

    SDL_Rect src = {0, 0, 0, 0};
    SDL_Rect dst = {x, y, 0, 0};

    for(char c : text){
        CachedFontGlyph* glyph = GetGlyph(c);

        if(glyph){
            src.w = dst.w = glyph->w;
            src.h = dst.h = glyph->h;

            SDL_RenderCopy(renderer, glyph->texture, &src, &dst);

            dst.x += dst.w;
        }
    }
}

void CachedFont::DrawTextCentered(int center_x, int center_y, std::string text){
    SDL_Rect text_size = GetTextSize(text);

    DrawText(center_x - text_size.w / 2, center_y - text_size.h / 2, text);
}

void CachedFont::DrawWrappedText(int x, int y, int w, std::string text){
    std::string line_buffer = "";

    bool first_in_line = true;

    while(text.length() > 0){
        std::string word;
        int space_index = text.find_first_of(' ');

        if(space_index == -1)
            word = text;
        else
            word = text.substr(0, space_index);

        if(!first_in_line)
            line_buffer += ' ';

        line_buffer += word;

        SDL_Rect line_size = GetTextSize(line_buffer);

        if(line_size.w > w){
            if(first_in_line){
                text = text.substr(line_buffer.length() + 1);
            }
            else{
                line_buffer = line_buffer.substr(0, line_buffer.length() - word.length());
            }

            DrawText(x, y, line_buffer);

            y += line_size.h;
            first_in_line = true;
            line_buffer.clear();
        }
        else{
            if(text.length() <= word.length())
                text = "";
            else
                text = text.substr(word.length() + 1);

            first_in_line = false;
        }

    }

    if(!line_buffer.empty()){
        DrawText(x, y, line_buffer);
    }
}

CachedFontGlyph* CachedFont::CreateNewGlyph(char c) {
    CachedFontGlyph* glyph = new CachedFontGlyph;

    SDL_Surface* surface = TTF_RenderGlyph_Blended(font, c, fontcolor);
    if(surface){
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        glyph->texture = texture;
        glyph->w = surface->w;
        glyph->h = surface->h;

        SDL_FreeSurface(surface);

        // glyph could not be loaded or character has no dimensions
        if(!glyph->texture || glyph->w == 0 || glyph->h == 0){
            if(glyph->texture) SDL_DestroyTexture(glyph->texture);
            delete glyph;

            return nullptr;
        }

        return glyph;
    }

    return nullptr;
}

CachedFontGlyph* CachedFont::GetGlyph(char c) {
    auto it = glyphMap.find(c);

    if(it != glyphMap.end()){
        return it->second;
    }
    else{
        CachedFontGlyph* glyph = CreateNewGlyph(c);
        glyphMap[c] = glyph;
        return glyph;
    }
}
