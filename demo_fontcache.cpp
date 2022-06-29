#include "Font.h"
#include <iostream>
#include <sstream>

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "No fontfile given!!\nCorrect way to run demo: " << argv[0] << " path_to_font.ttf " << std::endl;
        exit(0);
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_ALPHA_BITS, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Fonts", 0, 0);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize font
    const char* fontfile = argv[1];
    const int fontResolution = 40;
    auto font = new Font(fontfile, fontResolution);

    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS){
        glClearColor(0.2, 0.1, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        static double lastTime = glfwGetTime();
        double time = glfwGetTime();
        double dt = time - lastTime;
        lastTime = time;

        // Demo font rendering
        std::string text = "FPS: ";
        std::stringstream ss;
        ss << (1.0 / dt);
        text += ss.str();
        static std::string gibberish;
        gibberish += uint8_t(rand() % 256);
        if(gibberish.length() > 70) gibberish.clear();
        static double x = -0.75, y = -0.1;
        static double vx = -0.7, vy = -1.4;
        double dx = vx * dt, dy = (vy += -10.0 * dt) * dt;
        x += dx; y += dy;
        if(x < -1.0 || x > 0.4){ x -= dx; vx *= -1.0; }
        if(y < -1.0 || y > 0.75){ y -= dy; vy *= -1.0; }
        std::string a_lot_of_text = "This should be a lot of text:\n\n";
        for(int i=0; i<50; i++){for(int j=0; j<50; j++){ a_lot_of_text += ('a' + j % 26); } a_lot_of_text += '\n'; }
        font->RenderText(text, -1.0, -1.0, 0.1, {0xFF, 0xFF, 0xFF, 0xFF});
        font->RenderText("Hello", x, y, 0.15, {0xFF, 0, 0, uint8_t(0xFF/2 + 0xFF/2*sin(10.0*glfwGetTime()))});
        font->RenderText("      world!", -0.75, -0.1, 0.15, {0xFF, 0, 0, 0xFF});
        font->RenderText(gibberish, -1.0, 0.8, 0.05, {0, 0xFF, 0, 0xFF});
        font->RenderText(a_lot_of_text, -1.0, 0.6, 0.04, {0xFF, 0xFF, 0xFF, 0x22});

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
