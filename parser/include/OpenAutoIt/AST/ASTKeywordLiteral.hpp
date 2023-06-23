#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTKeywordLiteral final : public ASTExpression
{
public:
    ASTKeywordLiteral(TokenKind keyword)
        : m_Keyword{keyword}
    {
        m_NodeType = ASTNodeType::KeywordLiteral;
        PHI_ASSERT(keyword == TokenKind::KW_Default || keyword == TokenKind::KW_Null);
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        return indent_times(indent) + "KeywordLiteral [" + enum_name(m_Keyword) + "]";
    }

    // TODO: Make private
public:
    TokenKind m_Keyword;
};
} // namespace OpenAutoIt
