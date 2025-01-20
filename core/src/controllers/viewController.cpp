#include <dory/core/registry.h>
#include <dory/core/controllers/viewController.h>
#include "dory/core/resources/profiling.h"

namespace dory::core::controllers
{
    ViewController::ViewController(Registry &registry):
        _registry(registry)
    {}

    using namespace resources;
    using namespace services;
    using namespace std::chrono;

    bool ViewController::initialize(resources::IdType referenceId, resources::DataContext& context)
    {
        return true;
    }

    void ViewController::stop(resources::IdType referenceId, resources::DataContext& context)
    {

    }

    void ViewController::update(resources::IdType referenceId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        if(auto viewService = _registry.get<IViewService>())
        {
            profiling::pushTimeSlice(context.profiling, "rendering");

            float deltaTime = context.viewStatesUpdateTimeDelta.count() > 0 ? context.viewStatesUpdateTimeDelta.count() : 1.f;
            float alpha = context.viewStatesUpdateTime.count() / deltaTime;
            alpha = glm::clamp(alpha, 0.0f, 1.0f);
            context.viewStatesUpdateTime += timeStep;

            if(auto* frame = profiling::getCurrentFrame(context.profiling))
            {
                frame->viewStates.push_back(context.viewStates);
                frame->alpha = alpha;
            }

            viewService->updateViews(context.viewStates, alpha, context.profiling);

            profiling::popTimeSlice(context.profiling);
        }
    }
}