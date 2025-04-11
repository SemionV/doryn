//-----------------------------------------------------------------------------
//  Custom Assert System - Cross-platform, debugger-friendly assert macros
//  Supports optional debug-only checks and message-based assertions.
//-----------------------------------------------------------------------------

#pragma once

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

#if ASSERT_ENABLED

void reportAssertionFailure(const char* expressionString, const char* fileName, const int lineNumber) noexcept;

namespace dory::assert
{
}

using AssertHandlerFn = void(*)(const char* expr, const char* file, int line, const char* msg);

extern AssertHandlerFn g_AssertHandler;

#define doryAssert(expr) \
  do { \
    if (!(expr)) { \
      reportAssertionFailure(#expr, __FILE__, __LINE__); \
      debugBreak(); \
    } \
  } while(0)

#define doryAssertMsg(expr, msg) \
  do { \
    if (!(expr)) { \
      reportAssertionFailure(msg, __FILE__, __LINE__); \
      debugBreak(); \
    } \
  } while(0)

#if DEBUG_ASSERT_ENABLED
#define doryDebugAssert(expr) doryAssert(expr)
#else
#define doryDebugAssert(expr) ((void)0)
#endif

#else //ASSERT_ENABLED

#define doryAssert(expr) ((void)0)
#define doryAssertMsg(expr) ((void)0)
#define doryDebugAssert(expr) ((void)0)

#endif //ASSERT_ENABLED