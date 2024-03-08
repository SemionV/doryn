#pragma once

namespace dory::domain
{
    template<typename TId>
    class IdFactory
    {
    private:
        TId counter;

    public:
        static const constexpr TId nullId{};

    public:
        TId generate()
        {
            return ++counter;
        }
    };
}