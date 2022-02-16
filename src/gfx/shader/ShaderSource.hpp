#pragma once
#include <gfx/gl.h>
#include <memory>
#include <spdlog/spdlog.h>

namespace gfx::shader
{
    class ShaderSource {
    public:
        ShaderSource(GLenum shaderType, const std::string &source)
        {
            m_shader = glCreateShader(shaderType);
            const GLchar* shaderSource[] = { source.c_str() };
            glShaderSource(m_shader, 1, shaderSource, NULL);
            glCompileShader(m_shader);
            glGetShaderiv(m_shader, GL_COMPILE_STATUS, &m_shaderCompileStatus);
            if (m_shaderCompileStatus != GL_TRUE) {
                spdlog::error("Failed to compile shader");
                GLint logLength;
                glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 0) {
                    std::shared_ptr<GLchar[]> log(new GLchar[logLength]);
                    GLsizei written;
                    glGetShaderInfoLog(m_shader, logLength, &written, log.get());
                    spdlog::error("Shader compile log: {}", log.get());
                    m_shaderLog = log;
                }
            }
        }
        ShaderSource(const ShaderSource &) = delete;
        ShaderSource &operator=(const ShaderSource &) = delete;

        ShaderSource(ShaderSource &&other) noexcept : m_shader(other.Release()) {}

        ShaderSource &operator=(ShaderSource &&other) noexcept
        {
            Reset(other.Release());
            return *this;
        }

        GLuint Release()
        {
            GLuint ret = m_shader;
            m_shader = 0;
            return ret;
        }

        //Deletes shader from gpu
        GLuint Reset(GLuint obj = 0)
        {
            glDeleteShader(obj); //can be called with 0
            m_shader = obj;
            return m_shader;
        }

        ~ShaderSource()
        {
            Reset();
        };

        GLuint GetShader() const { return m_shader; }
        GLint ShaderCompileStatus() const { return m_shaderCompileStatus; }
        GLint ShaderSuccess() const { return m_shaderCompileStatus == GL_TRUE; }
        std::shared_ptr<GLchar[]> GetShaderLog() const { return m_shaderLog; }
    private:
        GLuint m_shader;
        GLint m_shaderCompileStatus;
        std::shared_ptr<GLchar[]> m_shaderLog;
    };
}
