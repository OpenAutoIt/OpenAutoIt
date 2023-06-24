#pragma once

#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/type_traits/to_underlying.hpp>
#include <phi/type_traits/underlying_type.hpp>
#include <filesystem>

namespace OpenAutoIt
{

/// This class represents a single source file on disk or in memory
class SourceFile
{
public:
    enum class Type
    {
        Basic  = 0,
        System = 1 << 0,
    };

    SourceFile(const Type type, std::filesystem::path file_path, phi::string_view content);

    [[nodiscard]] phi::boolean IsBasic() const;

    [[nodiscard]] phi::boolean IsSystem() const;

    Type                  m_Type;
    std::filesystem::path m_FilePath;
    phi::string_view      m_Content;
};

constexpr SourceFile::Type operator&(const SourceFile::Type lhs, const SourceFile::Type rhs)
{
    return static_cast<SourceFile::Type>(phi::to_underlying(lhs) & phi::to_underlying(rhs));
}

constexpr SourceFile::Type& operator&=(SourceFile::Type& lhs, const SourceFile::Type rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

constexpr SourceFile::Type operator|(const SourceFile::Type lhs, const SourceFile::Type rhs)
{
    return static_cast<SourceFile::Type>(phi::to_underlying(lhs) | phi::to_underlying(rhs));
}

constexpr SourceFile::Type& operator|=(SourceFile::Type& lhs, const SourceFile::Type rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

} // namespace OpenAutoIt

namespace std
{
template <>
struct hash<OpenAutoIt::SourceFile::Type>
{
    [[nodiscard]] size_t operator()(const OpenAutoIt::SourceFile::Type& type) const noexcept
    {
        return static_cast<size_t>(phi::to_underlying(type));
    }
};

} // namespace std
