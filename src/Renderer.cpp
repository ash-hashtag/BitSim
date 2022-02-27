#include <iostream>
#include "Renderer.h"

void GLClearError() {
    while (!glGetError);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL error]: " << error << " " << function << " @ " << file << " in line " << line << std::endl;
        return false;
    }
    return true;
}
