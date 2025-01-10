#pragma once

namespace dory::generic
{
    template<typename T>
    class IStream
    {
    public:
        virtual ~IStream() = default;
        virtual void send(T&& item) = 0;
        virtual bool receive(T& item) = 0;
    };
}
