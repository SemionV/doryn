//-----------------------------------------------------------------------------
//  Custom Assert System - Cross-platform, debugger-friendly assert macros
//  Supports optional debug-only checks and message-based assertions.
//-----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <sstream>

#if defined(DORY_PLATFORM_LINUX) || defined(DORY_PLATFORM_APPLE)
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/ptrace.h>

    inline bool doryIsDebuggerAttached()
    {
    #if defined(__APPLE__)
        // macOS specific implementation
        int mib[4];
        struct kinfo_proc info;
        size_t size = sizeof(info);
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PID;
        mib[3] = getpid();
        sysctl(mib, 4, &info, &size, NULL, 0);
        return (info.kp_proc.p_flag & P_TRACED) != 0;
    #else
        // Linux implementation
        static bool checked = false;
        static bool attached = false;

        if (!checked)
        {
            checked = true;
            attached = (ptrace(PTRACE_TRACEME, 0, nullptr, 0) == -1);
            if (!attached) ptrace(PTRACE_DETACH, 0, nullptr, 0);
        }
        return attached;
    #endif
    }
#elif defined(DORY_PLATFORM_WIN32)
#include <windows.h>

    inline bool doryIsDebuggerAttached()
    {
        return ::IsDebuggerPresent();
    }
#endif

inline void dorySafeDebugBreak()
{
    if (doryIsDebuggerAttached())
    {
    #if defined(_MSC_VER)
        __debugbreak();
    #elif defined(__clang__) || defined(__GNUC__)
        #if defined(__i386__) || defined(__x86_64__)
            __asm__ volatile("int $0x03");
        #else
            __builtin_trap();
        #endif
    #endif
    }
}

#define doryDebugBreak() dorySafeDebugBreak()

#ifndef ASSERT_ENABLED
#define ASSERT_ENABLED 0
#endif

#ifndef IN_HOUSE_ASSERT_ENABLED
#define IN_HOUSE_ASSERT_ENABLED 0
#endif

#ifndef DEBUG_ASSERT_ENABLED
#define DEBUG_ASSERT_ENABLED 0
#endif

namespace dory::assert
{
    struct AssertBase
    {
        using AssertFailureHandlerType = std::function<void(const char*)>;
    };

    template<bool, typename = void, typename = void>
    struct AssertLayer: public AssertBase
    {
        AssertLayer() = delete;

        static void handleAssert(const bool, const char*, bool&)
        {}

        static void check(const bool, const char*)
        {}
    };

    template<typename TImplementation, typename TUpperLevelAssert>
    struct AssertLayer<true, TImplementation, TUpperLevelAssert>: public AssertBase
    {
        AssertLayer() = delete;

        static void handleAssert(const bool condition, const char* msg, bool& isHandled)
        {
            if(!condition)
            {
                if(TImplementation::assertFailureHandler)
                {
                    TImplementation::assertFailureHandler(msg);
                }
            }

            isHandled = true;
        }

        static void check(const bool condition, const char* msg)
        {
            bool handled = false;
            TUpperLevelAssert::handleAssert(condition, msg, handled);
            if(!handled)
            {
                handleAssert(condition, msg, handled);
            }
        }
    };

    struct DebugAssert: public AssertLayer<DEBUG_ASSERT_ENABLED, DebugAssert, AssertLayer<false>>
    {
        static AssertBase::AssertFailureHandlerType assertFailureHandler;

        DebugAssert() = delete;
    };

    struct InHouseAssert: public AssertLayer<IN_HOUSE_ASSERT_ENABLED, InHouseAssert, DebugAssert>
    {
        static AssertBase::AssertFailureHandlerType assertFailureHandler;

        InHouseAssert() = delete;
    };

    struct Assert: public AssertLayer<ASSERT_ENABLED, Assert, InHouseAssert>
    {
        static AssertBase::AssertFailureHandlerType assertFailureHandler;

        Assert() = delete;
    };

    inline void debug(const bool condition, const char* msg)
    {
        DebugAssert::check(condition, msg);
    }

    inline void inhouse(const bool condition, const char* msg)
    {
        InHouseAssert::check(condition, msg);
    }

    inline void release(const bool condition, const char* msg)
    {
        Assert::check(condition, msg);
    }
}