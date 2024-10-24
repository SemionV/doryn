#pragma once

#include <string>
#include <filesystem>
#include <dory/core/resources/dataFormat.h>

namespace dory::core::services
{
    class IDataFormatResolver
    {
    public:
        virtual ~IDataFormatResolver() = default;

        virtual resources::DataFormat resolveFormat(const std::string_view key) = 0;
        virtual resources::DataFormat resolveFormat(const std::filesystem::path& path) = 0;
    };
}
