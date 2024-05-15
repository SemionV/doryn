#pragma once

#include "dependencies.h"

namespace dory
{
    std::string getTextFileContent(const std::filesystem::path& filename)
    {
        auto stream = std::ifstream(filename);
        stream.exceptions(std::ios_base::badbit);

        if(!stream.is_open())
        {
            throw std::ios_base::failure("cannot open file " + std::string(filename));
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
}