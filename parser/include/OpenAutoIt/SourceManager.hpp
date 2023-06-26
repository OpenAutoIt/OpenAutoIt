#pragma once

#include "OpenAutoIt/IncludeType.hpp"
#include "OpenAutoIt/SourceFile.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/forward/string_view.hpp>
#include <filesystem>
#include <unordered_map>

namespace OpenAutoIt
{

/// Abstract class for managing source files
class SourceManager
{
public:
    SourceManager()          = default;
    virtual ~SourceManager() = default;

    SourceManager(const SourceManager&) = delete;
    SourceManager(SourceManager&&)      = delete;

    SourceManager& operator=(const SourceManager&) = delete;
    SourceManager& operator=(SourceManager&&)      = delete;

    [[nodiscard]] virtual phi::observer_ptr<const SourceFile> LoadFile(
            const phi::string_view file_path, const IncludeType include_type) = 0;

    virtual void SetLocalSearchPath(const std::filesystem::path& search_path);
};

/// This class manages all the source files with access to the real filesystem
class RealFSSourceManager : public SourceManager
{
public:
    [[nodiscard]] phi::observer_ptr<const SourceFile> LoadFile(
            const phi::string_view file_path, const IncludeType include_type) override;

    void SetLocalSearchPath(const std::filesystem::path& search_path) override;

    [[nodiscard]] phi::boolean AddSearchPath(std::string search_path);

private:
    [[nodiscard]] phi::boolean LoadFileFromDisk(const std::filesystem::path& file_path);

    [[nodiscard]] phi::boolean IsFileLoaded(const std::filesystem::path& file_path) const;

    [[nodiscard]] phi::observer_ptr<const SourceFile> GetSourceFile(
            const std::filesystem::path& file_path) const;

    void AppendSourceFile(SourceFile&& file);

    [[nodiscard]] std::filesystem::path FindFile(const phi::string_view file_path,
                                                 const IncludeType      include_type) const;

    std::filesystem::path                       m_LocalSearchPath;
    std::vector<std::string>                    m_SearchPaths;
    std::vector<std::string>                    m_SourceFileContents;
    std::unordered_map<std::string, SourceFile> m_SourceFiles;
};

/// This class manages all the source files in memory and has no access to the actual filesystem
class VirtualSourceManager : public SourceManager
{
public:
    [[nodiscard]] phi::observer_ptr<const SourceFile> LoadFile(
            const phi::string_view file_path, const IncludeType include_type) override;

    void LoadFileFromMemory(phi::string_view file_name, phi::string_view content);

private:
    std::unordered_map<std::string_view, SourceFile> m_SourceFiles;
};

/// Source manager that actually manages no files and returns a nullptr for every file
class EmptySourceManager : public SourceManager
{
public:
    [[nodiscard]] phi::observer_ptr<const SourceFile> LoadFile(
            const phi::string_view file_path, const IncludeType include_type) override;
};

} // namespace OpenAutoIt
