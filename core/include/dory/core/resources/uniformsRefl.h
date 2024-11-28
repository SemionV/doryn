#pragma once

#include <refl.hpp>
#include <dory/core/resources/bindings/uniforms.h>

REFL_TYPE(dory::core::resources::bindings::uniforms::Material)
        REFL_FIELD(color)
        REFL_FIELD(meshId)
REFL_END

REFL_TYPE(dory::core::resources::bindings::uniforms::Uniforms)
        REFL_FIELD(color)
        REFL_FIELD(modelTransform)
        REFL_FIELD(viewTransform)
        REFL_FIELD(projectionTransform)
        REFL_FIELD(material)
REFL_END