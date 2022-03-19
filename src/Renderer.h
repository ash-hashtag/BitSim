#pragma once

#include <GL/glew.h>
#include <iostream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));\

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    Renderer();
    ~Renderer();
    void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
    void clear();
    void unbindall();
};