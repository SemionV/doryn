#pragma once

namespace dory::core::resources::factory
{
    template<typename TInstance>
    struct Instance
    {
        generic::extension::ResourceHandle<std::shared_ptr<TInstance>> instance {};
        std::string type {};
    };
}