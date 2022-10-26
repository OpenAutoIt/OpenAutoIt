#pragma once

#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
#define OPENAUTOIT_ENUM_AST_NODE_TYPE()                                                            \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(ArraySubscriptExpression)                                   \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(BinaryExpression)                                           \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(BooleanLiteral)                                             \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(ExpressionStatement)                                        \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(FunctionCallExpression)                                     \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(IfStatement)                                                \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(IntegerLiteral)                                             \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(KeywordLiteral)                                             \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(StringLiteral)                                              \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(UnaryExpression)                                            \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(VariableAssignment)                                         \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(VariableExpression)                                         \
    OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(WhileStatement)

    enum class ASTNodeType
    {
        NONE,

#define OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(name) name,

        OPENAUTOIT_ENUM_AST_NODE_TYPE()

#undef OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL

        // NOTE: Always keep last
        COUNT,
    };

    [[nodiscard]] PHI_ATTRIBUTE_PURE constexpr const char* enum_name(ASTNodeType node_type) noexcept
    {
        switch (node_type)
        {
#define OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL(name)                                                   \
    case ASTNodeType::name:                                                                        \
        return "AST" #name;

            OPENAUTOIT_ENUM_AST_NODE_TYPE()

#undef OPENAUTOIT_ENUM_AST_NODE_TYPE_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    class ASTNode
    {
    public:
        ASTNode() = default;

        virtual ~ASTNode() = default;

        [[nodiscard]] const char* Name() const noexcept
        {
            PHI_ASSERT(m_NodeType != ASTNodeType::NONE);
            PHI_ASSERT(m_NodeType != ASTNodeType::COUNT);

            return enum_name(m_NodeType);
        }

        [[nodiscard]] virtual std::string DumpAST(phi::usize indent = 0u) const noexcept = 0;

        [[nodiscard]] ASTNodeType NodeType() const noexcept
        {
            return m_NodeType;
        }

        template <typename TypeT>
        phi::not_null_observer_ptr<TypeT> as() noexcept
        {
            static_assert(phi::is_base_of_v<ASTNode, TypeT>,
                          "Can only cast to derived classes of ASTNode");

            TypeT* ret = dynamic_cast<TypeT*>(this);
            PHI_ASSERT(ret);

            return ret;
        }

    protected:
        ASTNodeType m_NodeType{ASTNodeType::NONE};
    };
} // namespace OpenAutoIt
