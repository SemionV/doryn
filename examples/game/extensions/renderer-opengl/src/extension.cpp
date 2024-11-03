#include <extension.h>
#include <iostream>
#include <openglRenderer.h>

namespace dory::renderer::opengl
{
    void Extension::attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext)
    {
        std::cout << "dory::renderer::opengl::Extension: attach extension\n";

        _registry.set<core::services::IRenderer, core::resources::GraphicalSystem::opengl>(library, _renderer.get());
    }

    Extension::Extension(core::Registry& registry):
            _registry(registry),
            _renderer(registry, std::make_shared<OpenglRenderer>(registry), core::resources::GraphicalSystem::opengl)
    {}

    Extension::~Extension()
    {
        std::cout << "dory::renderer::opengl::Extension: detach extension\n";
    }
}