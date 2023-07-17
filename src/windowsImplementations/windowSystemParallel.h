#pragma once

#include "windowSystem.h"

namespace doryWindows
{
    class DORY_API WindowSystemParallel: public WindowSystem
    {
        private:
            dory::IndividualProcessThread processThread;

        public:
            virtual bool connect() override;
            virtual void disconnect() override;
            virtual void update() override;
            virtual std::shared_ptr<dory::Window> createWindow(const WindowParameters& parameters) override;
    };
}