#include <dory/core/registry.h>
#include <dory/core/controllers/consoleFlusher.h>

namespace dory::core::controllers
{
    ConsoleFlusher::ConsoleFlusher(Registry& registry):
        DependencyResolver(registry)
    {}

    bool ConsoleFlusher::initialize(resources::IdType nodeId, resources::DataContext& context)
    {
        return true;
    }

    void ConsoleFlusher::stop(resources::IdType nodeId, resources::DataContext& context)
    {
    }

    void ConsoleFlusher::update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep,resources::DataContext& context)
    {
        if(auto ioDevice = _registry.get<devices::IStandardIODevice>())
        {
            ioDevice->flush();
        }
    }
}
