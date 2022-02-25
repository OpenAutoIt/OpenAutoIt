#include "OpenAutoIt/TokenStream.hpp"
#include "phi/core/assert.hpp"

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

        m_Iterator = m_Tokens.begin();
#if defined(PHI_DEBUG)
        m_Finialized = true;
#endif
    }

    void TokenStream::reset() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Iterator = m_Tokens.begin();
    }

    phi::boolean TokenStream::has_x_more(phi::usize x) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        auto it = m_Iterator;
        for (; x > 0u; ++it, --x)
        {
            if (it == m_Tokens.end())
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

        return m_Iterator != m_Tokens.end();
    }

    phi::boolean TokenStream::reached_end() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Iterator == m_Tokens.end();
    }

    const Token& TokenStream::look_ahead() const noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return *m_Iterator;
    }

    const Token& TokenStream::consume() noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return *m_Iterator++;
    }

    void TokenStream::skip(phi::usize n) noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif
        PHI_ASSERT(has_x_more(n));
        PHI_DBG_ASSERT(n != 0u);

        for (; n > 0u; --n, ++m_Iterator)
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
        PHI_DBG_ASSERT(index < m_Tokens.size());

        return m_Tokens.at(index.get());
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

    [[nodiscard]] TokenStream::iterator TokenStream::current_position() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Iterator;
    }

    [[nodiscard]] TokenStream::const_iterator TokenStream::current_position() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Iterator;
    }

    void TokenStream::set_position(TokenStream::iterator it) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Iterator = it;
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
