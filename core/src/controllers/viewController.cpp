#include <dory/core/registry.h>
#include <dory/core/controllers/viewController.h>

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
        /*auto viewService = _registry.get<services::IViewService>();
        if(viewService)
        {
            viewService->updateViews(context);
        }*/
    }
}