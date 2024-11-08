#include <dory/core/registry.h>
#include <dory/core/services/openglGraphicalSystem.h>
#include <glad/gl.h>

namespace dory::core::services
{
    OpenglGraphicalSystem::OpenglGraphicalSystem(Registry& registry):
        _registry(registry)
    {}
}