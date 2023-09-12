#include "base/dependencies.h"
#include "viewController.h"

namespace dory
{
    ViewController::ViewController(std::shared_ptr<IConfiguration> configuration, std::shared_ptr<View> view):
        configuration(configuration),
        view(view)
    {        
    }

    void ViewController::initialize(dory::DataContext& context)
    {
        std::cout << "initialize: Basic Scene Renderer" << std::endl;
    }

    void ViewController::stop(dory::DataContext& context)
    {        
    }

    void ViewController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {

    }
}