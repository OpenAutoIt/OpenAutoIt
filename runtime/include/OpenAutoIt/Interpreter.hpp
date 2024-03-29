#pragma once

#include "OpenAutoIt/AST/ASTBooleanLiteral.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTExpressionStatement.hpp"
#include "OpenAutoIt/AST/ASTIfStatement.hpp"
#include "OpenAutoIt/AST/ASTIntegerLiteral.hpp"
#include "OpenAutoIt/AST/ASTMacroExpression.hpp"
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
    Interpreter() = default;

    void SetDocument(phi::not_null_observer_ptr<ASTDocument> new_document);

    void Run();

    void Step();

    [[nodiscard]] phi::not_null_observer_ptr<ASTStatement> GetCurrentStatement() const;

    [[nodiscard]] VirtualMachine& vm();

    [[nodiscard]] const VirtualMachine& vm() const;

    enum class StatementFinished : bool
    {
        Yes = true,
        No  = false,
    };

    StatementFinished InterpretStatement(phi::not_null_observer_ptr<ASTStatement> statement);

    Variant InterpretExpression(phi::not_null_observer_ptr<ASTExpression> expression);

    std::vector<Variant> InterpretExpressions(
            std::vector<phi::not_null_scope_ptr<ASTExpression>>& expressions);

    Variant InterpretBuiltInFunctionCall(const TokenKind             function,
                                         const std::vector<Variant>& arguments);

    Variant InterpretFunctionCall(const phi::string_view      function,
                                  const std::vector<Variant>& arguments);

    Variant EvaluateMacroExpression(const TokenKind macro);

    Variant EvaluateUnaryExpression(const Variant& value, const TokenKind operator_kind);

    Variant EvaluateBinaryExpression(const Variant& lhs, const Variant& rhs, const TokenKind op);

    Variant EvaluateBinaryPlusExpression(const Variant& lhs, const Variant& rhs);
    Variant EvaluateBinaryMinusExpression(const Variant& lhs, const Variant& rhs);
    Variant EvaluateBinaryMultiplyExpression(const Variant& lhs, const Variant& rhs);
    Variant EvaluateBinaryDivideExpression(const Variant& lhs, const Variant& rhs);

private:
    phi::observer_ptr<ASTDocument> m_Document;
    VirtualMachine                 m_VirtualMachine;
    Statements                     m_VirtualBlock;
};
} // namespace OpenAutoIt
