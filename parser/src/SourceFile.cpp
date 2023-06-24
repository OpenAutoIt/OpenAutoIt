#include "OpenAutoIt/SourceFile.hpp"

#include <phi/core/move.hpp>

namespace OpenAutoIt
{

SourceFile::SourceFile(const Type type, std::filesystem::path file_path, phi::string_view content)
    : m_Type{type}
    , m_FilePath{phi::move(file_path)}
    , m_Content{phi::move(content)}
{}

phi::boolean SourceFile::IsBasic() const
{
    return m_Type == Type::Basic;
}

phi::boolean SourceFile::IsSystem() const
{
    return (m_Type & Type::System) == Type::System;
}

} // namespace OpenAutoIt
