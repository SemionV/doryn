#pragma once

#include <cassert>
#include <optional>
#include <memory>
#include "iLibrary.h"

namespace dory::generic::extension
{
    class LibraryHandle
    {
    private:
        std::optional<std::weak_ptr<ILibrary>> _libraryOption;

        std::shared_ptr<ILibrary> lock() const
        {
            assert(!isStatic());
            return (*_libraryOption).lock();
        }

    public:
        explicit LibraryHandle() = default;

        explicit LibraryHandle(std::weak_ptr<ILibrary> library):
                _libraryOption(std::move(library))
        {}

        bool isStatic() const
        {
            return !(bool)_libraryOption;
        }

        template<typename U, template<class> class TT>
        friend class ResourceHandleRoot;

        template<typename U>
        friend class ResourceHandle;
    };
}
