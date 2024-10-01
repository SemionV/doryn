module;

#include <string>
#include <filesystem>

export module dory.core.engine.services.fileService;

export import dory.core.engine.services.fileServiceInterface;

namespace dory::core::engine::services
{
    export class FileService: public IFileService<FileService>
    {
    public:
        static std::string readImpl(const std::filesystem::path& filePath);
        static void writeImpl(const std::filesystem::path& filePath, const std::string& content);
    };
}
