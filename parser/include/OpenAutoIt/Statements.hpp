#pragma once

#include <OpenAutoIt/AST/ASTStatement.hpp>
#include <phi/core/scope_ptr.hpp>
#include <vector>

namespace OpenAutoIt
{
using Statements = std::vector<phi::not_null_scope_ptr<ASTStatement>>;
}
