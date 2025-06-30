#pragma once
#include <cstddef>
#include <dory/macros/assert.h>

#if DORY_PLATFORM_LINUX
#include <sys/mman.h>
#elif DORY_PLATFORM_WIN32
#include <windows.h>
#include <cassert>
#include <limits>
#endif

namespace dory::memory
{
    enum class ErrorCode
    {
        Success,
        OutOfMemory
    };

    inline std::uintptr_t alignAddress(const std::uintptr_t address, const std::size_t align)
    {
        const std::size_t mask = align - 1;
        assert::debug((align & mask) == 0, "Alignment must be a power of 2");
        return (address + mask) & ~mask;
    }

    template<typename T>
    T* alignPointer(T* ptr, std::size_t align)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(ptr);
        const auto alignedAddress = alignAddress(address, align);
        return reinterpret_cast<T*>(alignedAddress);
    }

    std::size_t getSystemMemoryPageSize();
    void* reserveMemoryPages(std::size_t pageSize, std::size_t pagesCount);
    void releaseMemoryPages(void* ptr, std::size_t pageSize, std::size_t pagesCount);
    void commitPages(void* ptr, std::size_t pageSize, std::size_t pagesCount);


#if DORY_PLATFORM_LINUX
    inline std::size_t getSystemMemoryPageSize()
    {
        return sysconf(_SC_PAGESIZE);
    }

    inline void* reserveMemoryPages(const std::size_t pageSize, const std::size_t pagesCount)
    {
        assert::debug((std::numeric_limits<std::size_t>::max() / pageSize) > pagesCount, "Memory block size cannot be greater than max number of bytes in the system");
        void* ptr = mmap(nullptr, pageSize * pagesCount, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        const auto address = reinterpret_cast<uintptr_t>(ptr);
        const uintptr_t mask = -(address == reinterpret_cast<uintptr_t>(MAP_FAILED));
        return reinterpret_cast<void*>(address & ~mask);
    }

    inline void releaseMemoryPages(void* ptr, const std::size_t pageSize, const std::size_t pagesCount)
    {
        munmap(ptr, pageSize * pagesCount);
    }

    inline void commitMemoryPages(void* ptr, const std::size_t pageSize, const std::size_t pagesCount)
    {
        volatile char* bytes = static_cast<char*>(ptr);
        for(std::size_t i = 0; i < pagesCount; ++i)
        {
            bytes[i * pageSize] = 0;
        }
    }

    inline void copy(void* fromPtr, void* toPtr, std::size_t size)
    {
        std::memcpy(toPtr, fromPtr, size);
    }
#elif DORY_PLATFORM_WIN32
    inline std::size_t getSystemMemoryPageSize()
    {
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        return static_cast<std::size_t>(systemInfo.dwPageSize);
    }

    inline void* reserveMemoryPages(const std::size_t pageSize, const std::size_t pagesCount)
    {
        assert::debug((std::numeric_limits<std::size_t>::max() / pageSize) > pagesCount, "Memory block size cannot be greater than max number of bytes in the system");
        void* ptr = VirtualAlloc(nullptr, pageSize * pagesCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        return ptr;
    }

    inline void releaseMemoryPages(void* ptr, const std::size_t pageSize, const std::size_t pagesCount)
    {
        (void)pageSize; // unused on Windows
        (void)pagesCount;
        VirtualFree(ptr, 0, MEM_RELEASE);
    }

    inline void commitMemoryPages(void* ptr, const std::size_t pageSize, const std::size_t pagesCount)
    {
        void* committed = VirtualAlloc(ptr, pageSize * pagesCount, MEM_COMMIT, PAGE_READWRITE);
        assert::debug(committed != nullptr, "Failed to commit memory pages");
    }

    inline void copy(void* fromPtr, void* toPtr, std::size_t size)
    {
        std::memcpy(toPtr, fromPtr, size);
    }
#endif
}