#pragma once

#include "dependencies.h"
#include "configuration/configuration.h"
#include "domain/engine.h"
#include "domain/services/frameService.h"
#include "domain/dataContext.h"

namespace dory
{
    template<class TDataContext>
    class Project
    {
        private:
            std::shared_ptr<TDataContext> context;

        protected:
            virtual void configureProject() = 0;
            virtual void attachEventHandlers() = 0;
            virtual void configurePipeline(TDataContext& context) = 0;

            virtual std::shared_ptr<domain::Engine> getEngine() = 0;
            virtual std::shared_ptr<domain::services::IFrameService> getFrameService() = 0;
            virtual std::shared_ptr<TDataContext> getDataContext() = 0;

        public:
            void configure()
            {
                context = getDataContext();

                configureProject();
                attachEventHandlers();
                configurePipeline(*context);
            }

            void run()
            {
                auto engine = getEngine();
                auto frameService = getFrameService();

                engine->initialize(*context);
                frameService->startLoop(engine, *context);
            }
    };
}