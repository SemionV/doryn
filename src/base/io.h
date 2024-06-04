#pragma once

#include "dependencies.h"

namespace dory
{
    std::string readFromFile(const std::filesystem::path& filename)
    {
        auto stream = std::ifstream(filename);
        stream.exceptions(std::ios_base::badbit);

        if(!stream.is_open())
        {
            throw std::ios_base::failure("cannot open file " + filename.string());
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

    void writeToFile(const std::filesystem::path& filename, const std::string& content)
    {
        std::ofstream stream(filename.string(), std::ios::trunc);
        if(!stream.is_open())
        {
            throw std::ios_base::failure("cannot open file " + filename.string());
        }

        stream << content;
        stream.close();
    }
}