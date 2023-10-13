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
        std::array<Uniform*, MembersCount> members;

        UniformBlock(const std::string_view& blockName):
            DataBinding(blockName)
        {}

        template<typename ...T>
        UniformBlock(const std::string_view& blockName, T... members):
            DataBinding(blockName),
            members{members...}
        {}
    };

    struct ColorsUniformBlock: public UniformBlock<3>
    {
        static constexpr std::string_view pointLiteral = ".";
        static constexpr std::string_view blockNameLiteral = "ColorsBlock";
        static constexpr std::string_view prefixLiteral = dory::compileTime::JoinStringLiterals<blockNameLiteral, pointLiteral>;

        static constexpr std::string_view brightColorLiteral = "brightColor";
        static constexpr std::string_view hippieColorLiteral = "hippieColor";
        static constexpr std::string_view darkColorLiteral = "darkColor";

        Uniform brightColor = Uniform(dory::compileTime::JoinStringLiterals<prefixLiteral, brightColorLiteral>);
        Uniform hippieColor = Uniform(dory::compileTime::JoinStringLiterals<prefixLiteral, hippieColorLiteral>);
        Uniform darkColor = Uniform(dory::compileTime::JoinStringLiterals<prefixLiteral, darkColorLiteral>);

        ColorsUniformBlock():
            UniformBlock(blockNameLiteral, &brightColor, &hippieColor, &darkColor)
        {}
    };

    struct ColorsBufferInterface
    {
       domain::Color brightColor;
       domain::Color hippieColor;
       domain::Color darkColor;
    };

    class BufferInterfaceFactory
    {
        public:
            template<typename TInterface>
            static TInterface& getBufferInterface(const Buffer& hostBuffer, const std::size_t offset = 0)
            {
                TInterface& bufferInterface = *(static_cast<TInterface*>(hostBuffer.data) + offset);
                return bufferInterface;
            }
    };
}