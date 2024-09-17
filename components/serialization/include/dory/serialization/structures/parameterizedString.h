#pragma once

#include <dory/generics/typeTraits.h>

namespace dory::serialization
{
    class IParameterizedString
    {
    public:
        virtual std::string& getTemplate() = 0;
        [[nodiscard]] virtual const std::string& getTemplate() const = 0;
        virtual void updateTemplate() = 0;
    };
}