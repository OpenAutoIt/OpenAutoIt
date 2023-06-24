#pragma once

#include "OpenAutoIt/SourceFile.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/forward/string_view.hpp>
#include <filesystem>
#include <unordered_map>

namespace OpenAutoIt
{

// This class manages all the source files used
class SourceManager
{
public:
    [[nodiscard]] phi::observer_ptr<const SourceFile> LoadFile(
            const std::filesystem::path& file_path);

    [[nodiscard]] phi::boolean LoadFileFromDisk(const std::filesystem::path& file_path);
    void LoadFileFromMemory(phi::string_view file_name, phi::string_view content);

    [[nodiscard]] phi::boolean IsFileLoaded(const std::filesystem::path& file_path) const;

    [[nodiscard]] phi::observer_ptr<const SourceFile> GetSourceFile(
            const std::filesystem::path& file_path) const;

private:
    void AppendSourceFile(SourceFile&& file);

    std::vector<std::string>                    m_SourceFileContents;
    std::unordered_map<std::string, SourceFile> m_SourceFiles;
};

} // namespace OpenAutoIt
