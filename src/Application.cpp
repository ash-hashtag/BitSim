#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <chrono>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

struct Timer {
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        float ms = (end - start).count() * 1000.0f;
        std::cout << "Timer took " << ms << "ms" << std::endl;

    }
};


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "BitSim", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwWindowHint(GL_MAJOR_VERSION, 3);
    glfwWindowHint(GL_MINOR_VERSION, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //initialise GLEW
    if (glewInit() != GLEW_OK)
        std::cout << "glew isn't opening\n";
    {

        const float positions[16] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f
        };

        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        VertexArray va;
        VertexBufferLayout layout;
        VertexBuffer vb(positions, 16 * sizeof(float));
        
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        Shader shader("resources/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_color", 0.0f, 0.3f, 0.8f, 1.0f);
        
        Texture texture("resources/textures/cins.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        shader.Unbind();
        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        float r = 0.0f, inc = 0.05f;

        GLClearError();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            Clear();

            va.Bind();
            ib.Bind();
            shader.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            //glDrawArrays(GL_POLYGON, 0, 4);
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}