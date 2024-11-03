#include <dory/core/services/assetTypeResolver.h>
#include <regex>

namespace dory::core::services
{

    std::optional<std::string> AssetTypeResolver::resolve(resources::DataContext& context, const std::filesystem::path& filePath)
    {
        for(const auto& [key, value] : context.configuration.assetTypes)
        {
            auto regex = std::regex{ value, std::regex_constants::icase };
            if(std::regex_search(filePath.c_str(), regex))
            {
                return key;
            }
        }

        return {};
    }
}