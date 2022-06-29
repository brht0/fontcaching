#ifndef __FONT_H__
#define __FONT_H__

#define GL_GLEXT_PROTOTYPES
#include "GLFW/glfw3.h"
#include "SDL2/SDL_ttf.h"
#include <string>
#include <iostream>

#include "Shader.h"

class Font{
public:
    Font(const char* filename, int resolution);
    void RenderText(std::string text, double x, double y, double textHeight, SDL_Color color);

private:
    static constexpr unsigned maxChar = 256;
    int w, h;
    SDL_FRect rects[maxChar];

    struct Vertex{
        float x, y;
        float tx, ty;
        float r, g, b, a;
    };

    unsigned texture, program, vbo, vao;
    static const int RENDER_BATCH_SIZE = 64;
    Vertex vertices[RENDER_BATCH_SIZE * 4];

const char* vert = \
"#version 330 core\n"\
"\n"\
"layout(location = 0) in vec2 screenPos;\n"\
"layout(location = 1) in vec2 texturePos;\n"\
"layout(location = 2) in vec4 inColor;\n"\
"\n"\
"out vec2 outTexturePos;\n"\
"out vec4 outColor;\n"\
"\n"\
"uniform sampler2D charTexture;\n"\
"\n"\
"void main(){\n"\
"   gl_Position = vec4(screenPos, 0.0, 1.0);\n"\
"   outTexturePos = texturePos;\n"\
"   outColor = inColor;\n"\
"}\n"\
"";

const char* frag =\
"#version 330 core\n"\
"\n"\
"in vec2 outTexturePos;\n"\
"in vec4 outColor;\n"\
"\n"\
"out vec4 fragColor;\n"\
"\n"\
"uniform sampler2D charTexture;\n"\
"\n"\
"void main(){\n"\
"   vec4 textureColor = texture2D(charTexture, outTexturePos);\n"\
"   fragColor = textureColor * outColor;\n"\
"}\n"\
"";
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
        SDL_Surface* s1 = TTF_RenderGlyph_Blended(font, c, {0xFF, 0xFF, 0xFF, 0xFF});
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
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)finalSurface->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(finalSurface);

    program = CreateShaderNoFile(vert, frag);
    
    glGenBuffers(1, &vbo);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glBindVertexArray(0);
}

void Font::RenderText(std::string text, double x, double y, double textHeight, SDL_Color color){
    int screenW, screenH;
    glfwGetWindowSize(glfwGetCurrentContext(), &screenW, &screenH);
    float aspectRatio = screenH / (float)screenW;

    glUseProgram(program);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1f(glGetUniformLocation(program, "charTexture"), 0);
    glBindVertexArray(vao);

    int vboCounter = 0;
    int charCounter = 0;

    float vx2 = x;
    for(char c : text){
        if(c == '\n'){
            vx2 = x;
            y -= textHeight * 2.0;
            continue;
        }

        auto rect = rects[(unsigned char)c];
        if(rect.w <= 0 || rect.h <= 0)
            continue;

        float tx1 = rect.x / (float)w;
        float ty1 = rect.y / (float)h;
        float tx2 = (rect.x + rect.w) / (float)w;
        float ty2 = (rect.y + rect.h) / (float)h;

        float vx1 = vx2;
        vx2 += (rect.w * (2.0*textHeight / rect.h)) * aspectRatio;
        float vy1 = y;
        float vy2 = y + 2.0*textHeight;

        Vertex v1 = {vx2, vy1, tx2, ty2, color.r/(float)256, color.g/(float)256, color.b/(float)256, color.a/(float)256};
        Vertex v2 = {vx1, vy1, tx1, ty2, color.r/(float)256, color.g/(float)256, color.b/(float)256, color.a/(float)256};
        Vertex v3 = {vx1, vy2, tx1, ty1, color.r/(float)256, color.g/(float)256, color.b/(float)256, color.a/(float)256};
        Vertex v4 = {vx2, vy2, tx2, ty1, color.r/(float)256, color.g/(float)256, color.b/(float)256, color.a/(float)256};

        vertices[vboCounter++] = v1;
        vertices[vboCounter++] = v2;
        vertices[vboCounter++] = v3;
        vertices[vboCounter++] = v4;

        charCounter ++;
        if(charCounter >= RENDER_BATCH_SIZE){
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

            glDrawArrays(GL_QUADS, 0, RENDER_BATCH_SIZE * 4);

            charCounter = 0;
            vboCounter = 0;
        }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_QUADS, 0, charCounter*4);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // __FONT_H__