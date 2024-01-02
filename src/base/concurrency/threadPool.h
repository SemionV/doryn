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
        explicit Task(PackagedTask&& packagedTask, Ts&&... arguments) noexcept:
                packagedTask(std::move(packagedTask)),
                arguments(std::forward<Ts>(arguments)...)
        {}

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
        std::condition_variable tasksUpdated;
        std::deque<TaskType> tasks;
        std::vector<std::future<void>> exitTokens;
        std::atomic<bool> running {true};

    private:
        void threadBody(std::promise<void> exitToken)
        {
            std::cout << std::this_thread::get_id() << ": threadBody start" << "\n";
            while(running)
            {
                auto lock = std::unique_lock(tasksMutex);
                tasksUpdated.wait(lock, [&]()
                {
                    return !tasks.empty() || !running;
                });

                if(!tasks.empty())
                {
                    auto task = std::move(tasks.front());
                    tasks.pop_front();
                    lock.unlock();

                    std::cout << std::this_thread::get_id() << ": start task" << "\n";
                    task();
                    std::cout << std::this_thread::get_id() << ": end task" << "\n";
                }
            }

            exitToken.set_value_at_thread_exit();
            std::cout << std::this_thread::get_id() << ": threadBody end" << "\n";
        }

    public:
        explicit Worker(std::size_t threadsCount)
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
            std::cout << std::this_thread::get_id() << ": start stop Worker" << "\n";
            running = false;
            tasksUpdated.notify_all();
            for(auto& exitFuture : exitTokens)
            {
                std::cout << std::this_thread::get_id() << ": start get thread end future" << "\n";
                exitFuture.get();
                std::cout << std::this_thread::get_id() << ": end get thread end future" << "\n";
            }
            std::cout << std::this_thread::get_id() << ": end stop Worker" << "\n";
        }

        template<typename F>
        std::future<T> addTask(F&& taskBody, Ts&&... arguments)
        {
            auto packagedTask = std::packaged_task<T(Ts...)>(std::forward<F>(taskBody));
            auto future = packagedTask.get_future();
            auto task = TaskType(std::move(packagedTask), std::forward<Ts>(arguments)...);
            {
                auto lock = std::lock_guard<std::mutex>(tasksMutex);
                tasks.emplace_back(std::move(task));
            }
            tasksUpdated.notify_all();

            return future;
        }
    };
}