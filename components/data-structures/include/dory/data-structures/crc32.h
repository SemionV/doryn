#pragma once
#include <array>
#include <cstdint>

namespace dory::data_structures::containers
{
    using CRC32Table =  std::array<uint32_t, 256>;

    class CRC32
    {
    private:
        static constexpr uint32_t Polynomial = 0xEDB88320u; // IEEE, reversed

    public:
        // Generate 256-entry lookup table at compile time
        static std::array<uint32_t, 256> generateTable() noexcept
        {
            std::array<uint32_t, 256> table{};

            for (uint32_t i = 0; i < 256; ++i)
            {
                uint32_t crc = i;
                for (int bit = 0; bit < 8; ++bit)
                {
                    if (crc & 1u)
                        crc = (crc >> 1) ^ Polynomial;
                    else
                        crc >>= 1;
                }
                table[i] = crc;
            }

            return table;
        }

        // Core compute over raw bytes
        static uint32_t compute(const void* data, const std::size_t size, const CRC32Table& table) noexcept
        {
            const auto* bytes = static_cast<const std::uint8_t*>(data);
            uint32_t crc = 0xFFFFFFFFu;

            for (std::size_t i = 0; i < size; ++i)
            {
                const uint8_t b = bytes[i];
                crc = (crc >> 8) ^ table[(crc ^ b) & 0xFFu];
            }

            return crc ^ 0xFFFFFFFFu;
        }

        // Convenience overload for C-strings (null-terminated)
        static uint32_t compute(const char* cstr, const CRC32Table& table) noexcept
        {
            if (!cstr)
                return 0;

            const char* p = cstr;
            std::size_t len = 0;
            while (*p++)
                ++len;

            return compute(cstr, len, table);
        }
    };
}
