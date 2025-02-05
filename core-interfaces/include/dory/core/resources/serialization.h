#pragma once

namespace dory::core::resources::serialization
{
    template<typename TInstance>
    struct FactoryInstance
    {
        generic::extension::ResourceHandle<std::shared_ptr<TInstance>> instance {};
        std::string type {};
    };
}