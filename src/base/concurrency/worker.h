#pragma once

#include "base/dependencies.h"

namespace dory::concurrency {
    template<typename T, typename... Ts>
    class Task {
    private:
        using PackagedTask = std::packaged_task<T(Ts...)>;

        PackagedTask packagedTask;
        std::tuple<Ts...> arguments;
    public:
        explicit Task(PackagedTask &&packagedTask, Ts &&... arguments) noexcept:
                packagedTask(std::move(packagedTask)),
                arguments(std::forward<Ts>(arguments)...) {}

        auto operator()() {
            return std::apply(packagedTask, arguments);
        }
    };

    struct WorkerDefaultProfilePolicy {
        template<typename TLog>
        inline static void print(TLog &log, const char *message) {
        }
    };

    struct WorkerLogProfilePolicy {
        template<typename TLog>
        inline static void print(TLog &log, const char *message) {
            log.printLine(std::this_thread::get_id(), ": ", message);
        }
    };

    template<typename TLog, typename TProfilePolicy, typename T, typename... Ts>
    class Worker {
    private:
        using TaskType = Task<T, Ts...>;

        std::mutex tasksMutex;
        std::condition_variable tasksUpdated;
        std::deque<TaskType> tasks;
        std::vector<std::future<void>> exitTokens;
        std::atomic<bool> running{true};
        TLog &log;

    private:
        void threadBody(std::promise<void> exitToken) {
            TProfilePolicy::print(log, "threadBody start");
            while (running) {
                auto lock = std::unique_lock(tasksMutex);
                tasksUpdated.wait(lock, [&]() {
                    return !tasks.empty() || !running;
                });

                if (!tasks.empty()) {
                    auto task = std::move(tasks.front());
                    tasks.pop_front();
                    lock.unlock();

                    TProfilePolicy::print(log, "start task");
                    task();
                    TProfilePolicy::print(log, "end task");
                }
            }

            exitToken.set_value_at_thread_exit();
            TProfilePolicy::print(log, "threadBody end");
        }

    public:
        Worker(const Worker &) = delete;

        Worker &operator=(const Worker &) = delete;

        explicit Worker(TLog &log, std::size_t threadsCount) :
                log(log) {
            for (std::size_t i = 0; i < threadsCount; ++i) {
                auto promise = std::promise<void>{};
                exitTokens.emplace_back(promise.get_future());

                auto thread = std::thread(&Worker::threadBody, this, std::move(promise));
                thread.detach();
            }
        }

        ~Worker() {
            TProfilePolicy::print(log, "start stop Worker");
            running = false;
            tasksUpdated.notify_all();
            for (auto &exitFuture: exitTokens) {
                TProfilePolicy::print(log, "start get thread end future");
                exitFuture.get();
                TProfilePolicy::print(log, "end get thread end future");
            }
            TProfilePolicy::print(log, "end stop Worker");
        }

        template<typename F>
        std::future<T> addTask(F &&taskBody, Ts &&... arguments) {
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