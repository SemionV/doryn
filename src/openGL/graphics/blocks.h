#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"

namespace dory::openGL::graphics
{
    static constexpr GLuint unboundId = GL_INVALID_INDEX;

    struct Buffer
    {
        GLuint index = unboundId;
        GLsizeiptr size = 0;
        GLvoid* data = nullptr;
    };

    struct Uniform
    {
        std::string key;
        GLuint index = unboundId;
        GLint count = 0;
        GLint offset = 0;
        GLint size = 0;
        GLint type = 0;

        Uniform() = default;

        Uniform(const std::string_view& blockName):
            key(blockName)
        {}
    };

    struct UniformBlock
    {
        std::string key;
        GLuint index = unboundId;
        GLint size = 0;
        Buffer buffer;

        UniformBlock(const std::string_view& blockName):
            key(blockName)
        {}

        virtual std::size_t getMembersCount() = 0;
        virtual Uniform* getMembers() = 0;
    };

    template<std::size_t MembersCount>
    struct UniformArrayBlock: public UniformBlock
    {
        std::array<Uniform, MembersCount> members;

        UniformArrayBlock(const std::string_view& blockName):
            UniformBlock(blockName)
        {}

        template<typename... T>
        UniformArrayBlock(const std::string_view& blockName, T... members):
            UniformBlock(blockName),
            members{members...}
        {}

        UniformArrayBlock(const std::string_view& blockName, const std::string_view(&memberNames)[MembersCount]):
            UniformBlock(blockName)
        {
            for(std::size_t i; i < MembersCount; ++i)
            {
                members[i] = memberNames[i];
            }
        }

        std::size_t getMembersCount() noexcept override
        {
            return members.size();
        }

        Uniform* getMembers() noexcept override
        {
            return members.data();
        }
    };
}