#include "OpenAutoIt/Utililty.hpp"

#include <phi/core/scope_guard.hpp>
#include <cstdio>

namespace OpenAutoIt
{
    phi::optional<std::string> read_file(const std::filesystem::path& file_path) noexcept
    {
        // Check that the file actually exists
        if (!std::filesystem::exists(file_path))
        {
            return {};
        }

        std::FILE* file        = std::fopen(file_path.c_str(), "r");
        auto       scope_guard = phi::make_scope_guard([&file]() { (void)std::fclose(file); });

        if (file == nullptr)
        {
            // Failed to open file
            return {};
        }

        (void)std::fseek(file, 0, SEEK_END);

        const long ftell_size = std::ftell(file);
        if (ftell_size == -1L)
        {
            // Failed to optain size
            return {};
        }
        const std::size_t size = static_cast<std::size_t>(ftell_size);

        std::rewind(file);

        // Create empty string of desired size
        std::string str(size, '\0');

        // Read data
        (void)std::fread(str.data(), sizeof(std::string::value_type), size, file);

        return phi::move(str);
    }

    phi::boolean write_file(const std::filesystem::path& file_path, std::string_view data) noexcept
    {
        std::FILE* file        = std::fopen(file_path.c_str(), "w");
        auto       scope_guard = phi::make_scope_guard([&file]() { (void)std::fclose(file); });

        if (file == nullptr)
        {
            return false;
        }

        (void)std::fwrite(data.data(), sizeof(data.front()), data.size(), file);
        return true;
    }
} // namespace OpenAutoIt
