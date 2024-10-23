#pragma once

#include <cassert>

namespace dory::core
{
    namespace extensionPlatform
    {
        template<typename TResource>
        struct ResourcePresentTrait
        {
            static bool isResourcePresent(const TResource& resource)
            {
                return true;
            }
        };

        template<typename T>
        struct ResourcePresentTrait<std::shared_ptr<T>>
        {
            static bool isResourcePresent(const std::shared_ptr<T>& resource)
            {
                return static_cast<bool>(resource);
            }
        };

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

            bool isLibraryPresent()
            {
                return (!_library || (bool) *_library);
            }

            explicit operator bool()
            {
                return isLibraryPresent() && ResourcePresentTrait<TResource>::isResourcePresent(_resource);
            }

            inline TResource& operator*()
            {
                assert((bool)this);
                return _resource;
            }

            inline TResource& operator->()
            {
                assert(this->operator bool());
                return _resource;
            }

            inline bool operator==(const TResource& otherResource)
            {
                return otherResource == _resource;
            }
        };
    }
}