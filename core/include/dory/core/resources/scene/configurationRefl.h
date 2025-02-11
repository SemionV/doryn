#pragma once

#include <refl.hpp>
#include <dory/core/triggers/timeFrameTrigger.h>
#include <dory/core/controllers/viewController.h>
#include <dory/core/controllers/frameCounter.h>
#include <dory/core/controllers/profiler.h>
#include <dory/core/controllers/eventDispatcher.h>
#include <dory/core/controllers/stateUpdater.h>
#include <dory/core/controllers/accelerationMovementController.h>
#include <dory/core/controllers/movementController.h>
#include <dory/core/controllers/transformController.h>
#include <dory/core/controllers/consoleFlusher.h>
#include <dory/core/controllers/windowSystemController.h>
#include <dory/core/services/scene/directors/assetLoader.h>
#include <dory/core/services/scene/directors/applicationDirector.h>
#include <dory/core/services/scene/directors/viewDirector.h>
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
    REFL_FIELD(controllerInstance)
    REFL_FIELD(controller)
    REFL_FIELD(children)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Pipeline)
    REFL_FIELD(nodes)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::GraphicalContext)
    REFL_FIELD(graphicalSystem)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::Window)
    REFL_FIELD(width)
    REFL_FIELD(height)
    REFL_FIELD(title)
    REFL_FIELD(graphicalContext)
REFL_END;

REFL_TYPE(dory::core::resources::scene::configuration::SceneConfiguration)
    REFL_FIELD(name)
    REFL_FIELD(assets)
    REFL_FIELD(objects)
    REFL_FIELD(cameras)
    REFL_FIELD(lights)
    REFL_FIELD(pipeline)
    REFL_FIELD(devices)
    REFL_FIELD(directors)
REFL_END;

REFL_TYPE(dory::core::resources::Duration)
    REFL_FIELD(nanoseconds)
    REFL_FIELD(microseconds)
    REFL_FIELD(milliseconds)
    REFL_FIELD(seconds)
    REFL_FIELD(minutes)
REFL_END;

REFL_TYPE(dory::core::triggers::TimeFrameTrigger)
    REFL_FIELD(duration)
REFL_END;

REFL_TYPE(dory::core::controllers::ViewController)
REFL_END;

REFL_TYPE(dory::core::controllers::FrameCounter)
    REFL_FIELD(interval)
REFL_END;

REFL_TYPE(dory::core::controllers::Profiler)
REFL_END;

REFL_TYPE(dory::core::controllers::EventDispatcher)
REFL_END;

REFL_TYPE(dory::core::controllers::StateUpdater)
REFL_END;

REFL_TYPE(dory::core::controllers::AccelerationMovementController)
REFL_END;

REFL_TYPE(dory::core::controllers::MovementController)
REFL_END;

REFL_TYPE(dory::core::controllers::TransformController)
REFL_END;

REFL_TYPE(dory::core::controllers::ConsoleFlusher)
REFL_END;

REFL_TYPE(dory::core::controllers::WindowSystemController)
REFL_END;

REFL_TYPE(dory::core::services::scene::directors::AssetLoader)
REFL_END;

REFL_TYPE(dory::core::services::scene::directors::ApplicationDirector)
REFL_END;

REFL_TYPE(dory::core::services::scene::directors::ViewDirector)
REFL_END;