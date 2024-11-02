#include <dory/core/registry.h>
#include <dory/core/controllers/viewController.h>
#include <dory/core/repositories/iRepository.h>

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
        _registry.get<
                generic::registry::Service<repositories::IRepository<resources::entity::Window>>,
                generic::registry::Service<repositories::IViewRepository>>(
        [this, &context](repositories::IRepository<resources::entity::Window>* windowRepository,
           repositories::IViewRepository* viewRepository)
        {
            auto views = viewRepository->getAll();
            auto windows = windowRepository->getAll();
            for(const auto& window : windows)
            {
                for(const auto& view : views)
                {
                    if(view.windowId == window.id)
                    {
                        auto renderer = _registry.get<services::IRenderer>((std::decay_t<decltype(window.graphicalSystem)>)window.graphicalSystem);
                        if(renderer)
                        {
                            renderer->draw(context, window, view);
                        }
                    }
                }
            }
        });
    }
}