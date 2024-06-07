#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "stringResource.h"

namespace dory::domain::resources
{
    using FmtTemplate = fmt::runtime_format_string<>;

    struct Localization
    {
        String hello;
        GoodByeTemplate goodBye;
        FmtTemplate direct;
    };
}
REFL_TYPE(dory::domain::resources::Localization)
    REFL_FIELD(hello)
    REFL_FIELD(goodBye)
    REFL_FIELD(direct)
REFL_END