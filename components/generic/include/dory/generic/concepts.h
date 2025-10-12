#pragma once

namespace dory::generic::concepts
{
    template<std::size_t Number>
    constexpr bool is_power_of_two = (Number & (Number - 1)) == 0;
}