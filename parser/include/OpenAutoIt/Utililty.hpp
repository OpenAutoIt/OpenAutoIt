#pragma once

#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>
#include <filesystem>
#include <string>
#include <string_view>

namespace OpenAutoIt
{
    [[nodiscard]] inline std::string indent_times(phi::usize indent) noexcept
    {
        std::string ret;

        for (; indent > 0u; --indent)
        {
            ret += "  ";
        }

        return ret;
    }

    [[nodiscard]] phi::optional<std::string> read_file(
            const std::filesystem::path& file_path) noexcept;

    phi::boolean write_file(const std::filesystem::path& file_path, std::string_view data) noexcept;
} // namespace OpenAutoIt
