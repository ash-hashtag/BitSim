#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <chrono>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


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


struct Sources{
    std::string VertexSource;
    std::string FragmentSource;
};


static Sources ParseShader(const std::string path)
{
    enum class shaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::fstream shaderfile;

    shaderfile.open(path, std::ios::in);
    std::string src[2];
    if (shaderfile.is_open())
    {
        std::string line;
        src[0] = "";
        src[1] = "";
        shaderType type = shaderType::NONE;
        while (getline(shaderfile, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("Vertex") != std::string::npos)
                    type = shaderType::VERTEX;
                else if (line.find("Fragment") != std::string::npos)
                    type = shaderType::FRAGMENT;
            }
            else {
                src[(int)type] += line + "\n";
            }
        }
    }
    return { src[0], src[1] };
}



static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length + sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader " << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;

    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
    unsigned int program = glCreateProgram();

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);

    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

        float positions[8] = {
            -0.5, -0.5,
            0.5, -0.5,
            0.5, 0.5,
            -0.5, 0.5
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vao;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        VertexBuffer vb(positions, 8 * sizeof(float));
        /*
        unsigned int m_RendererID;
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);
        */
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

        IndexBuffer ib(indices, 6);

        Sources src = ParseShader("resources\\Shaders\\Basic.Shader");
        unsigned int shader = CreateShader(src.VertexSource, src.FragmentSource);
        glUseProgram(shader);

        int u_loc = glGetUniformLocation(shader, "u_color");
        ASSERT(u_loc != -1);
        float r = 0.0f, inc = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shader);
            glUniform4f(u_loc, r, 0.3f, r, 1.0f);
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            //glDrawArrays(GL_POLYGON, 0, 4);
            if (r > 1.0f)
                inc = -0.05f;
            else if (r < 0.0f)
                inc = 0.05f;
            r += inc;
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();


        }

        glDeleteProgram(shader);
    }
    glfwTerminate();

    return 0;
}