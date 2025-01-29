#include <dory/core/services/dataFormatResolver.h>
#include <dory/generic/typeTraits.h>

namespace dory::core::services
{
    resources::DataFormat DataFormatResolver::resolveFormat(const std::string_view key)
    {
        const auto lowerCaseKey = dory::generic::toLower(key);
        if(lowerCaseKey == yaml)
        {
            return resources::DataFormat::yaml;
        }

        if(lowerCaseKey == json)
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

        return resolveFormat(static_cast<std::string_view>(extension));
    }
}