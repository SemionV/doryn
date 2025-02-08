#pragma once

#include <refl.hpp>
#include <dory/core/controllers/accelerationMovementController.h>
#include <dory/core/controllers/viewController.h>
#include <dory/core/triggers/timeFrameTrigger.h>
#include <dory/core/resources/scene/configuration.h>

REFL_TYPE(dory::core::resources::scene::configuration::Shader)
    REFL_FIELD(type)
    REFL_FIELD(filename)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::MaterialProperties)
    REFL_FIELD(color)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Material)
    REFL_FIELD(baseMaterials)
    REFL_FIELD(shaders)
    REFL_FIELD(properties)
    REFL_FIELD(polygonMode)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Mesh)
    REFL_FIELD(filename)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Assets)
    REFL_FIELD(shaders)
    REFL_FIELD(materials)
    REFL_FIELD(meshes)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::ObjectComponents)
    REFL_FIELD(linearMovement)
    REFL_FIELD(rotationMovement)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Transform)
    REFL_FIELD(position)
    REFL_FIELD(rotation)
    REFL_FIELD(scale)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Object)
    REFL_FIELD(transform)
    REFL_FIELD(mesh)
    REFL_FIELD(material)
    REFL_FIELD(components)
    REFL_FIELD(children)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Node)
    REFL_FIELD(parent)
    REFL_FIELD(trigger)
    REFL_FIELD(controller)
    REFL_FIELD(children)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Scene)
    REFL_FIELD(name)
    REFL_FIELD(assets)
    REFL_FIELD(objects)
    REFL_FIELD(cameras)
    REFL_FIELD(lights)
    REFL_FIELD(pipeline)
REFL_END;

REFL_TYPE(dory::core::controllers::ViewController)
REFL_END;

REFL_TYPE(dory::core::triggers::TimeFrameTrigger)
    REFL_FIELD(nanoseconds)
    REFL_FIELD(microseconds)
    REFL_FIELD(milliseconds)
    REFL_FIELD(seconds)
    REFL_FIELD(minutes)
REFL_END;