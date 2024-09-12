#pragma once

#include <dory/generics/typeTraits.h>
#include <spdlog/fmt/fmt.h>

namespace dory::domain::resources
{
    class ParameterizedString
    {
        private:
        std::string value;
        fmt::runtime_format_string<> compiledValue;
        public:
        std::string& getTemplate()
        {
            return value;
        }

        [[nodiscard]] decltype(auto) getTemplate() const
        {
            return value;
        }

        void updateTemplate()
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