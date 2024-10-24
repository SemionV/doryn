#pragma once

#include <filesystem>
#include <memory>
#include <utility>


namespace dory::generic::extension
{
    class ILibrary
    {
    public:
#ifdef DORY_PLATFORM_WIN32
        const constexpr static std::string_view systemSharedLibraryFileExtension = ".dll";
#endif
#ifdef DORY_PLATFORM_LINUX
        const constexpr static std::string_view systemSharedLibraryFileExtension = ".so";
#endif

    public:
        const std::string name;
        const std::filesystem::path path;

        ILibrary() = default;
        virtual ~ILibrary() = default;

        ILibrary(std::string  libraryName, std::filesystem::path  libraryPath):
                name(std::move(libraryName)), path(std::move(libraryPath))
        {}

        virtual bool isLoaded() = 0;
    };
}