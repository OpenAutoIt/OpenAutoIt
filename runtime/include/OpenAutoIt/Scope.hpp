#pragma once

#include <OpenAutoIt/AST/ASTStatement.hpp>
#include <OpenAutoIt/Statements.hpp>
#include <OpenAutoIt/Variant.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/sized_types.hpp>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace OpenAutoIt
{
    enum class ScopeKind : phi::uint8_t
    {
        Function,
        Block,
    };

    class Scope
    {
    public:
        Scope(ScopeKind scope_kind, std::string_view scope_name,
              Statements& scope_statements) noexcept
            : kind{scope_kind}
            , name{scope_name}
            , statements{scope_statements}
        {}

        ScopeKind                                     kind;
        std::string_view                              name;
        std::unordered_map<std::string_view, Variant> variables;
        Statements&                                   statements;
        phi::usize                                    index{0u};
    };
} // namespace OpenAutoIt
