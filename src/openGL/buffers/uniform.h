#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"

namespace dory::openGL::buffers
{
    // Helper function to convert GLSL types to storage sizes
    std::size_t getOpenGLTypeSize(GLenum type)
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

    struct DataBinding
    {
        std::string key;
        GLuint index = GL_INVALID_INDEX;
        GLint size = 0;
        GLint offset = 0;

        DataBinding() {}

        DataBinding(const std::string_view& blockName):
            key(blockName.data())
        {}
    };

    struct Buffer: public DataBinding
    {
        GLvoid* data = nullptr;
    };

    struct Uniform: public DataBinding
    {
        GLint type = 0;
    };

    template<std::size_t MembersCount>
    struct UniformBlock: public DataBinding
    {
        std::array<Uniform, MembersCount> members;

        UniformBlock(const std::string_view& blockName):
            DataBinding(blockName)
        {}
    };

    template<std::size_t MembersCount>
    class BufferBlockBinder
    {
        private:
            UniformBlock<MembersCount> bufferBlockBinding;
            Buffer dataBuffer;

        private:
            template<std::size_t N>
            static constexpr void getBlockMemberNames(const UniformBlock<N>& block, const char*(&memberNames)[N]) noexcept
            {
                for(std::size_t i = 0; i < N; ++i)
                {
                    memberNames[i] = block.members[i].key.c_str();
                }
            }

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
                        member.size = memberSize[i];
                    }
                }
            }

        protected:
            static constexpr std::string_view pointLiteral = ".";

        public:
            explicit BufferBlockBinder(const std::string_view& blockName, 
                const std::string_view(&memberNames)[MembersCount]):
                bufferBlockBinding(blockName)
            {
                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    bufferBlockBinding.members[i].key = memberNames[i];
                }
            }

            void bind(GLuint programId)
            {
                bindBlock(programId, bufferBlockBinding);
            }

            std::size_t getBlockDataSize()
            {
                std::size_t size = 0;

                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    auto member = bufferBlockBinding.members[i];
                    size += getOpenGLTypeSize(member.type);
                }

                return size;
            }

            void bindToDataBuffer(Buffer&& buffer)
            {
                dataBuffer = std::move(buffer);

                glBindBufferBase(GL_UNIFORM_BUFFER, bufferBlockBinding.index, buffer.index);
            }
    };

    Buffer allocateBuffer(GLint bufferSize)
    {
        Buffer buffer;
        buffer.size = bufferSize;
        buffer.data = malloc(bufferSize);
        if(buffer.data == nullptr)
        {
            buffer.size = 0;
        }

        return buffer;
    }

    void deallocateBuffer(Buffer& buffer) noexcept
    {
        if(buffer.data && buffer.size)
        {
            free(buffer.data);
            buffer.data = 0;
            buffer.size = 0;
        }
    }

    class ColorsUniformBlockBinder: public BufferBlockBinder<3>
    {
        private:
            static constexpr std::string_view blockNameLiteral = "ColorsBlock";
            static constexpr std::string_view prefixLiteral = dory::domain::JoinStringLiterals<blockNameLiteral, pointLiteral>;

            static constexpr std::string_view brightColorLiteral = "brightColor";
            static constexpr std::string_view hippieColorLiteral = "hippieColor";
            static constexpr std::string_view darkColorLiteral = "darkColor";

            static constexpr auto brightColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, brightColorLiteral>;
            static constexpr auto hippieColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, hippieColorLiteral>;
            static constexpr auto darkColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, darkColorLiteral>;

        public:
            ColorsUniformBlockBinder():
                BufferBlockBinder(blockNameLiteral, {brightColorUniformName, hippieColorUniformName, darkColorUniformName})
            {
            }
    };
}