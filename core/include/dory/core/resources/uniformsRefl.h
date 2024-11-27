#pragma once

#include <refl.hpp>
#include <dory/core/resources/bindings/uniforms.h>

REFL_TYPE(dory::core::resources::bindings::MaterialUniform)
        REFL_FIELD(color)
        REFL_FIELD(meshId)
REFL_END

REFL_TYPE(dory::core::resources::bindings::Uniforms)
        REFL_FIELD(modelTransform)
        REFL_FIELD(viewTransform)
        REFL_FIELD(projectionTransform)
        REFL_FIELD(material)
REFL_END