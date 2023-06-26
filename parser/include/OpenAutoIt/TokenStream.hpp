#pragma once

#include "OpenAutoIt/TokenKind.hpp"
#include "Token.hpp"
#include <vector>

namespace OpenAutoIt
{
class TokenStream
{
public:
    using storage_type           = std::vector<Token>;
    using iterator               = typename storage_type::iterator;
    using const_iterator         = typename storage_type::const_iterator;
    using reverse_iterator       = typename storage_type::reverse_iterator;
    using const_reverse_iterator = typename storage_type::const_reverse_iterator;

    template <typename... ArgsT>
    void emplace_back(ArgsT&&... args)
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finalized);
#endif

        m_Tokens.emplace_back(std::forward<ArgsT>(args)...);
    }

    void push_back(const Token& value);

    void push_back(Token&& value);

    void finalize();

    void reset();

    [[nodiscard]] phi::boolean has_x_more(phi::usize x) const;

    [[nodiscard]] phi::boolean has_more() const;

    [[nodiscard]] phi::boolean reached_end() const;

    [[nodiscard]] const Token& look_ahead() const;

    [[nodiscard]] const Token& look_behind() const;

    void consume();

    void skip(phi::usize n = 1u);

    [[nodiscard]] const Token* find_first_token_of_type(TokenKind type) const;

    [[nodiscard]] const Token* find_last_token_of_type(TokenKind type) const;

    template <typename PredicateT>
    [[nodiscard]] const Token* find_first_token_if(PredicateT pred) const
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finalized);
#endif

        for (const Token& token : m_Tokens)
        {
            if (pred(token))
            {
                return &token;
            }
        }

        return nullptr;
    }

    template <typename PredicateT>
    [[nodiscard]] const Token* find_last_token_if(PredicateT pred) const
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finalized);
#endif

        const Token* last = nullptr;

        for (const Token& token : m_Tokens)
        {
            if (pred(token))
            {
                last = &token;
            }
        }

        return last;
    }

    [[nodiscard]] const Token& at(phi::usize index) const;

    [[nodiscard]] phi::usize size() const;

    [[nodiscard]] phi::boolean is_empty() const;

    // Iterator
    [[nodiscard]] phi::usize current_position();

    void set_position(phi::usize index);

    [[nodiscard]] const_iterator begin() const;

    [[nodiscard]] const_iterator cbegin() const;

    [[nodiscard]] const_iterator end() const;

    [[nodiscard]] const_iterator cend() const;

    [[nodiscard]] const_reverse_iterator rbegin() const;

    [[nodiscard]] const_reverse_iterator rend() const;

    [[nodiscard]] const Token& front() const;

    [[nodiscard]] const Token& back() const;

    void clear()
    {
        m_Tokens.clear();
        m_Index = 0u;
#if defined(PHI_DEBUG)
        m_Finalized = false;
#endif
    }

private:
    storage_type m_Tokens;
    phi::usize   m_Index = 0u;
#if defined(PHI_DEBUG)
    phi::boolean m_Finalized{false};
#endif
};
} // namespace OpenAutoIt
