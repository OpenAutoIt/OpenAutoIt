#pragma once

#include "OpenAutoIt/Variant.hpp"
#include <string_view>
#include <unordered_map>

namespace OpenAutoIt
{
    struct FunctionScope
    {
        FunctionScope(std::string_view scope_name) noexcept
            : name{scope_name}
        {}

        std::string_view                              name;
        std::unordered_map<std::string_view, Variant> variables;
    };
} // namespace OpenAutoIt
