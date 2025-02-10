#pragma once

#include <refl.hpp>
#include <dory/core/resources/factory.h>

REFL_TYPE(dory::core::resources::factory::Instance<dory::core::ITrigger>)
        REFL_FIELD(type)
REFL_END

REFL_TYPE(dory::core::resources::factory::Instance<dory::core::IController>)
        REFL_FIELD(type)
REFL_END

REFL_TYPE(dory::core::resources::factory::Instance<dory::core::services::ISceneDirector>)
        REFL_FIELD(type)
REFL_END

REFL_TYPE(dory::core::resources::factory::Instance<dory::core::devices::IDevice>)
        REFL_FIELD(type)
REFL_END