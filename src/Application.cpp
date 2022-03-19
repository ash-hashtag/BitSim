#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <chrono>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"


#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBufferLayout.h"

struct Timer {
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        float ms = ((end - start).count());
        std::cout << "MS: " << ms <<std::endl;
    }
};



int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(960, 540, "BitSim", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwWindowHint(GL_MAJOR_VERSION, 3);
    glfwWindowHint(GL_MINOR_VERSION, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //initialise GLEW
    if (glewInit() != GLEW_OK)
        std::cout << "glew isn't opening\n";
    {
        /*
        const float positions[16] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f
        }; */
        
        const float positions[16] = {
            -50.0f, -50.0f, 0.0f, 0.0f,
            50.0f, -50.0f, 1.0f, 0.0f,
            50.0f, 50.0f, 1.0f, 1.0f,
            -50.0f, 50.0f, 0.0f, 1.0f
        };
        /*
        const float positions[16] = {
            1.0f, 1.0f, 0.0f, 0.0f,
            2.0f, 1.0f, 1.0f, 0.0f,
            2.0f, 2.0f, 1.0f, 1.0f,
            1.0f, 2.0f, 1.0f, 1.0f
        };*/


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

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);

        Shader shader("resources/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_color", 0.0f, 0.3f, 0.8f, 0.1f);


        Texture texture("resources/textures/rash.png");
        texture.Bind(0);
        shader.SetUniform1i("u_Texture", 0);

        shader.Unbind();
        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        GLClearError();

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();


        Renderer renderer;
        while (!glfwWindowShouldClose(window))
        {
            //Timer timer;
            renderer.clear();

            ImGui_ImplGlfwGL3_NewFrame();
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;

                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.draw(va, ib, shader);
            }            
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;

                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.draw(va, ib, shader);
            }
            //glDrawArrays(GL_POLYGON, 0, 4);
            {
                ImGui::SliderFloat3("translation A: ", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
                ImGui::SliderFloat3("translation B: ", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


            glfwSwapBuffers(window);
            glfwPollEvents();

        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}