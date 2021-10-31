#include "FontCache.h"

CFontNode* CFontMap::operator[](int index){
    if(index < 0 || index >= map_length)
        return nullptr;

    CFontNode* node = start;

    while(index > 0){
        node = node->next;
        index --;
    }

    return node;
}

CFontGlyph CFont::CreateGlyph(char c, CFontGlyphInfo* info){
    CFontGlyph glyph;

    SDL_Surface* surface = TTF_RenderGlyph_Blended(font, c, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int w = surface->w;
    int h = surface->h;

    SDL_FreeSurface(surface);

    glyph.texture = texture;

    SDL_Rect rect = {0, 0, w, h};

    info->rect = rect;

    return glyph;
}

CFontGlyph* CFont::GetGlyph(char c, CFontGlyphInfo* info){
    CFontNode* node = nodeMap[c - 0x20];

    *info = node->glyphInfo;

    return &node->glyph;
}

SDL_Rect CFont::GetTextSize(std::string text){
    SDL_Rect rect = {0, 0, 0, 0};

    for(char c : text){
        CFontGlyphInfo info;
        GetGlyph(c, &info);

        rect.w += info.rect.w;
        if(info.rect.h > rect.h)
            rect.h = info.rect.h;
    }
    return rect;
}

CFont::CFont(SDL_Renderer* _renderer, const char* _fontfile, int _fontsize, SDL_Color _color)
    : fontfile(_fontfile),
    fontsize(_fontsize),
    color(_color),
    renderer(_renderer)
{
    font = TTF_OpenFont(fontfile, fontsize);

    nodeMap.map_length = 0;
    nodeMap.start = nullptr;

    CFontNode* fontnode = new CFontNode;
    CFontGlyphInfo info;
    fontnode->glyph = CreateGlyph(0x20, &info);
    fontnode->glyphInfo = info;

    nodeMap.start = fontnode;
    nodeMap.map_length ++;

    for(int c = 0x21; c <= 0x7E; c++){
        CFontNode* new_fontnode = new CFontNode;
        new_fontnode->glyph = CreateGlyph(c, &info);
        new_fontnode->glyphInfo = info;

        fontnode->next = new_fontnode;
        new_fontnode->next = nullptr;

        fontnode = new_fontnode;

        nodeMap.map_length ++;
    }
}

CFont::~CFont(){
    CFontNode* node = nodeMap.start;
    do{
        if(node){
            if(node->glyph.texture)
                node->glyph.texture;
        }
        node = node->next;
    } while(node);

    TTF_CloseFont(font);
}

void CFont::DrawText(int x, int y, const char* text){
    int w = 0;

    SDL_Rect dst = {x, y, 0, 0};

    for(int c = 0; c < strlen(text); c++){
        CFontGlyphInfo info;
        CFontGlyph* glyph = GetGlyph(text[c], &info);

        dst.w = info.rect.w;
        dst.h = info.rect.h;

        SDL_RenderCopy(renderer, glyph->texture, &info.rect, &dst);

        dst.x += dst.w;
    }
}

void CFont::DrawTextCentered(int center_x, int center_y, const char* text){
    SDL_Rect text_size = GetTextSize(text);

    DrawText( center_x - text_size.w / 2, center_y - text_size.h / 2, text);
}

void CFont::DrawWrappedText(int x, int y, int w, std::string text){
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

            DrawText(x, y, line_buffer.c_str());

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
        DrawText(x, y, line_buffer.c_str());
    }
}
