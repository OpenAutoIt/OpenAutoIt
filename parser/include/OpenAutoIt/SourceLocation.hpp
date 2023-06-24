#pragma once

#include <phi/core/observer_ptr.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{

class SourceFile;

struct SourceLocation
{
    phi::observer_ptr<const SourceFile> source_file;
    phi::u64                            line_number{0u};
    phi::u64                            column{0u};
};

} // namespace OpenAutoIt
