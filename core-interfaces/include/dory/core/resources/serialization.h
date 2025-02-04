#pragma once

namespace dory::core::resources::serialization
{
    template<typename TInstance>
    struct FactoryInstance
    {
        //TODO: use ResourceHandle instead, support hot reloading
        std::unique_ptr<TInstance> instance {};
        std::string type {};
    };
}