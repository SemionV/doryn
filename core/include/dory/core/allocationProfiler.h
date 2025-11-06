#pragma once

#include <iostream>
#include <spdlog/fmt/fmt.h>

namespace dory::core
{
    class AllocProfiler
    {
    private:
        template<typename... Args>
        void print(fmt::format_string<Args...> messageTemplate, Args&&... args)
        {
            fmt::memory_buffer buffer;
            fmt::format_to(std::back_inserter(buffer), messageTemplate, std::forward<Args>(args)...);
            std::cout.write(buffer.data(), (std::streamsize)buffer.size());
            std::cout.put('\n');
        }
    public:
        void traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex);
        void traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex);
        void traceLargeAlloc(void* ptr, std::size_t size);
        void traceLargeFree(void* ptr);
    };
}
