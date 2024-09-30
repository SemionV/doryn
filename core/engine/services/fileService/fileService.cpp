/*module;

#include <string>
#include <filesystem>

module dory.core.engine.services.fileService;

namespace dory::core::engine::services
{
    static std::string FileService::readImpl(const std::filesystem::path& filePath)
    {
        auto stream = std::ifstream(filePath);
        stream.exceptions(std::ios_base::badbit);

        if(!stream.is_open())
        {
            throw std::ios_base::failure("cannot open file " + filePath.string());
        }

        auto result = std::string();

        constexpr auto read_size = std::size_t(4096);
        auto buffer = std::string(read_size, '\0');
        while (stream.read(& buffer[0], read_size))
        {
            result.append(buffer, 0, stream.gcount());
        }
        result.append(buffer, 0, stream.gcount());

        return result;
    }

    static void FileService::writeImpl(const std::filesystem::path& filePath, const std::string& content)
    {
        std::ofstream stream(filePath.string(), std::ios::trunc);
        if(!stream.is_open())
        {
            throw std::ios_base::failure("cannot open file " + filePath.string());
        }

        stream << content;
        stream.close();
    }
}*/

module dory.core.engine.services.fileService;