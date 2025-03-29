#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <coroutine>

struct ContinuationAwaitable
{
    bool await_ready() noexcept
    {
        return false;
    }

    auto await_suspend(auto handle) noexcept
    {
        return handle.promise().continuation.value();
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
        /*if(continuation)
        {
            return ContinuationAwaitable{};
        }*/

        return std::suspend_always{};
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
    TResult result;

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

template<typename T>
class Task
{
private:
    using PromiseType = AsyncPromise<T, Task>;

    std::coroutine_handle<AsyncPromise<T, Task>> _handle;

    explicit Task(PromiseType& promise) noexcept: _handle(std::coroutine_handle<PromiseType>::from_promise(promise))
    {}

public:
    using promise_type = PromiseType;

    static Task create(PromiseType& promise)
    {
        return Task{ promise };
    }

    Task(Task&& other) noexcept: _handle(std::exchange(other._handle, {}))
    {}

    ~Task()
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

    //TODO: handle the T=void case
    T await_resume() noexcept
    {
        auto& promise = _handle.promise();
        if(promise.result)
        {
            return std::move(*promise.result);
        }

        if(promise.exception)
        {
            std::rethrow_exception(std::move(*promise.exception));
        }

        return T{};
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

Task<int> action(const int base)
{
    co_return base + 1;
}

TEST(CoroutineTests, AsyncTask)
{
    std::cout << "Step 1" << std::endl;
    auto task = action(2);
    task.resume();
    std::cout << "Step 2" << std::endl;
}

/*template<typename T>
struct TaskPromise<Task<void>>: public Promise<Task<void>>
{
    void unhandled_exception()
    {
        std::terminate();
    }

    void return_void()
    {}
};

template<typename TValue>
class TaskPromise<Task<TValue>>: public Promise<TValue>
{
private:
    std::variant<std::monostate, TValue, std::exception_ptr> _result;
    std::coroutine_handle<> _continuation;

public:
    TValue return_value()
    {
        return value;
    }

    void unhandled_exception()
    {
        std::terminate();
    }
};


template<typename TResult = void>
class Task
{
private:
    using PromiseType = TaskPromise<Task>;

    std::coroutine_handle<PromiseType> _handle;

    explicit Task(std::coroutine_handle<PromiseType> handle): _handle(handle)
    {}

public:
    using promise_type = PromiseType;

    static Task create(PromiseType& promise)
    {
        using Handle = std::coroutine_handle<PromiseType>;
        return Task{ Handle::from_promise(promise) };
    }

    Task(Task&& other) noexcept: _handle(std::exchange(other._handle, {}))
    {}

    ~Task()
    {
        if(_handle)
        {
            _handle.destroy();
        }
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

Task<> action()
{
    std::cout << "Step 2" << std::endl;
    co_await std::suspend_always{};

    std::cout << "Step 4" << std::endl;
}

TEST(CoroutineTests, SimpleTask)
{
    std::cout << "Step 1" << std::endl;
    auto task = action();
    task.resume();

    std::cout << "Step 3" << std::endl;
    task.resume();

    std::cout << "Step 5" << std::endl;
}*/