#pragma once

#include "iLibrary.h"
#include <cassert>

namespace dory::core::extensionPlatform
{
    template<typename TResource>
    class ResourceRef
    {
    private:
        std::optional<std::shared_ptr<ILibrary>> _library;
        TResource _resource;

    public:
        explicit ResourceRef(std::optional<std::shared_ptr<ILibrary>> library, TResource resource):
                _library(std::move(library)),
                _resource(std::move(resource))
        {}

        explicit operator bool()
        {
            return (!_library || (bool) *_library);
        }

        inline TResource& operator*()
        {
            assert((bool)this);
            return _resource;
        }

        inline TResource& operator->()
        {
            assert((bool)this);
            return _resource;
        }
    };
}