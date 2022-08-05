#pragma once
#include <gfx/shader/ShaderSource.hpp>
#include <gfx/BufferObject.hpp>

class ShaderBuffer{
public:
    ShaderBuffer(const std::string& source)
    {
        m_program = glCreateProgram();
        m_vertexShader = std::make_unique<gfx::shader::ShaderSource>(GL_VERTEX_SHADER, "#version 300 es\nin vec2 fragColor; void main() { gl_Position = vec4( fragColor.x, fragColor.y, 0, 1 ); }");
        m_fragmentShader = std::make_unique<gfx::shader::ShaderSource>(GL_FRAGMENT_SHADER, source);
    }

    ShaderBuffer(const ShaderBuffer &) = delete;
    ShaderBuffer &operator=(const ShaderBuffer &) = delete;

    ~ShaderBuffer()
    {
        glDeleteProgram(m_program);
    }
private:
    GLuint m_program;
    std::unique_ptr<gfx::shader::ShaderSource> m_vertexShader;
    std::unique_ptr<gfx::shader::ShaderSource> m_fragmentShader;
    std::vector<std::shared_ptr<gfx::BufferObject>> m_bufferObjects;
};