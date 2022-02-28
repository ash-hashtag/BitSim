#include "Renderer.h"

void GLClearError() {
    int i = 0;
    while (!glGetError())
        i++;
    std::cout << "Found " << i << " errors" << std::endl;
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL error]: " << error << " " << function << " @ " << file << " in line " << line << std::endl;
        break;
    }
    return true;
}

void Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}
/*
void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}*/
