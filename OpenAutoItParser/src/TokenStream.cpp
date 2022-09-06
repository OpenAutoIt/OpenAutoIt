#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "phi/core/assert.hpp"
#include <phi/core/types.hpp>
#include <iterator>

namespace OpenAutoIt
{
    void TokenStream::push_back(const Token& value) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finialized);
#endif

        m_Tokens.push_back(value);
    }

    void TokenStream::push_back(Token&& value) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finialized);
#endif

        m_Tokens.push_back(value);
    }

    void TokenStream::finalize() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finialized);
#endif

        m_Index = 0u;
#if defined(PHI_DEBUG)
        m_Finialized = true;
#endif
    }

    void TokenStream::reset() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Index = 0u;
    }

    phi::boolean TokenStream::has_x_more(phi::usize amount) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
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

    phi::boolean TokenStream::has_more() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Index < m_Tokens.size();
    }

    phi::boolean TokenStream::reached_end() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Index >= m_Tokens.size();
    }

    const Token& TokenStream::look_ahead() const noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens[m_Index.unsafe()];
    }

    void TokenStream::consume() noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Index += 1u;
    }

    void TokenStream::skip(phi::usize n) noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif
        PHI_ASSERT(has_x_more(n));
        PHI_ASSERT(n != 0u);

        for (; n > 0u; --n, ++m_Index)
        {}
    }

    const Token* TokenStream::find_first_token_of_type(TokenKind type) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
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

    const Token* TokenStream::find_last_token_of_type(TokenKind type) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
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

    const Token& TokenStream::at(phi::usize index) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif
        PHI_ASSERT(index < m_Tokens.size());

        return m_Tokens[index.unsafe()];
    }

    phi::usize TokenStream::size() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.size();
    }

    [[nodiscard]] phi::boolean TokenStream::is_empty() const noexcept
    {
        return m_Tokens.empty();
    }

    [[nodiscard]] phi::usize TokenStream::current_position() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Index;
    }

    void TokenStream::set_position(phi::usize index) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Index = index;
    }

    TokenStream::const_iterator TokenStream::begin() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.begin();
    }

    TokenStream::const_iterator TokenStream::cbegin() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.cbegin();
    }

    TokenStream::const_iterator TokenStream::end() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.end();
    }

    TokenStream::const_iterator TokenStream::cend() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.cend();
    }

    [[nodiscard]] TokenStream::const_reverse_iterator TokenStream::rbegin() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.rbegin();
    }

    [[nodiscard]] TokenStream::const_reverse_iterator TokenStream::rend() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.rend();
    }

    [[nodiscard]] const Token& TokenStream::front() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.front();
    }

    [[nodiscard]] const Token& TokenStream::back() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.back();
    }
} // namespace OpenAutoIt
