#include <dory/core/services/dataFormatResolver.h>
#include <dory/generic/typeTraits.h>

namespace dory::core::services
{
    resources::DataFormat DataFormatResolver::resolveFormat(const std::string_view key)
    {
        auto lowerCaseKey = dory::generic::toLower(key);
        if(lowerCaseKey == yaml)
        {
            return resources::DataFormat::yaml;
        }
        else if(lowerCaseKey == json)
        {
            return resources::DataFormat::json;
        }

        return resources::DataFormat::unknown;
    }

    resources::DataFormat DataFormatResolver::resolveFormat(const std::filesystem::path& path)
    {
        auto extension = std::string{ path.extension().string() };
        if(!extension.empty() && extension[0] == '.')
        {
            return resolveFormat(std::string_view{ ++extension.begin(), extension.end() });
        }

        return resolveFormat((std::string_view)extension);
    }
}