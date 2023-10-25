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

    template<std::size_t MembersCount>
    struct UniformBlock
    {
        std::string key;
        GLuint index = unboundId;
        GLint size = 0;

        std::array<Uniform, MembersCount> members;
        Buffer buffer;

        UniformBlock(const std::string_view& blockName):
            key(blockName)
        {}

        template<typename... T>
        UniformBlock(const std::string_view& blockName, T... members):
            key(blockName),
            members{members...}
        {}

        UniformBlock(const std::string_view& blockName, const std::string_view(&memberNames)[MembersCount]):
            key(blockName)
        {
            for(std::size_t i; i < MembersCount; ++i)
            {
                members[i] = memberNames[i];
            }
        }
    };
}