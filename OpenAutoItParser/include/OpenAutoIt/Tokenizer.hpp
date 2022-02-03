#pragma once

#include "OpenAutoIt/Token.hpp"
#include <phi/container/string_view.hpp>
#include <vector>

namespace OpenAutoIt
{
    [[nodiscard]] std::vector<Token> Tokenize(phi::string_view data) noexcept;
}
