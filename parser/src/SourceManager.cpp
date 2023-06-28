#include "OpenAutoIt/SourceManager.hpp"

#include "OpenAutoIt/IncludeType.hpp"
#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/observer_ptr.hpp>
#include <cstdlib>
#include <filesystem>

namespace OpenAutoIt
{

void SourceManager::SetLocalSearchPath(const std::filesystem::path& /*search_path*/)
{}

phi::observer_ptr<const SourceFile> RealFSSourceManager::LoadFile(const phi::string_view file_path,
                                                                  const IncludeType include_type)
{
    const std::filesystem::path path = FindFile(file_path, include_type);

    if (IsFileLoaded(path))
    {
        return GetSourceFile(path);
    }

    if (LoadFileFromDisk(path))
    {
        PHI_ASSERT(IsFileLoaded(path));
        return GetSourceFile(path);
    }

    return {};
}

void RealFSSourceManager::SetLocalSearchPath(const std::filesystem::path& search_path)
{
    std::error_code error_code;
    m_LocalSearchPath = std::filesystem::absolute(search_path, error_code);
}

phi::boolean RealFSSourceManager::LoadFileFromDisk(const std::filesystem::path& file_path)
{
    PHI_ASSERT(!IsFileLoaded(file_path));

    // Check that the file is a regular file and actually exists
    std::error_code error;
    if (!std::filesystem::is_regular_file(file_path, error) ||
        !std::filesystem::exists(file_path, error))
    {
        return false;
    }

    auto content_opt = read_file(file_path);
    if (!content_opt)
    {
        return false;
    }

    m_SourceFileContents.emplace_back(phi::move(content_opt.value()));

    SourceFile file{SourceFile::Type::Basic, file_path, m_SourceFileContents.back()};
    AppendSourceFile(phi::move(file));

    return true;
}

phi::boolean RealFSSourceManager::IsFileLoaded(const std::filesystem::path& file_path) const
{
    return m_SourceFiles.contains(file_path.string());
}

phi::observer_ptr<const SourceFile> RealFSSourceManager::GetSourceFile(
        const std::filesystem::path& file_path) const
{
    auto iterator = m_SourceFiles.find(file_path.string());
    if (iterator != m_SourceFiles.end())
    {
        return &iterator->second;
    }

    return nullptr;
}

void RealFSSourceManager::AppendSourceFile(SourceFile&& file)
{
    PHI_ASSERT(!m_SourceFiles.contains(file.m_FilePath.string()));

    m_SourceFiles.emplace(file.m_FilePath.string(), phi::move(file));
}

std::filesystem::path RealFSSourceManager::FindFile(const phi::string_view file_path,
                                                    const IncludeType      include_type) const
{
    // No need to look if we already have an absolute path
    if (std::filesystem::path(std::string_view(file_path)).is_absolute())
    {
        return {std::string_view(file_path)};
    }

    // For a local include we try the local search path first
    if (include_type == IncludeType::Local)
    {
        if (std::filesystem::exists(m_LocalSearchPath / std::string_view(file_path)))
        {
            return m_LocalSearchPath / std::string_view{file_path};
        }
    }

    // Search through all registered search paths
    for (const auto& search_path : m_SearchPaths)
    {
        const auto path = std::filesystem::path(search_path) / std::string_view(file_path);

        if (std::filesystem::exists(path))
        {
            return phi::move(path);
        }
    }

    // For a global include we try the local search path last
    if (include_type == IncludeType::Global)
    {
        if (std::filesystem::exists(m_LocalSearchPath / std::string_view(file_path)))
        {
            return m_LocalSearchPath / std::string_view{file_path};
        }
    }

    return {};
}

phi::observer_ptr<const SourceFile> VirtualSourceManager::LoadFile(
        const phi::string_view file_path, const IncludeType /* include_type*/)
{
    const auto iterator = m_SourceFiles.find(file_path);
    if (iterator != m_SourceFiles.end())
    {
        return &iterator->second;
    }

    return {};
}

void VirtualSourceManager::LoadFileFromMemory(phi::string_view file_name, phi::string_view content)
{
    PHI_ASSERT(!file_name.is_empty());

    SourceFile file{SourceFile::Type::Basic, std::filesystem::path(std::string_view(file_name)),
                    phi::move(content)};

    m_SourceFiles.emplace(file_name, phi::move(file));
}

phi::observer_ptr<const SourceFile> EmptySourceManager::LoadFile(
        const phi::string_view /*file_path*/, const IncludeType /*include_type*/)
{
    return {};
}

} // namespace OpenAutoIt
