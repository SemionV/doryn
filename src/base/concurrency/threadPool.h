#pragma once

#include "base/dependencies.h"

namespace dory::concurrency
{
    template<typename T, typename... Ts>
    class Task
    {
    private:
        using PackagedTask = std::packaged_task<T(Ts...)>;

        PackagedTask packagedTask;
        std::tuple<Ts...> arguments;
    public:
        template<typename F>
        explicit Task(PackagedTask&& packagedTask, Ts&&... arguments):
                packagedTask(std::forward<F>(packagedTask)),
                arguments(std::forward<Ts>(arguments)...)
        {}

        auto getFuture()
        {
            return packagedTask.get_future();
        }

        auto operator()()
        {
            return std::apply(packagedTask, arguments);
        }
    };

    template<std::size_t NThreads, typename T, typename... Ts>
    class Worker
    {
    private:
        using PackagedTask = std::packaged_task<T(Ts...)>;
        using TaskType = Task<T, Ts...>;

        std::mutex mutex;
        std::deque<TaskType> tasks;

    private:
        void threadBody()
        {

        }

    public:
        template<typename F>
        std::future<T> addTask(F&& taskBody, Ts&&... arguments)
        {
            auto packagedTask = PackagedTask(std::forward(taskBody));
            auto task = TaskType(std::move(packagedTask), std::forward<Ts>(arguments)...);
            auto future = task.getFuture();

            {
                auto lock = std::lock_guard<std::mutex>(mutex);
                tasks.emplace_front(std::move(task));
            }

            return future;
        }
    };
}