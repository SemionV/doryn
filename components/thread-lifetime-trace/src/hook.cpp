#if defined(DORY_PLATFORM_LINUX) || defined(DORY_PLATFORM_APPLE)

#include <pthread.h>
#include <dlfcn.h>
#include <atomic>
#include <unistd.h>
#include <dory/profiling/profiler.h>
#include <spdlog/fmt/fmt.h>

static inline long dory_os_tid()
{
#if defined(DORY_PLATFORM_LINUX)
#if defined(SYS_gettid)
    return (long)syscall(SYS_gettid);
#else
    return (long)gettid(); // if available
#endif
#elif defined(DORY_PLATFORM_APPLE)
    // Apple extension: stable thread id for current thread
    std::uint64_t tid = 0;
    pthread_threadid_np(nullptr, &tid);
    return (long)tid; // may truncate only if long is 32-bit; on macOS arm64 long is 64-bit
#endif
}

extern "C"
{
    using real_pthread_create_t = int(*)(pthread_t*, const pthread_attr_t*, void*(*)(void*), void*);

    static real_pthread_create_t real_pthread_create = nullptr;
    static std::atomic<bool> inited { false };

    struct Shim
    {
        void* (*fn)(void*);
        void*  arg;
    };

    static void init_real()
    {
        bool expected = false;
        if (inited.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            void* sym = dlsym(RTLD_NEXT, "pthread_create");
            real_pthread_create = reinterpret_cast<real_pthread_create_t>(sym);
        }
    }

    __attribute__((constructor))
    static void hello()
    {
        const char msg[] = "pthread_create_hook loaded\n";
        write(2, msg, sizeof msg - 1);
    }

    static void cleanup_on_exit(void* /*unused*/)
    {
        auto tid = dory_os_tid();
        if (DORY_TRACE_IS_PROFILER_READY)
        {
            const auto message = fmt::format("Thread Exit: {}", tid);
            DORY_TRACE_MESSAGE_STACK(message.c_str(), message.size(), 0);
        }

        char buf[64];
        const int n = snprintf(buf, sizeof(buf), "thread exit tid=%d\n", tid);
        if (n > 0)
        {
            write(2, buf, n);
        }
    }

    static void* trampoline(void* p)
    {
        auto [fn, arg] = *static_cast<Shim*>(p);
        free(p);

        auto tid = dory_os_tid();

        char buf[64];
        const int n = snprintf(buf, sizeof(buf), "thread start tid=%d\n", tid);
        if (n > 0)
        {
            write(2, buf, n);
        }

        if (DORY_TRACE_IS_PROFILER_READY)
        {
            const auto message = fmt::format("Thread Created: {}", tid);
            DORY_TRACE_MESSAGE_STACK(message.c_str(), message.size(), 0);
        }

        void* result = nullptr;

        // Register cleanup handler that fires on any exit path
        pthread_cleanup_push(cleanup_on_exit, nullptr);

        // Call the user's function
        result = fn(arg);

        // On normal return, execute cleanup too
        pthread_cleanup_pop(/*execute=*/1);

        return result;
    }

    __attribute__((visibility("default")))
    int pthread_create(pthread_t* t, const pthread_attr_t* a, void*(*start)(void*), void* arg)
    {
        init_real();

        const auto shim = (Shim*)malloc(sizeof(Shim));
        shim->fn  = start;
        shim->arg = arg;

        if (DORY_TRACE_IS_PROFILER_READY)
        {
            const auto message = fmt::format("Create Thread");
            DORY_TRACE_MESSAGE_STACK(message.c_str(), message.size(), 32);
        }

        return real_pthread_create(t, a, &trampoline, shim);
    }

} // extern "C"

#elif defined(DORY_PLATFORM_WIN32)

//Need to export at least one symbol to trigger .lib file generation on windows
extern "C" __declspec(dllexport) void dory_thread_lifetime_trace_touch() {}

#endif