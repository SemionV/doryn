#pragma once

#include "iLibrary.h"
#include <cassert>

namespace dory::core::extensionPlatform
{
    class LibraryHandle
    {
    private:
        std::optional<std::weak_ptr<ILibrary>> _libraryOption;

        std::shared_ptr<ILibrary> lock()
        {
            assert(!isStatic());
            return (*_libraryOption).lock();
        }

    public:
        explicit LibraryHandle() = default;

        explicit LibraryHandle(std::weak_ptr<ILibrary> library):
                _libraryOption(std::move(library))
        {}

        bool isStatic()
        {
            return !(bool)_libraryOption;
        }

        template<typename U>
        friend class ResourceHandle;
    };
}
