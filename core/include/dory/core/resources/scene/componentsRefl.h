#pragma once

#include <refl.hpp>
#include <dory/core/resources/scene/components.h>

REFL_TYPE(dory::core::resources::scene::components::AccelerationMovement)
    REFL_FIELD(endless)
    REFL_FIELD(value)
    REFL_FIELD(highVelocity)
    REFL_FIELD(lowVelocity)
    REFL_FIELD(decelerationDistance)
    REFL_FIELD(acceleration)
    REFL_FIELD(deceleration)
REFL_END;

REFL_TYPE(dory::core::resources::scene::components::AccelerationMovementState)
    REFL_FIELD(distanceDone)
    REFL_FIELD(currentVelocity)
    REFL_FIELD(step)
REFL_END;

REFL_TYPE(dory::core::resources::scene::components::RotationMovement)
    REFL_FIELD(setup)
    REFL_FIELD(state)
REFL_END;

REFL_TYPE(dory::core::resources::scene::components::LinearMovement)
    REFL_FIELD(setup)
    REFL_FIELD(state)
REFL_END;