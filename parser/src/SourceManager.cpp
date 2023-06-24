#include "OpenAutoIt/SourceManager.hpp"

#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/observer_ptr.hpp>
#include <cstdlib>
#include <filesystem>

namespace OpenAutoIt
{

phi::observer_ptr<const SourceFile> SourceManager::LoadFile(const std::filesystem::path& file_path)
{
    if (IsFileLoaded(file_path))
    {
        return GetSourceFile(file_path);
    }

    if (LoadFileFromDisk(file_path))
    {
        PHI_ASSERT(IsFileLoaded(file_path));
        return GetSourceFile(file_path);
    }

    return {};
}

phi::boolean SourceManager::LoadFileFromDisk(const std::filesystem::path& file_path)
{
    std::filesystem::path absolute_path = std::filesystem::absolute(file_path);

    // No need to load a file twice
    if (IsFileLoaded(absolute_path))
    {
        return true;
    }

    auto content_opt = read_file(absolute_path);
    if (!content_opt)
    {
        return false;
    }

    m_SourceFileContents.emplace_back(phi::move(content_opt.value()));

    SourceFile file{SourceFile::Type::Basic, absolute_path, m_SourceFileContents.back()};
    AppendSourceFile(phi::move(file));

    return true;
}

void SourceManager::LoadFileFromMemory(phi::string_view file_name, phi::string_view content)
{
    SourceFile file{SourceFile::Type::Basic, std::filesystem::path(file_name.data()),
                    phi::move(content)};
    AppendSourceFile(phi::move(file));
}

phi::boolean SourceManager::IsFileLoaded(const std::filesystem::path& file_path) const
{
    return m_SourceFiles.contains(std::filesystem::absolute(file_path));
}

phi::observer_ptr<const SourceFile> SourceManager::GetSourceFile(
        const std::filesystem::path& file_path) const
{
    auto iterator = m_SourceFiles.find(std::filesystem::absolute(file_path));
    if (iterator != m_SourceFiles.end())
    {
        return &iterator->second;
    }

    return nullptr;
}

void SourceManager::AppendSourceFile(SourceFile&& file)
{
    PHI_ASSERT(!m_SourceFiles.contains(file.m_FilePath));

    m_SourceFiles.emplace(std::filesystem::absolute(file.m_FilePath), phi::move(file));
}

} // namespace OpenAutoIt
