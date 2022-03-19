#include "Shader.h"
#include "Renderer.h"
#include <fstream>

Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource src = ParseShader(filepath);
    m_RendererID = CreateShader(src.VertexSource, src.FragmentSource);

}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end());
        return m_uniformLocationCache[name];
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform " << name << " doesn't exit! " << std::endl;
    m_uniformLocationCache[name] = location;
    return location;
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4 matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

ShaderProgramSource Shader::ParseShader(const std::string path)
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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
