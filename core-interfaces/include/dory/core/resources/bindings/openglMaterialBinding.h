#pragma once

#include "materialBinding.h"
#include <unordered_map>

namespace dory::core::resources::bindings
{
    struct UniformBlockBinding
    {
        GLuint blockIndex {};
        GLint blockSize;
        std::size_t bufferOffset {};
        unsigned int bindingPoint {};
        std::vector<GLuint> memberIndices;
        std::vector<GLint> memberCounts;
        std::vector<GLint> memberOffsets;
        std::vector<GLint> memberTypes;
    };

    struct UniformBinding
    {
        std::unordered_map<std::size_t, GLint> locations {};
        std::unordered_map<std::size_t, UniformBlockBinding> blocks {};
        std::size_t blockBufferSize {};
        GLuint blockBufferId;
    };

    struct OpenglMaterialBinding: public MaterialBinding
    {
        GLuint glProgramId {};
        UniformBinding dynamicUniforms;
        UniformBinding staticUniforms;
    };
}