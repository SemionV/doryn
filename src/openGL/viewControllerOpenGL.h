#pragma once

namespace doryOpenGL
{
    class ViewControllerOpenGL: public dory::ViewController
    {
        public:
            ViewControllerOpenGL(std::shared_ptr<dory::View> view);

            void initialize(dory::DataContext& context) override;
            void stop(dory::DataContext& context) override;
            void update(const dory::TimeSpan& timeStep, dory::DataContext& context) override;
    };
}