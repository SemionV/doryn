#pragma once

#include "base/typeComponents.h"

namespace dory::domain
{
    template<typename TServiceLocator>
    class Service
    {
    protected:
        TServiceLocator& services;
    public:
        explicit Service(TServiceLocator& serviceLocator):
                services(serviceLocator)
        {}
    };
}
