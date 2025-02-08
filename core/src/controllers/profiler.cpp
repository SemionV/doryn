#include <dory/core/registry.h>
#include <dory/core/controllers/profiler.h>

namespace dory::core::controllers
{
    Profiler::Profiler(Registry& registry):
        DependencyResolver(registry)
    {}

    bool Profiler::initialize(resources::IdType nodeId, resources::DataContext& context)
    {
        return true;
    }

    void Profiler::stop(resources::IdType nodeId, resources::DataContext& context)
    {
    }

    void Profiler::update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        auto* capture = getCurrentCapture(context.profiling);

        if(!capture)
        {
            startNewCapture(context.profiling, context.profiling.captureIdCounter++, 100);
        }
        else
        {
            if(capture->done)
            {
                if(auto profilingService = _registry.get<services::IProfilingService>())
                {
                    profilingService->analyze(*capture);
                }
                removeCurrentCapture(context.profiling);

                startNewCapture(context.profiling, context.profiling.captureIdCounter++, 100);

                frameCount = {};
            }
            else
            {
                addNewFrame(*capture, frameCount);
            }
        }

        frameCount++;
    }
}
