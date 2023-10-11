#pragma once

#include "openGL/dependencies.h"
#include "blocks.h"

namespace dory::openGL::graphics
{
    class OpenglProcedures
    {
        public:
            template<std::size_t N>
            static void bindBlock(GLuint programId, UniformBlock<N>& block)
            {
                block.index = glGetUniformBlockIndex(programId, block.key.c_str());
                if(block.index != GL_INVALID_INDEX)
                {
                    glGetActiveUniformBlockiv(programId, block.index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.size);
                    
                    GLuint memberIndices[N];
                    GLint memberOffset[N];
                    GLint memberSize[N];
                    GLint memberType[N];

                    const char* memberNames[N];
                    getBlockMemberNames(block, memberNames);

                    glGetUniformIndices(programId, N, memberNames, memberIndices);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_OFFSET, memberOffset);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_SIZE, memberSize);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_TYPE, memberType);

                    for(std::size_t i = 0; i < N; ++i)
                    {
                        Uniform& member = block.members[i];
                        member.index = memberIndices[i];
                        member.offset = memberOffset[i];
                        member.type = memberType[i];
                        member.count = memberSize[i];

                        member.size = getOpenGLTypeSize(member.type) * member.count;
                    }
                }
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

            template<std::size_t MembersCount>
            static std::size_t getBlockDataSize(const UniformBlock<MembersCount>& block)
            {
                std::size_t size = 0;

                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    auto member = block.members[i];
                    size += getOpenGLTypeSize(member.type) * member.size;
                }

                return size;
            }
    };
}