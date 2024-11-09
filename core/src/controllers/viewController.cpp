#include <dory/core/registry.h>
#include <dory/core/controllers/viewController.h>
#include <dory/core/repositories/windowRepository.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::controllers
{
    ViewController::ViewController(Registry &registry):
        _registry(registry)
    {}

    bool ViewController::initialize(resources::IdType referenceId, resources::DataContext& context)
    {
        return true;
    }

    void ViewController::stop(resources::IdType referenceId, resources::DataContext& context)
    {

    }

    void ViewController::update(resources::IdType referenceId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        _registry.getAll<repositories::IWindowRepository, resources::WindowSystem>([this, &context](auto& windowRepositories) {
            for(const auto& [windowSystem, resourceHandle] : windowRepositories)
            {
                auto windowRepository = resourceHandle.lock();
                if(windowRepository)
                {
                    windowRepository->each([this, &context](auto& window) {
                        auto gpuClient = _registry.get<services::IGraphicalSystem>();
                        if(gpuClient)
                        {
                            gpuClient->render(context, window);
                        }
                    });
                }
            }
        });
    }
}