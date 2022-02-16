#pragma once
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <memory>
#include <gfx/BufferObject.hpp>
#include <gfx/shader/ShaderSource.hpp>
#include <gfx/shader/input/TimeUniform.hpp>
#include <gfx/shader/input/ResolutionUniform.hpp>

namespace gfx
{
    class ShaderProgram {
    public:
        ShaderProgram(std::shared_ptr<Window> window);
        ~ShaderProgram();

        void render();

        std::shared_ptr<GLchar[]> LinkNewFragmentShader(const std::string &shaderSource);

    private:
        void Uniforms();

        float time;

        GLuint m_program;
        std::shared_ptr<gfx::shader::ShaderSource> m_vertexShader;
        std::shared_ptr<gfx::shader::ShaderSource> m_fragmentShader;

        std::shared_ptr<gfx::shader::input::TimeUniform> m_timeUniform;
        std::shared_ptr<gfx::shader::input::ResolutionUniform> m_resolutionUniform;

        std::shared_ptr<Window> m_window;

        //vertex array object
        GLuint m_vao;
        //vertex array object
        std::shared_ptr<gfx::BufferObject> m_vbo;
        //element buffer object
        std::shared_ptr<gfx::BufferObject> m_ebo;


        GLint m_gVertexPos2DLocation;
    };
}
