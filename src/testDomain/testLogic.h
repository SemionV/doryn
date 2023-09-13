#pragma once

namespace test
{
    class DORY_API TestLogic
    {
        private:
            std::shared_ptr<dory::SystemConsoleEventHub> consoleEventHub;
            std::shared_ptr<doryOpenGL::GlfwWindowEventHub> windowEventHub;

        public:
            TestLogic(std::shared_ptr<dory::SystemConsoleEventHub> consoleEventHub,
                std::shared_ptr<doryOpenGL::GlfwWindowEventHub> windowEventHub);
    };
}