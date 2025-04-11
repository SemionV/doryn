//-----------------------------------------------------------------------------
//  Custom Assert System - Cross-platform, debugger-friendly assert macros
//  Supports optional debug-only checks and message-based assertions.
//-----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <sstream>

//Cause break into debugger with interruption code 3
#if defined(_MSC_VER)
    #define debugBreak() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #if defined(__i386__) || defined(__x86_64__)
        #define debugBreak() __asm__ volatile("int $0x03")
    #elif defined(__aarch64__) || defined(__arm__)
        #define debugBreak() __builtin_trap()
    #else
        #define debugBreak() __builtin_trap()
    #endif
#else
    #define debugBreak() ((void)0)
#endif

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
    template<bool>
    struct TAssert
    {
        TAssert() = delete;

        static void check(const bool, const char*)
        {}

        static void check(const bool condition, const char* expr, const char* file, const int line)
        {}
    };

    template<>
    struct TAssert<true>
    {
        TAssert() = delete;

        using AssertFailureHandlerType = std::function<void(const char*)>;
        static AssertFailureHandlerType assertFailureHandler;

        static void setHandler(AssertFailureHandlerType handler)
        {
            assertFailureHandler = std::move(handler);
        }

        static void check(const bool condition, const char* msg)
        {
            if(!condition)
            {
                if(assertFailureHandler)
                {
                    assertFailureHandler(msg);
                }

                debugBreak();
            }
        }

        static void check(const bool condition, const char* expr, const char* file, const int line)
        {
            if(!condition)
            {
                if(assertFailureHandler)
                {
                    std::ostringstream oss;
                    oss << "Assertion failed: " << expr << ", file: " << file << ", line: " << line;
                    assertFailureHandler(oss.str().c_str());
                }

                debugBreak();
            }
        }
    };

    struct Assert: public TAssert<ASSERT_ENABLED>
    {
        Assert() = delete;
    };

    struct InHouseAssert: public TAssert<IN_HOUSE_ASSERT_ENABLED>
    {
        InHouseAssert() = delete;
    };

    struct DebugAssert: public TAssert<DEBUG_ASSERT_ENABLED>
    {
        DebugAssert() = delete;
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

#define DORY_ASSERT(cond, msg) \
    ::dory::assert::Assert::check((cond), msg, __FILE__, __LINE__)

#define DORY_IN_HOUSE_ASSERT(cond, msg) \
    ::dory::assert::InHouseAssert::check((cond), msg, __FILE__, __LINE__)

#define DORY_DEBUG_ASSERT(cond, msg) \
    ::dory::assert::DebugAssert::check((cond), msg, __FILE__, __LINE__)