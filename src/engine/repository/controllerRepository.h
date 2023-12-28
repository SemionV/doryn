#pragma once

#include <vector>
#include <ranges>
#include <algorithm>
#include "engine/entity.h"

namespace dory::repository
{
    template<typename TController>
    class ControllerRepository
    {
    private:
        std::vector<entity::PipelineNode<TController>> storage;

    private:
        void sort()
        {
            std::ranges::sort(storage, std::ranges::greater(), &entity::PipelineNode<TController>::priority);
        }

    public:
        auto getAll()
        {
            return std::ranges::ref_view{storage};
        }
    };
}