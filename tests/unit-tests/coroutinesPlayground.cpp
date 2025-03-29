#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <coroutine>
#include <future>

template<typename TPromise>
struct ContinuationAwaitable
{
    bool await_ready() noexcept
    {
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<TPromise> handle) noexcept
    {
        auto& continuation = handle.promise().continuation;
        if(continuation)
        {
            return *continuation;
        }

        return std::noop_coroutine();
    }

    void await_resume() noexcept {}
};

template<typename TTask, typename TImplementation>
class Promise
{
public:
    std::optional<std::coroutine_handle<>> continuation;
    std::optional<std::exception_ptr> exception;

    TTask get_return_object()
    {
        return TTask::create((TImplementation&)*this);
    }

    auto initial_suspend()
    {
        return std::suspend_always{};
    }

    auto final_suspend() noexcept
    {
        return ContinuationAwaitable<TImplementation>{};
    }

    void unhandled_exception() noexcept
    {
        exception = std::current_exception();
    }
};

template<typename TResult, template<class> class TTask>
class AsyncPromise: public Promise<TTask<TResult>, AsyncPromise<TResult, TTask>>
{
public:
    TResult result{};

    void return_value(TResult value)
    {
        result = std::move(value);
    }
};

template<template<class> class TTask>
class AsyncPromise<void, TTask>: public Promise<TTask<void>, AsyncPromise<void, TTask>>
{
public:
    void return_void()
    {}
};

template<typename T, template<class> class TImplementation>
class AwaitableTask
{
protected:
    using PromiseType = AsyncPromise<T, TImplementation>;

    std::coroutine_handle<AsyncPromise<T, TImplementation>> _handle;

    explicit AwaitableTask(PromiseType& promise) noexcept: _handle(std::coroutine_handle<PromiseType>::from_promise(promise))
    {}

public:
    using promise_type = PromiseType;

    AwaitableTask(AwaitableTask&& other) noexcept: _handle(std::exchange(other._handle, {}))
    {}

    ~AwaitableTask()
    {
        if(_handle)
        {
            _handle.destroy();
        }
    }

    bool await_ready()
    {
        return false;
    }

    auto await_suspend(std::coroutine_handle<> continuationHandle) noexcept
    {
        _handle.promise().continuation = continuationHandle;
        return _handle;
    }

    bool resume()
    {
        if(!_handle.done())
        {
            _handle.resume();
        }

        return !_handle.done();
    }
};

template<typename T = void>
class Task: public AwaitableTask<T, Task>
{
private:
    using PromiseType = AsyncPromise<T, Task>;

    explicit Task(PromiseType& promise) noexcept: AwaitableTask<T, Task>(promise)
    {}

public:
    static Task create(PromiseType& promise)
    {
        return Task{ promise };
    }

    Task(Task&& other) noexcept: AwaitableTask<T, Task>(std::move(other))
    {}

    T await_resume() noexcept
    {
        auto& promise = this->_handle.promise();
        if(promise.exception)
        {
            std::rethrow_exception(std::move(*promise.exception));
        }

        return std::move(*promise.result);
    }

    T result()
    {
        auto& promise = this->_handle.promise();
        return std::move(promise.result);
    }
};

template<>
class Task<void>: public AwaitableTask<void, Task>
{
private:
    explicit Task(PromiseType& promise) noexcept: AwaitableTask(promise)
    {}

public:
    static Task create(PromiseType& promise)
    {
        return Task{ promise };
    }

    Task(Task&& other) noexcept: AwaitableTask(std::move(other))
    {}

    void await_resume() noexcept
    {
        auto& promise = this->_handle.promise();
        if(promise.exception)
        {
            std::rethrow_exception(std::move(*promise.exception));
        }
    }
};

Task<int> action(const int base)
{
    co_return base + 1;
}

Task<> job()
{
    std::cout << "Job Step 1" << std::endl;
    co_await std::suspend_always{};
    std::cout << "Job Step 2" << std::endl;
}

TEST(CoroutineTests, AsyncTask)
{
    std::cout << "Step 1" << std::endl;
    auto task = action(3);
    std::cout << "Result: " << task.result() << std::endl;
    while(task.resume()) {}
    std::cout << "Step 2" << std::endl;
    std::cout << "Result: " << task.result() << std::endl;

    auto task2 = job();
    while(task2.resume()) {}
    std::cout << "Step 3" << std::endl;
}