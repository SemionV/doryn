#pragma once

#include "blocks.h"

namespace dory::opengl::graphics
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

    class VertexArray
    {
        public:
            GLuint id = unboundId;
            Buffer buffer;
            std::size_t verticesCount {};

            virtual const std::size_t getAttributesCount() const = 0;
            virtual std::size_t getAttributesCount() = 0;
            virtual const VertexAttribute* getAttributes() const = 0;
            virtual VertexAttribute* getAttributes() = 0;
    };

    template<std::size_t NAttributes>
    class AttributedVertexArray: public VertexArray
    {
        private:
            std::array<VertexAttribute, NAttributes> vertexAttributes;

        public:
            AttributedVertexArray() = default;

            AttributedVertexArray(const VertexAttribute (&attributes)[NAttributes])
            {
                for(std::size_t i = 0; i < NAttributes; ++i)
                {
                    vertexAttributes[i] = attributes[i];
                }
            }

            const std::size_t getAttributesCount() const noexcept override
            {
                return vertexAttributes.size();
            }

            const VertexAttribute* getAttributes() const noexcept override
            {
                return vertexAttributes.data();
            }

            std::size_t getAttributesCount() noexcept override
            {
                return vertexAttributes.size();
            }

            VertexAttribute* getAttributes() noexcept override
            {
                return vertexAttributes.data();
            }
    };
}