#pragma once

namespace OpenAutoIt
{

/// The type of include
///
/// The only difference really only happens if you have both a local file and a file on the search path with the same name.
/// In that case a local include will include the file from the current working directory while a global include will use the one
/// from the search path.
enum class IncludeType : bool
{
    Local,  /// An include like this '#include "foo.au3"'
    Global, /// An include like this '#include <foo.au3>'
};

} // namespace OpenAutoIt
