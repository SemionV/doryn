#pragma once

#include <string>

namespace dory::generic
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
