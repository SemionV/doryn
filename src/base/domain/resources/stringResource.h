#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::domain::resources
{
    class ParameterizedString
    {
        private:
        std::string value;
#ifdef USE_FMT
        fmt::runtime_format_string<> compiledValue;
#else
        std::string compiledValue;
#endif
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
#ifdef USE_FMT
            compiledValue = fmt::runtime(value);
#else
            compiledValue = value;
#endif
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