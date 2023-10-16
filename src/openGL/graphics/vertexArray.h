#pragma once

#include "openGL/dependencies.h"

namespace dory::openGL::graphics
{
    struct VertexAttribute
    {
        GLuint id;
        GLint count;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void* pointer;

        VertexAttribute(GLuint id, GLint count, GLenum type, GLboolean normalized = GL_FALSE, GLsizei stride = 0, const void* pointer = nullptr):
            id(id), count(count), type(type), normalized(normalized), stride(stride), pointer(pointer)
        {}
    };

    struct VertexArray
    {
        GLuint id;
        Buffer vertexBuffer;
        std::vector<VertexAttribute> vertexAttributes;
    };
}