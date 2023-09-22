#pragma once

#include "dependencies.h"
#include "configuration/configuration.h"

namespace dory
{
    class Project
    {
        protected:
            virtual void configureProject() = 0;
            virtual void configurePipeline() = 0;

        public:
            void configure()
            {
                configureProject();
                configurePipeline();
            }
    };
}