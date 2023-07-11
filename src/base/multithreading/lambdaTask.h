#pragma once

#include "task.h"

namespace dory
{
    class DORY_API LambdaTask: public Task
    {
        private:
            std::function<void()> lambda;

        public:
            LambdaTask(std::function<void()> lambda);

            void operator()() override;
    };
}