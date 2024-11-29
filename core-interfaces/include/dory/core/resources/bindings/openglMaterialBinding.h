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
        std::vector<GLuint> memberIndices;
        std::vector<GLint> memberCounts;
        std::vector<GLint> memberOffsets;
        std::vector<GLint> memberTypes;
    };

    struct OpenglMaterialBinding: public MaterialBinding
    {
        GLuint glProgramId {};
        std::unordered_map<std::size_t, GLint> uniformLocations {};
        std::unordered_map<std::size_t, UniformBlockBinding> uniformBlocks {};
        std::size_t uniformBlockBufferSize {};
        GLuint uniformBlockBufferId;
    };
}