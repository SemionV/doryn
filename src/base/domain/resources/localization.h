#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "stringResource.h"

namespace dory::domain::resources
{
    struct Localization
    {
        StringResource hello;
        StringResource goodBye;
    };
}
REFL_TYPE(dory::domain::resources::Localization)
    REFL_FIELD(hello)
    REFL_FIELD(goodBye)
REFL_END