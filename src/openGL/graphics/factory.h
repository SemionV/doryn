#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"
#include "blocks.h"

namespace dory::openGL::graphics
{
    Buffer allocateBuffer(GLint bufferSize)
    {
        Buffer buffer;
        buffer.data = malloc(bufferSize);
        if(buffer.data != nullptr)
        {
            buffer.size = bufferSize;
        }

        return buffer;
    }

    void deallocateBuffer(Buffer& buffer) noexcept
    {
        if(buffer.data)
        {
            free(buffer.data);
            buffer.data = 0;
            buffer.size = 0;
        }
    }

    class BufferBlockFactory
    {
        public:
            template<std::size_t MembersCount>
            static constexpr void getBlockMemberNames(const UniformBlock<MembersCount>& block, const char*(&memberNames)[MembersCount]) noexcept
            {
                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    memberNames[i] = block.members[i]->key.c_str();
                }
            }

            template<std::size_t MembersCount>
            static UniformBlock<MembersCount> createUniformBlock(const std::string_view& blockName, const std::string_view(&memberNames)[MembersCount]) noexcept
            {
                UniformBlock<MembersCount> bufferBlock(blockName);

                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    bufferBlock.members[i]->key = memberNames[i];
                }

                return bufferBlock;
            }

            template<std::size_t MembersCount>
            static const Uniform& getUniformBlockMember(UniformBlock<MembersCount>& uniformBloc, const std::string_view& memberName)
            {
                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    auto member = uniformBloc.members[i];
                    if(strcmp(memberName.data(), member->key.c_str()) == 0)
                    {
                        return member;
                    }
                }
            }
    };

    class ColorsUniformBlockInterface
    {
        private:
            static constexpr std::string_view pointLiteral = ".";
            static constexpr std::string_view blockNameLiteral = "ColorsBlock";
            static constexpr std::string_view prefixLiteral = dory::compileTime::JoinStringLiterals<blockNameLiteral, pointLiteral>;

            static constexpr std::string_view brightColorLiteral = "brightColor";
            static constexpr std::string_view hippieColorLiteral = "hippieColor";
            static constexpr std::string_view darkColorLiteral = "darkColor";

            static constexpr auto brightColorUniformName = dory::compileTime::JoinStringLiterals<prefixLiteral, brightColorLiteral>;
            static constexpr auto hippieColorUniformName = dory::compileTime::JoinStringLiterals<prefixLiteral, hippieColorLiteral>;
            static constexpr auto darkColorUniformName = dory::compileTime::JoinStringLiterals<prefixLiteral, darkColorLiteral>;

        private:
            template<std::size_t MembersCount>
            static void setMemberColor(domain::Color& colorData, UniformBlock<MembersCount>& uniformBloc, Buffer& buffer, const std::string_view& memberName)
            {
                Uniform& member = BufferBlockFactory::getUniformBlockMember(uniformBloc, memberName);
                memcpy(buffer.data + member.offset, &colorData, member.size);
            }

        public:
            static decltype(auto) createColorsUniformBlock() noexcept
            {
                return BufferBlockFactory::createUniformBlock(blockNameLiteral, {brightColorUniformName, hippieColorUniformName, darkColorUniformName});
            }

            template<std::size_t MembersCount>
            static void setBrightColor(domain::Color& colorData, UniformBlock<MembersCount>& uniformBloc, Buffer& buffer)
            {
                setMemberColor(colorData, uniformBloc, brightColorUniformName);
            }

            template<std::size_t MembersCount>
            static void setHippieColor(domain::Color& colorData, UniformBlock<MembersCount>& uniformBloc, Buffer& buffer)
            {
                setMemberColor(colorData, uniformBloc, hippieColorUniformName);
            }

            template<std::size_t MembersCount>
            static void setDarkColor(domain::Color& colorData, UniformBlock<MembersCount>& uniformBloc, Buffer& buffer)
            {
                setMemberColor(colorData, uniformBloc, darkColorUniformName);
            }
    };
}