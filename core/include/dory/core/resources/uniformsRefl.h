#pragma once

#include <refl.hpp>
#include <dory/core/resources/bindings/uniforms.h>
#include <dory/core/resources/objects/materialProperties.h>

REFL_TYPE(dory::core::resources::objects::MaterialProperties)
        REFL_FIELD(color)
        REFL_FIELD(color2)
        REFL_FIELD(color3)
REFL_END

REFL_TYPE(dory::core::resources::bindings::uniforms::Uniforms)
        REFL_FIELD(modelTransform)
        REFL_FIELD(viewTransform)
        REFL_FIELD(projectionTransform)
        REFL_FIELD(material)
REFL_END