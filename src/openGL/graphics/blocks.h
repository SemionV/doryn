#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"

namespace dory::openGL::graphics
{
    struct DataBinding
    {
        std::string key;
        GLuint index = GL_INVALID_INDEX;
        GLint count = 0;
        GLint offset = 0;
        std::size_t size = 0;

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
        Uniform members[MembersCount];

        UniformBlock(const std::string_view& blockName):
            DataBinding(blockName)
        {}

        UniformBlock(const std::string_view& blockName, const Uniform(&members)[MembersCount]):
            DataBinding(blockName),
            members(members)
        {}
    };

    struct ColorsUniformBlock: public UniformBlock<3>
    {
        Uniform& brightColor;
        Uniform& hippieColor;
        Uniform& darkColor;

        ColorsUniformBlock():
            UniformBlock("ColorsBlock", {
                Uniform("ColorsBlock.brightColor"),
                Uniform("ColorsBlock.hippieColor"),
                Uniform("ColorsBlock.darkColor")
            }),
            brightColor(UniformBlock::members[0]),
            hippieColor(UniformBlock::members[1]),
            darkColor(UniformBlock::members[2])
        {}
    };
}