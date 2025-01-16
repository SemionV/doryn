#pragma once

#include <dory/core/services/iProfilingService.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services
{
    class ProfilingService: public IProfilingService, public DependencyResolver
    {
    public:
        explicit ProfilingService(Registry& registry);

        void analyze(const resources::profiling::Capture& capture) final;
    };
}
