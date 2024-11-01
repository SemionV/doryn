#pragma once

#include <dory/core/iController.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::controllers
{
    class ViewController: public IController
    {
    private:
        Registry& _registry;

    public:
        explicit ViewController(Registry& registry);

        bool initialize(resources::IdType referenceId, resources::DataContext& context) override;
        void stop(resources::IdType referenceId, resources::DataContext& context) override;
        void update(resources::IdType referenceId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) override;
    };
}