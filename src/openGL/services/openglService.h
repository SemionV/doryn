#pragma once

#include "openGL/dependencies.h"
#include "bufferBlockService.h"
#include "openGL/graphics/blocks.h"
#include "openGL/graphics/program.h"
#include "openGL/graphics/vertexArray.h"

namespace dory::openGL::services
{
    class OpenglService
    {
        private:
            template<std::size_t NAttributes>
            static std::size_t getSingleVertexSize(graphics::VertexArray<NAttributes>& vertexArray) noexcept
            {
                std::size_t size {};
                for(std::size_t i = 0; i < NAttributes; ++i)
                {
                    auto attribute = vertexArray.vertexAttributes[i];
                    size += getOpenGLTypeSize(attribute.type) * attribute.count;
                }

                return size;
            }

            template<std::size_t NAttributes>
            static std::size_t getVerticiesCount(graphics::VertexArray<NAttributes>& vertexArray) noexcept
            {
                auto vertexSize = getSingleVertexSize(vertexArray);
                if(vertexSize > 0)
                {
                    return vertexArray.buffer.size / vertexSize;
                }

                return 0;
            }

            // Helper function to convert GLSL types to storage sizes
            static std::size_t getOpenGLTypeSize(GLenum type)
            {
                std::size_t size = 0;

                #define CASE(Enum, Count, Type) \
                case Enum: size = Count * sizeof(Type); break
                
                switch (type)
                {
                    CASE(GL_FLOAT, 1, GLfloat);
                    CASE(GL_FLOAT_VEC2, 2, GLfloat);
                    CASE(GL_FLOAT_VEC3, 3, GLfloat);
                    CASE(GL_FLOAT_VEC4, 4, GLfloat);
                    CASE(GL_INT, 1, GLint);
                    CASE(GL_INT_VEC2, 2, GLint);
                    CASE(GL_INT_VEC3, 3, GLint);
                    CASE(GL_INT_VEC4, 4, GLint);
                    CASE(GL_UNSIGNED_INT, 1, GLuint);
                    CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
                    CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
                    CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
                    CASE(GL_BOOL, 1, GLboolean);
                    CASE(GL_BOOL_VEC2, 2, GLboolean);
                    CASE(GL_BOOL_VEC3, 3, GLboolean);
                    CASE(GL_BOOL_VEC4, 4, GLboolean);
                    CASE(GL_FLOAT_MAT2, 4, GLfloat);
                    CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
                    CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
                    CASE(GL_FLOAT_MAT3, 9, GLfloat);
                    CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
                    CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
                    CASE(GL_FLOAT_MAT4, 16, GLfloat);
                    CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
                    CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
                    #undef CASE
                    default:
                        fprintf(stderr, "Unknown type: 0x%x\n", type);
                    break;
                }

                return size;
            }

            static void useProgram(const graphics::Program& program)
            {
                glUseProgram(program.id);
            }

            template<std::size_t NAttributes>
            static void useVertextArray(const graphics::VertexArray<NAttributes>& vertexArray)
            {
                glBindVertexArray(vertexArray.id);
            }

        public:
            static void clearViewport(const domain::Color& clearColor)
            {
                GLfloat color[] = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
                glClearBufferfv(GL_COLOR, 0, color);
            }

            template<std::size_t N>
            static void loadUniformBlock(GLuint programId, graphics::UniformBlock<N>& block)
            {
                block.index = glGetUniformBlockIndex(programId, block.key.c_str());
                if(block.index != graphics::unboundId)
                {
                    glGetActiveUniformBlockiv(programId, block.index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.size);
                    
                    GLuint memberIndices[N];
                    GLint memberOffset[N];
                    GLint memberSize[N];
                    GLint memberType[N];

                    const char* memberNames[N];
                    BufferBlockService::getBlockMemberNames(block, memberNames);

                    glGetUniformIndices(programId, N, memberNames, memberIndices);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_OFFSET, memberOffset);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_SIZE, memberSize);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_TYPE, memberType);

                    std::size_t size {};
                    for(std::size_t i = 0; i < N; ++i)
                    {
                        graphics::Uniform& member = block.members[i];
                        member.index = memberIndices[i];
                        member.offset = memberOffset[i];
                        member.type = memberType[i];
                        member.count = memberSize[i];

                        member.size = getOpenGLTypeSize(member.type) * member.count;
                        size += member.size;
                    }
                    block.size = size;
                }
            }

            template<std::size_t MembersCount>
            static void setUniformBlockData(graphics::UniformBlock<MembersCount>& block, GLvoid* data, GLsizeiptr dataSize)
            {
                block.buffer.data = data;
                block.buffer.size = dataSize;

                if(block.buffer.index != graphics::unboundId)
                {
                    glDeleteBuffers(1, &block.buffer.index);
                    block.buffer.index = graphics::unboundId;
                }

                glCreateBuffers(1, &block.buffer.index);
                if(block.buffer.index != graphics::unboundId)
                {
                    glBindBufferBase(GL_UNIFORM_BUFFER, block.index, block.buffer.index);

                    glBindBuffer(GL_UNIFORM_BUFFER, block.buffer.index);
                    glBufferStorage(GL_UNIFORM_BUFFER, block.buffer.size, block.buffer.data, 0);
                }
            }

            template<std::size_t NAttributes>
            static void loadVertexArray(graphics::VertexArray<NAttributes>& vertexArray)
            {
                glGenVertexArrays(1, &vertexArray.id);
            }

            template<std::size_t NAttributes>
            static void setVertexArrayData(graphics::VertexArray<NAttributes>& vertexArray, GLvoid* data, GLsizeiptr dataSize) noexcept
            {
                vertexArray.buffer.data = data;
                vertexArray.buffer.size = dataSize;
                vertexArray.verticesCount = getVerticiesCount(vertexArray);

                glBindVertexArray(vertexArray.id);

                if(vertexArray.buffer.index != graphics::unboundId)
                {
                    glDeleteBuffers(1, &vertexArray.buffer.index);
                    vertexArray.buffer.index = graphics::unboundId;
                }

                glCreateBuffers(1, &vertexArray.buffer.index);
                if(vertexArray.buffer.index != graphics::unboundId)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, vertexArray.buffer.index);
                    glBufferStorage(GL_ARRAY_BUFFER, dataSize, data, 0);
                }

                for(std::size_t i = 0; i < vertexArray.vertexAttributes.size(); ++i)
                {
                    graphics::VertexAttribute& attribute = vertexArray.vertexAttributes[i];
                    glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
                    glEnableVertexAttribArray(i);
                }
            }

            static void drawObject(const graphics::Program& program, const TrianglesVertexArray& vertexArray)
            {
                useProgram(program);
                useVertextArray(vertexArray);

                glDrawArrays(GL_TRIANGLES, 0, vertexArray.verticesCount);
            }
    };
}