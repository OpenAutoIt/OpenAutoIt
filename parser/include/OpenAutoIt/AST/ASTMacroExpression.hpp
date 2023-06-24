#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{

class ASTMacroExpression final : public ASTExpression
{
public:
    ASTMacroExpression(TokenKind keyword)
        : m_Macro{keyword}
    {
        m_NodeType = ASTNodeType::MacroExpression;
        PHI_ASSERT(static_cast<phi::size_t>(m_Macro) >= MacroFirst &&
                   static_cast<phi::size_t>(m_Macro) <= MacroLast);
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        return indent_times(indent) + "MacroExpression [" + enum_name(m_Macro) + "]";
    }

    // TODO: Make private
public:
    TokenKind m_Macro;
};

} // namespace OpenAutoIt
