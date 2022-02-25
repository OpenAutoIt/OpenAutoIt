#pragma once

#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/Token.hpp"
#include <vector>

namespace OpenAutoIt
{
    [[nodiscard]] ASTDocument ParseAST(const std::vector<Token>& tokens) noexcept;
}
