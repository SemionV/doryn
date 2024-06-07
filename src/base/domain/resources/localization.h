#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "stringResource.h"

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
        std::string get(Ts... params)
        {
            return fmt::format(compiledValue, params...);
        }
    };

    struct Localization
    {
        String hello;
        GoodByeTemplate goodBye;
        ParameterizedString birthDate;
    };
}
REFL_TYPE(dory::domain::resources::Localization)
    REFL_FIELD(hello)
    REFL_FIELD(goodBye)
    REFL_FIELD(birthDate)
REFL_END