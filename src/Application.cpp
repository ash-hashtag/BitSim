#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>


typedef struct {
    std::string VertexSource;
    std::string FragmentSource;
}Sources;



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

    //initialise GLEW
    if (glewInit() != GLEW_OK)
        std::cout << "glew isn't opening\n";


    const float positions[] = {
        -0.5, -0.5,
        0.5, -0.5,
        0.5, 0.5,
        -0.5, 0.5
    };
    
    const unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), positions, GL_STATIC_DRAW);
    
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);



    Sources src = ParseShader("resources\\Shaders\\Basic.Shader");

    unsigned int shader = CreateShader(src.VertexSource, src.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_POLYGON, 0, 4);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        glfwSwapInterval(1);

    }

    glDeleteProgram(shader);
    glfwTerminate();

    return 0;
}