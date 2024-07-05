#pragma once

#include "../graphics/blocks.h"

namespace dory::openGL::services
{
    graphics::Buffer allocateBuffer(GLint bufferSize)
    {
        graphics::Buffer buffer;
        buffer.data = malloc(bufferSize);
        if(buffer.data != nullptr)
        {
            buffer.size = bufferSize;
        }

        return buffer;
    }

    void deallocateBuffer(graphics::Buffer& buffer) noexcept
    {
        if(buffer.data)
        {
            free(buffer.data);
            buffer.data = 0;
            buffer.size = 0;
        }
    }

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