#pragma once
#include <SDL2/SDL.h>
#include <gfx/shader/ShaderInput.hpp>
#include <memory>
#include <Window.hpp>

namespace gfx::shader::input
{
    class ResolutionUniform : public gfx::shader::ShaderInput {
    public:
        ResolutionUniform(std::shared_ptr<Window> window)
        {
            m_window = window;
        }

        void Update(){ }

        void ApplyUniform(GLuint program)
        {
            GLint uniformLoc = glGetUniformLocation(program, "iResolution");
            if(uniformLoc != -1)
                glUniform3f(uniformLoc, (GLfloat)*m_window->getWidth(), (GLfloat)*m_window->getHeight(), 1.f);
        }
    private:
        std::shared_ptr<Window> m_window;
    };
}
