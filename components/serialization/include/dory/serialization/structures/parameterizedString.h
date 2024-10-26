#pragma once

#include <dory/generic/typeTraits.h>

namespace dory::serialization
{
    class IParameterizedString
    {
    public:
        virtual ~IParameterizedString() = default;

        virtual std::string& getTemplate() = 0;
        [[nodiscard]] virtual const std::string& getTemplate() const = 0;
        virtual void updateTemplate() = 0;
    };
}