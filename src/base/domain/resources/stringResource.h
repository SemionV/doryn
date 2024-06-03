#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::domain::resources
{
    struct StringResource
    {
    private:
        fmt::runtime_format_string<> fmtRuntimeString;

    public:
        std::optional<std::string> fmt;
    };
}
REFL_TYPE(dory::domain::resources::StringResource)
        REFL_FIELD(fmt)
REFL_END