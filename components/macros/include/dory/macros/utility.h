#pragma once

#include <sstream>
#include <string>
#include <thread>

#define DORY_CONCAT_IMPL(x, y) x##y
#define DORY_CONCAT(x, y) DORY_CONCAT_IMPL(x, y)

#define DORY_NOOP do {} while(0)

inline std::string threadIdToString(const std::thread::id& id)
{
    std::stringstream ss;
    ss << id;
    return ss.str();
}

#if defined(DORY_PLATFORM_LINUX)
#include <sched.h>
#include <unistd.h>

inline pid_t linux_tid() {
#if defined(SYS_gettid)
    return (pid_t)syscall(SYS_gettid);   // works everywhere
#else
    return gettid();                     // glibc ≥ 2.30
#endif
}

#define DORY_CURRENT_THREAD_ID() linux_tid();

#elif defined(DORY_PLATFORM_WIN32)

#define DORY_CURRENT_THREAD_ID() threadIdToString(std::this_thread::get_id()); //Standard C++ thread id, not equal to OS provided ID

#endif
