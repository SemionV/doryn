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
    };

    template<std::size_t MembersCount>
    struct UniformBlock: public DataBinding
    {
        std::array<Uniform, MembersCount> members;

        UniformBlock(const std::string_view& blockName):
            DataBinding(blockName)
        {}
    };
}