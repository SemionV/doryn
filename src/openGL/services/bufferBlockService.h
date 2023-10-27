#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"
#include "openGL/graphics/blocks.h"

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
            template<std::size_t MembersCount>
            static constexpr void getBlockMemberNames(const graphics::UniformBlock<MembersCount>& block, const char*(&memberNames)[MembersCount]) noexcept
            {
                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    memberNames[i] = block.members[i].key.c_str();
                }
            }

            template<std::size_t MembersCount>
            static graphics::UniformBlock<MembersCount> createUniformBlock(const std::string_view& blockName, const std::string_view(&memberNames)[MembersCount]) noexcept
            {
                graphics::UniformBlock<MembersCount> bufferBlock(blockName);

                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    bufferBlock.members[i].key = memberNames[i];
                }

                return bufferBlock;
            }

            template<std::size_t MembersCount>
            static const graphics::Uniform& getUniformBlockMember(graphics::UniformBlock<MembersCount>& uniformBloc, const std::string_view& memberName)
            {
                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    auto member = uniformBloc.members[i];
                    if(strcmp(memberName.data(), member.key.c_str()) == 0)
                    {
                        return member;
                    }
                }
            }
    };
}