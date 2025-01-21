#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/registry.h>
#include <dory/core/services/pipelineService.h>

using namespace dory;
using namespace core;
using namespace services;
using namespace resources;
using namespace configuration;

TEST(PipelineService, update)
{
    Registry registry{};
    Configuration configuration {};
    Localization localization {};
    DataContext context { configuration, localization};

    //setup registry
        //setup pipeline repo
        //setup logger

    auto pipelineService = PipelineService{ registry };

    pipelineService.update(context, std::chrono::nanoseconds { 1 });
}