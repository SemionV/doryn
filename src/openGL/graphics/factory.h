#pragma once

#include "openGL/dependencies.h"
#include "base/base.h"
#include "blocks.h"

namespace dory::openGL::graphics
{
    class BufferBlockFactory
    {
        public:
            template<std::size_t MembersCount>
            static constexpr void getBlockMemberNames(const UniformBlock<MembersCount>& block, const char*(&memberNames)[MembersCount]) noexcept
            {
                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    memberNames[i] = block.members[i].key.c_str();
                }
            }

            template<std::size_t MembersCount>
            static UniformBlock<MembersCount> createUniformBlock(const std::string_view& blockName, const std::string_view(&memberNames)[MembersCount]) noexcept
            {
                UniformBlock<MembersCount> bufferBlock(blockName);

                for(std::size_t i = 0; i < MembersCount; ++i)
                {
                    bufferBlock.members[i].key = memberNames[i];
                }

                return bufferBlock;
            }
    };

    class ColorsUniformBlockFactory
    {
        private:
            static constexpr std::string_view pointLiteral = ".";
            static constexpr std::string_view blockNameLiteral = "ColorsBlock";
            static constexpr std::string_view prefixLiteral = dory::domain::JoinStringLiterals<blockNameLiteral, pointLiteral>;

            static constexpr std::string_view brightColorLiteral = "brightColor";
            static constexpr std::string_view hippieColorLiteral = "hippieColor";
            static constexpr std::string_view darkColorLiteral = "darkColor";

            static constexpr auto brightColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, brightColorLiteral>;
            static constexpr auto hippieColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, hippieColorLiteral>;
            static constexpr auto darkColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, darkColorLiteral>;

        public:
            static decltype(auto) createColorsUniformBlock() noexcept
            {
                return BufferBlockFactory::createUniformBlock(blockNameLiteral, {brightColorUniformName, hippieColorUniformName, darkColorUniformName});
            }

            template<std::size_t MembersCount>
            static void setBrightColor(float color[4], UniformBlock<MembersCount>& uniformBloc, Buffer& buffer)
            {
                
            }
    };
}