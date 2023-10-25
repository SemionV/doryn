#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"

namespace dory::openGL::graphics
{
    static constexpr GLuint unboundId = GL_INVALID_INDEX;

    struct DataBinding
    {
        std::string key;
        GLuint index = unboundId;
        GLint count = 0;
        GLint offset = 0;
        GLint size = 0;

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

        Uniform() = default;

        Uniform(const std::string_view& blockName):
            DataBinding(blockName)
        {}
    };

    template<std::size_t MembersCount>
    struct UniformBlock: public DataBinding
    {
        std::array<Uniform, MembersCount> members;
        Buffer buffer;

        UniformBlock(const std::string_view& blockName):
            DataBinding(blockName)
        {}

        template<typename... T>
        UniformBlock(const std::string_view& blockName, T... members):
            DataBinding(blockName),
            members{members...}
        {}

        UniformBlock(const std::string_view& blockName, const std::string_view(&memberNames)[MembersCount]):
            DataBinding(blockName)
        {
            for(std::size_t i; i < MembersCount; ++i)
            {
                members[i] = memberNames[i];
            }
        }
    };
}