#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "phi/core/assert.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/types.hpp>
#include <iterator>

namespace OpenAutoIt
{
void TokenStream::push_back(const Token& value)
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(!m_Finalized);
#endif

    m_Tokens.push_back(value);
}

void TokenStream::push_back(Token&& value)
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(!m_Finalized);
#endif

    m_Tokens.push_back(value);
}

void TokenStream::finalize()
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(!m_Finalized);
#endif

    m_Index = 0u;
#if defined(PHI_DEBUG)
    m_Finalized = true;
#endif
}

void TokenStream::reset()
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    m_Index = 0u;
}

[[nodiscard]] PHI_ATTRIBUTE_PURE phi::boolean TokenStream::has_x_more(phi::usize amount) const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    for (phi::usize index = m_Index; amount > 0u; ++index, --amount)
    {
        if (index < m_Tokens.size())
        {
            return false;
        }
    }

    return true;
}

[[nodiscard]] PHI_ATTRIBUTE_PURE phi::boolean TokenStream::has_more() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Index < m_Tokens.size();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE phi::boolean TokenStream::reached_end() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Index >= m_Tokens.size();
}

PHI_ATTRIBUTE_PURE const Token& TokenStream::look_ahead() const
{
    PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens[m_Index.unsafe()];
}

PHI_ATTRIBUTE_PURE const Token& TokenStream::look_behind() const
{
    PHI_ASSERT(!m_Tokens.empty());
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    if (m_Index == 0u)
    {
        return m_Tokens[0u];
    }

    return m_Tokens[m_Index.unsafe() - 1u];
}

void TokenStream::consume()
{
    PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    m_Index += 1u;
}

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=noreturn")

void TokenStream::skip(phi::usize n)
{
    PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif
    PHI_ASSERT(has_x_more(n));
    PHI_ASSERT(n != 0u);

    m_Index += n;
}

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_ATTRIBUTE_PURE const Token* TokenStream::find_first_token_of_type(TokenKind type) const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    for (const Token& token : m_Tokens)
    {
        if (token.GetTokenKind() == type)
        {
            return &token;
        }
    }

    return nullptr;
}

PHI_ATTRIBUTE_PURE const Token* TokenStream::find_last_token_of_type(TokenKind type) const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    const Token* last = nullptr;

    for (const Token& token : m_Tokens)
    {
        if (token.GetTokenKind() == type)
        {
            last = &token;
        }
    }

    return last;
}

[[nodiscard]] PHI_ATTRIBUTE_PURE const Token& TokenStream::at(phi::usize index) const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif
    PHI_ASSERT(index < m_Tokens.size());

    return m_Tokens[index.unsafe()];
}

[[nodiscard]] PHI_ATTRIBUTE_PURE phi::usize TokenStream::size() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.size();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE phi::boolean TokenStream::is_empty() const
{
    return m_Tokens.empty();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE phi::usize TokenStream::current_position()
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Index;
}

void TokenStream::set_position(phi::usize index)
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    m_Index = index;
}

PHI_ATTRIBUTE_PURE TokenStream::const_iterator TokenStream::begin() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.begin();
}

PHI_ATTRIBUTE_PURE TokenStream::const_iterator TokenStream::cbegin() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.cbegin();
}

PHI_ATTRIBUTE_PURE TokenStream::const_iterator TokenStream::end() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.end();
}

PHI_ATTRIBUTE_PURE TokenStream::const_iterator TokenStream::cend() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.cend();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE TokenStream::const_reverse_iterator TokenStream::rbegin() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.rbegin();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE TokenStream::const_reverse_iterator TokenStream::rend() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.rend();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE const Token& TokenStream::front() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.front();
}

[[nodiscard]] PHI_ATTRIBUTE_PURE const Token& TokenStream::back() const
{
#if defined(PHI_DEBUG)
    PHI_ASSERT(m_Finalized);
#endif

    return m_Tokens.back();
}
} // namespace OpenAutoIt
