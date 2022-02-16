#pragma once
#include <glad/glad.h>

namespace gfx::shader
{
    class ShaderInput {
    public:
        virtual void Update() = 0;

        virtual void ApplyUniform(GLuint program) = 0;
    };
}
