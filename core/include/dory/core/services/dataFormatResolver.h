#pragma once

#include <dory/core/services/iDataFormatResolver.h>

namespace dory::core::services
{
    class DataFormatResolver: public IDataFormatResolver
    {
    private:
        constexpr static auto yaml = "yaml";
        constexpr static auto json = "json";
        constexpr static auto scene = "scene";

    public:
        resources::DataFormat resolveFormat(std::string_view key) override;
        resources::DataFormat resolveFormat(const std::filesystem::path& path) override;
    };
}
