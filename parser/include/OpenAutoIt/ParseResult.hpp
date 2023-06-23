#pragma once

#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/ParseError.hpp"
#include "OpenAutoIt/ParseWarning.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <phi/core/scope_ptr.hpp>
#include <vector>

namespace OpenAutoIt
{
class ParseResult
{
public:
    void Clear()
    {
        m_TokenStream.clear();
        m_Document.clear();
        m_Errors.clear();
        m_Warnings.clear();
    }

    TokenStream                 m_TokenStream;
    phi::scope_ptr<ASTDocument> m_Document;
    std::vector<ParseError>     m_Errors;
    std::vector<ParseWarning>   m_Warnings;
};
} // namespace OpenAutoIt
