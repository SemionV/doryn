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

    template<typename T, typename... Ts>
    class Worker
    {
    private:
        using TaskType = Task<T, Ts...>;

        std::mutex tasksMutex;
        std::condition_variable taskAdded;
        std::deque<TaskType> tasks;
        std::vector<std::future<void>> exitTokens;
        std::atomic<bool> running {true};

    private:
        void threadBody(std::promise<void> exitToken)
        {
            while(running)
            {
                auto lock = std::unique_lock(tasksMutex);
                taskAdded.wait(lock, [&]()
                {
                    return !tasks.empty();
                });

                auto& task = tasks.front();
                tasks.pop_front();
                lock.unlock();

                task();
            }

            exitToken.set_value_at_thread_exit();
        }

    public:
        explicit Worker(std::size_t threadsCount):
                exitTokens(threadsCount)
        {
            for(std::size_t i = 0; i < threadsCount; ++i)
            {
                auto promise = std::promise<void>{};
                exitTokens.emplace_back(promise.get_future());

                auto thread = std::thread(&Worker::threadBody, this, std::move(promise));
                thread.detach();
            }
        }

        ~Worker()
        {
            running = false;
            for(auto& exitToken : exitTokens)
            {
                exitToken.get();
            }
        }

        template<typename F>
        std::future<T> addTask(F&& taskBody, Ts&&... arguments)
        {
            auto packagedTask = std::packaged_task<T(Ts...)>(std::forward(taskBody));
            auto task = TaskType(std::move(packagedTask), std::forward<Ts>(arguments)...);
            auto future = task.getFuture();

            {
                auto lock = std::lock_guard<std::mutex>(tasksMutex);
                tasks.emplace_back(std::move(task));
            }
            taskAdded.notify_all();

            return future;
        }
    };
}