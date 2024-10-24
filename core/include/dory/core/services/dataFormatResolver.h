#pragma once

#include <dory/core/services/iDataFormatResolver.h>

namespace dory::core::services
{
    class DataFormatResolver: public IDataFormatResolver
    {
    private:
        constexpr const static char* yaml = "yaml";
        constexpr const static char* json = "json";

    public:
        resources::DataFormat resolveFormat(std::string_view key) override;
        resources::DataFormat resolveFormat(const std::filesystem::path& path) override;
    };
}
