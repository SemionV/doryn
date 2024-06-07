#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::domain::resources
{
    struct String
    {
        std::string value;
    };

    struct FmtTemplate
    {
        std::string value;
        fmt::runtime_format_string<> compiledValue;
    };

    struct GoodByeTemplate
    {
        FmtTemplate fmt;
    };
}
REFL_TYPE(dory::domain::resources::String)
    REFL_FIELD(value)
REFL_END

REFL_TYPE(dory::domain::resources::FmtTemplate)
        REFL_FIELD(value)
REFL_END

REFL_TYPE(dory::domain::resources::GoodByeTemplate)
        REFL_FIELD(fmt)
REFL_END

namespace dory::domain::resources
{
    struct Resource
    {
        static std::string get(String& resource)
        {
            return resource.value;
        }

        static void compile(FmtTemplate& resource)
        {
            resource.compiledValue = fmt::runtime(resource.value);
        }

        static std::string get(GoodByeTemplate& resource, const std::string& name)
        {
            return fmt::format(resource.fmt.compiledValue, name);
        }
    };
}