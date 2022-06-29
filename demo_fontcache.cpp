#include "Font.h"
#include <iostream>
#include <sstream>

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "No fontfile given!!\nCorrect way to run demo: " << argv[0] << " path_to_font.ttf " << std::endl;
        exit(0);
    }

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(500, 400, "Fonts", 0, 0);
    glfwMakeContextCurrent(window);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Initialize font
    const char* fontfile = argv[1];
    const int fontResolution = 50;
    auto font = new Font(fontfile, fontResolution);

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2, 0.1, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Demo font rendering
        std::string text = "Time: ";
        std::stringstream ss;
        ss << glfwGetTime();
        text += ss.str();
        static std::string gibberish;
        gibberish += uint8_t(rand() % 256);
        if(gibberish.length() > 70) gibberish.clear();
        font->RenderText(text, -1.0, -1.0, 0.1, {0xFF, 0xFF, 0xFF, 0xFF});
        font->RenderText("Hello world!", -0.75, -0.1, 0.15, {0xFF, 0, 0, uint8_t(0xFF/2 + 0xFF/2*sin(10.0*glfwGetTime()))});
        font->RenderText(gibberish, -1.0, 0.8, 0.05, {0, 0xFF, 0, 0xFF});
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
