module;

#include <string>
#include <filesystem>

export module dory.core.engine.services.fileService;

//import dory.core.engine.services.fileServiceInterface;

export namespace dory::core::engine::services
{
    class FileService/*: public IFileService<FileService>*/
    {
    public:
        static std::string readImpl(const std::filesystem::path& filePath);
        static void writeImpl(const std::filesystem::path& filePath, const std::string& content);
    };
}
