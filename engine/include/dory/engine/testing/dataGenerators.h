#pragma once

namespace dory::testing
{
    template<typename T, auto N = 10000>
    auto getArray()
    {
        auto data = std::make_shared<std::array<T, N>>();
        for(std::size_t i = 0; i < N; ++i)
        {
            (*data)[i] = (int)i + 1;
        }
        std::random_device rd;
        std::mt19937 gen {rd()};

        std::ranges::shuffle(*data, gen);

        return data;
    }

    template<typename T, auto N = 10000>
    auto getList()
    {
        auto rawData = getArray<T, N>();
        auto data = std::list<T>(rawData->size());

        std::size_t i = 0;
        for(auto& item : data)
        {
            item = (*rawData)[i++];
        }

        return data;
    }
}