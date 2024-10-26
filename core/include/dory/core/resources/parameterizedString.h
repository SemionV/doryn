#pragma once

#include <dory/generic/parameterizedString.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::resources
{
    class ParameterizedString: public dory::generic::IParameterizedString
    {
    private:
        std::string value;
        fmt::runtime_format_string<> compiledValue;

    public:
        std::string& getTemplate() override
        {
            return value;
        }

        [[nodiscard]] const std::string& getTemplate() const override
        {
            return value;
        }

        void updateTemplate() override
        {
            compiledValue = fmt::runtime(value);
        }

        template<typename... Ts>
        std::string format(Ts&&... params)
        {
            return fmt::format(compiledValue, std::forward<Ts>(params)...);
        }

        template<typename... Ts>
        std::string get(Ts&&... params)
        {
            return format(std::forward<Ts>(params)...);
        }
    };
}
