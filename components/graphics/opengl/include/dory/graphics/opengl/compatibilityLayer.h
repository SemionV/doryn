#pragma once

#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif

#include "caps.h"

namespace dory::graphics::opengl
{
    class CompatibilityLayer
    {
    private:
        GLCaps _caps;
    public:
        CompatibilityLayer();

        void createBuffer(GLuint* out) const;
        void namedBufferStorage(GLenum target, GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags) const;
        void* mapNamedBuffer(GLenum target, GLuint buffer, GLenum access) const;
        bool  unmapNamedBuffer(GLenum target, GLuint buffer) const;
        void namedBufferSubData(GLenum target, GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) const;
        void createVertexArray(GLuint* out) const;

        void vaoBindVertexBuffer(GLuint vao, GLuint bindingIndex,
                             GLuint buffer, GLintptr offset, GLsizei stride) const;

        void vaoAttribFormat(GLuint vao, GLuint attribIndex,
                             GLint size, GLenum type, GLboolean normalized,
                             GLuint relativeOffset) const;

        void vaoAttribBinding(GLuint vao, GLuint attribIndex,
                              GLuint bindingIndex) const;

        void vaoEnableAttrib(GLuint vao, GLuint attribIndex) const;

        void vaoSetElementBuffer(GLuint vao, GLuint elementBuffer) const;

        // Optional helper: applies one attribute in one call on 4.1
        void vaoSetAttribPointerCompat(GLuint vao,
                                       GLuint attribIndex,
                                       GLuint vbo,
                                       GLint size, GLenum type, GLboolean normalized,
                                       GLsizei stride,
                                       GLintptr absoluteOffset) const;
    };
}