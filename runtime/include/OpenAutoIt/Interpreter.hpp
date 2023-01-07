#pragma once

#include "OpenAutoIt/AST/ASTBooleanLiteral.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTExpressionStatement.hpp"
#include "OpenAutoIt/AST/ASTIfStatement.hpp"
#include "OpenAutoIt/AST/ASTIntegerLiteral.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTStringLiteral.hpp"
#include "OpenAutoIt/AST/ASTVariableAssignment.hpp"
#include "OpenAutoIt/AST/ASTVariableExpression.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Variant.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <OpenAutoIt/AST/ASTDocument.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/move.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>
#include <iostream>
#include <string>

// TODO: Lots of const correctness issues here

namespace OpenAutoIt
{
    // Simple AST Interpreter
    class Interpreter
    {
    public:
        explicit Interpreter(phi::not_null_observer_ptr<ASTDocument> document) noexcept;

        void SetDocument(phi::not_null_observer_ptr<ASTDocument> document) noexcept;

        void Run();

        [[nodiscard]] VirtualMachine& vm() noexcept;

        [[nodiscard]] const VirtualMachine& vm() const noexcept;

        void InterpretStatements(std::vector<phi::not_null_scope_ptr<ASTStatement>>& statements);

        void InterpretStatement(phi::not_null_observer_ptr<ASTStatement> statement);

        Variant InterpretExpression(phi::not_null_observer_ptr<ASTExpression> expression);

        std::vector<Variant> InterpretExpressions(
                std::vector<phi::not_null_scope_ptr<ASTExpression>>& expressions);

        Variant InterpretBuiltInFunctionCall(const TokenKind             function,
                                             const std::vector<Variant>& arguments);

        Variant InterpretFunctionCall(const phi::string_view      function,
                                      const std::vector<Variant>& arguments);

        Variant EvaluateBinaryExpression(const Variant& lhs, const Variant& rhs,
                                         const TokenKind op);

        Variant EvaluateBinaryPlusExpression(const Variant& lhs, const Variant& rhs) noexcept;
        Variant EvaluateBinaryMinusExpression(const Variant& lhs, const Variant& rhs) noexcept;
        Variant EvaluateBinaryMultiplyExpression(const Variant& lhs, const Variant& rhs) noexcept;

    private:
        phi::not_null_observer_ptr<ASTDocument> m_Document;
        VirtualMachine                          m_VirtualMachine;
    };
} // namespace OpenAutoIt
