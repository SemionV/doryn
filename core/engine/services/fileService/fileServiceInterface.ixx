module;

#include <string>
#include <filesystem>

export module dory.core.engine.services.fileServiceInterface;

import dory.core.typeTraits;

export namespace dory::core::engine::services
{
    template<typename TImplementation>
    class IFileService: typeTraits::NonCopyable
    {
    public:
        std::string read(const std::filesystem::path& filePath)
        {
            return static_cast<TImplementation*>(this)->readImpl(filePath);
        }

        void write(const std::filesystem::path& filePath, const std::string& content)
        {
            return static_cast<TImplementation*>(this)->writeImpl(filePath, content);
        }
    };
}
