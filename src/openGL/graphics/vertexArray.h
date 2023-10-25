#pragma once

#include "openGL/dependencies.h"
#include "blocks.h"

namespace dory::openGL::graphics
{
    struct VertexAttribute
    {
        GLint count;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void* pointer;

        VertexAttribute() = default;

        VertexAttribute(GLint count, GLenum type, GLboolean normalized = GL_FALSE, GLsizei stride = 0, const void* pointer = nullptr):
            count(count), type(type), normalized(normalized), stride(stride), pointer(pointer)
        {}
    };

    template<std::size_t NAttributes>
    struct VertexArray
    {
        GLuint id = unboundId;
        Buffer buffer;
        std::array<VertexAttribute, NAttributes> vertexAttributes;
        std::size_t verticesCount {};

        VertexArray() = default;

        VertexArray(const VertexAttribute (&attributes)[NAttributes])
        {
            for(std::size_t i = 0; i < NAttributes; ++i)
            {
                vertexAttributes[i] = attributes[i];
            }
        }
    };
}