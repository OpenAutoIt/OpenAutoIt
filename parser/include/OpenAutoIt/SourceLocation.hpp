#pragma once

#include "OpenAutoIt/SourceFile.hpp"
#include <phi/core/observer_ptr.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{

struct SourceLocation
{
    phi::observer_ptr<const SourceFile> source_file;
    phi::u64                            line_number{0u};
    phi::u64                            column{0u};
};

template <typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream,
                                               const SourceLocation&               source_location)
{
    return stream << source_location.source_file->m_FilePath.string() << ":"
              << source_location.line_number << ":" << source_location.column;
}

} // namespace OpenAutoIt
