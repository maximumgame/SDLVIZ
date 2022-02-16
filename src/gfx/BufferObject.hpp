#pragma once
#include <gfx/gl.h>

namespace gfx
{
    class BufferObject {
    public:
        BufferObject(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
        {
            glGenBuffers(1, &m_buffer);
            glBindBuffer(target, m_buffer);
            glBufferData(target, size, data, usage);
            glBindBuffer(target, 0);
        }
        BufferObject(const BufferObject &) = delete;
        BufferObject &operator=(const BufferObject &) = delete;

        BufferObject(BufferObject &&other) noexcept : m_buffer(other.Release()) {}

        BufferObject &operator=(BufferObject &&other) noexcept
        {
            Reset(other.Release());
            return *this;
        }

        //Deletes buffer from gpu
        GLuint Reset(GLuint obj = 0)
        {
            glDeleteBuffers(1, &obj); //can be called with 0
            m_buffer = obj;
            return m_buffer;
        }

        ~BufferObject()
        {
            Reset();
        }

        //relinquish ownership of the buffer object without resetting it
        GLuint Release()
        {
            GLuint ret = m_buffer;
            m_buffer = 0;
            return ret;
        }

        void Bind(GLenum target) const
        {
            glBindBuffer(target, m_buffer);
        }
        GLuint GetGLObject() const { return m_buffer; }

    private:
        GLuint m_buffer;
    };
}
