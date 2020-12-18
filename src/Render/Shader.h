//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;

    // caching for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& Filepath);
    ~Shader();

    void bind() const;
    void unbind() const;
    inline unsigned int getRendererID() const
    {
        return m_RendererID;
    }

    void setUniform1i(const std::string Name, int Value);
    void setUniform1iv(const std::string Name, const int Count, const int *Value);
    void setUniform1f(const std::string Name, float Value);
    void setUniform4f(const std::string Name, float v0, float v1, float v2, float v3);
    void setUniformMat4f(const std::string Name, glm::mat4& matrix);

private:
    int getUniformLocation(const std::string& name);
    ShaderProgramSource parseShader(const std::string &File);
    unsigned int compileShader(unsigned int type, const std::string &source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
};