#pragma once

#include "OpenAutoIt/SourceLocation.hpp"
#include <phi/container/string_view.hpp>

namespace OpenAutoIt
{
class ParseNote
{
public:
    phi::string_view m_Text;
    SourceLocation   m_Location;
};
} // namespace OpenAutoIt
