#pragma once

#include <phi/core/boolean.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>
#include <phi/forward/string_view.hpp>
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

[[nodiscard]] phi::optional<std::string> read_file(const std::filesystem::path& file_path) noexcept;

phi::boolean write_file(const std::filesystem::path& file_path, std::string_view data) noexcept;

void out(std::string_view data);

void err(std::string_view data);

void disable_output();

[[nodiscard]] phi::boolean string_equals_ignore_case(const phi::string_view lhs,
                                                     const phi::string_view rhs) noexcept;
} // namespace OpenAutoIt
