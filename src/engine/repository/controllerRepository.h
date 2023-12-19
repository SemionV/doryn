#pragma once

#include <vector>
#include <ranges>
#include "engine/entity.h"

namespace dory::repository
{
    template<typename TController>
    class ControllerRepository
    {
    private:
        using Storage =  std::vector<entity::PipelineNode<TController>>;
        Storage storage;

    public:
        auto getAll()
        {
            return std::ranges::ref_view{storage};
        }
    };
}