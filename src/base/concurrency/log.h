#pragma once

#include "base/dependencies.h"

namespace dory::concurrency
{
    template<typename TStream>
    class Log
    {
    private:
        std::mutex streamMutex;
        TStream& stream;
    public:
        explicit Log(TStream& stream):
                stream(stream)
        {}

        template<typename... Ts>
        void print(const Ts... arguments)
        {
            std::lock_guard<std::mutex> lock(streamMutex);
            (stream << ... << arguments);
        }

        template<typename... Ts>
        void printLine(const Ts... arguments)
        {
            print(arguments..., "\n");
        }

        template<typename S, typename T>
        friend Log<S>& operator << (Log<S>& log, const T& argument);
    };

    template<typename TStream, typename T>
    Log<TStream>& operator << (Log<TStream>& log, const T& argument)
    {
        log.print(argument);
        return log;
    }
}