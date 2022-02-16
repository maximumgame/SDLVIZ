#pragma once
#include <gfx/shader/ShaderInput.hpp>
#include <chrono>

namespace gfx::shader::input {
    class TimeUniform : public gfx::shader::ShaderInput {
    public:
        TimeUniform()
        {
            time = 0.f;
            last = std::chrono::steady_clock::now();
        }

        void Update()
        {
            auto now = std::chrono::steady_clock::now();
            time += std::chrono::duration_cast<std::chrono::duration<float>>(now - last).count();
            last = now;
        }

        void ApplyUniform(GLuint program)
        {
            GLint loc = glGetUniformLocation(program, "iTime");
            if(loc != -1)
                glUniform1f(loc, time);
        }
    private:
        float time;
        std::chrono::steady_clock::time_point last;
    };
}