#pragma once

#include <dory/engine/resources/opengl/blocks.h>

namespace dory::opengl::services
{
    graphics::Buffer allocateBuffer(GLint bufferSize);
    void deallocateBuffer(graphics::Buffer& buffer) noexcept;

    class BufferBlockService
    {
        public:
            static void getBlockMemberNames(const graphics::UniformBlock& block, 
                const char** memberNames) noexcept
            {
                auto membersCount = block.getMembersCount();
                auto members = block.getMembers();

                for(std::size_t i = 0; i < membersCount; ++i)
                {
                    memberNames[i] = members[i].key.c_str();
                }
            }
    };
}