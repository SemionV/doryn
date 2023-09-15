#pragma once

namespace test
{
    class DORY_API TestLogic
    {
        private:
            std::shared_ptr<dory::SystemConsoleEventHub> consoleEventHub;
            std::shared_ptr<dory::WindowEventHub> windowEventHub;

        public:
            TestLogic(std::shared_ptr<dory::SystemConsoleEventHub> consoleEventHub,
                std::shared_ptr<dory::WindowEventHub> windowEventHub);
    };
}