#include "OpenAutoIt/Interpreter.hpp"

#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTExitStatement.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTFloatLiteral.hpp"
#include "OpenAutoIt/AST/ASTFunctionCallExpression.hpp"
#include "OpenAutoIt/AST/ASTFunctionDefinition.hpp"
#include "OpenAutoIt/AST/ASTKeywordLiteral.hpp"
#include "OpenAutoIt/AST/ASTMacroExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTTernaryIfExpression.hpp"
#include "OpenAutoIt/AST/ASTUnaryExpression.hpp"
#include "OpenAutoIt/AST/ASTWhileStatement.hpp"
#include "OpenAutoIt/BuiltinFunctions.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/UnsafeOperations.hpp"
#include "OpenAutoIt/Variant.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <phi/core/unsafe_cast.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/format.h>

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_MSVC_SUPPRESS_WARNING(4702) // unreachable code

namespace OpenAutoIt
{
void Interpreter::SetDocument(phi::not_null_observer_ptr<ASTDocument> new_document)
{
    m_Document = new_document;
    vm().PushGlobalScope(m_Document->m_Statements);
}

void Interpreter::Run()
{
    while (vm().CanRun())
    {
        Step();
    }
}

void Interpreter::Step()
{
    Scope& current_scope = vm().GetCurrentScope();

    // Check if we reached the end of the current scope
    if (current_scope.index >= current_scope.statements.size())
    {
        vm().PopScope();
        return;
    }

    const auto current_statement = GetCurrentStatement();

    // Interpret statement
    const StatementFinished result = InterpretStatement(current_statement);

    // Increment index if the statement is finished and we can still run
    if (result == StatementFinished::Yes && vm().CanRun())
    {
        ++current_scope.index;
    }
}

phi::not_null_observer_ptr<ASTStatement> Interpreter::GetCurrentStatement() const
{
    const Scope& current_scope = vm().GetCurrentScope();
    PHI_ASSERT(!current_scope.statements.empty());
    PHI_ASSERT(current_scope.index < current_scope.statements.size());

    return current_scope.statements.at(current_scope.index.unsafe());
}

PHI_ATTRIBUTE_CONST VirtualMachine& Interpreter::vm()
{
    return m_VirtualMachine;
}

PHI_ATTRIBUTE_CONST const VirtualMachine& Interpreter::vm() const
{
    return m_VirtualMachine;
}

Interpreter::StatementFinished Interpreter::InterpretStatement(
        phi::not_null_observer_ptr<ASTStatement> statement)
{
    // NOTE: Generally we return Yes for finished statements and the ending of loops
    //       While returning No for unfinished loops like While and For

    switch (statement->NodeType())
    {
        case ASTNodeType::ExpressionStatement: {
            auto expression_statement = statement->as<ASTExpressionStatement>();

            InterpretExpression(expression_statement->m_Expression);
            return StatementFinished::Yes;
        }

        case ASTNodeType::IfStatement: {
            auto if_statement = statement->as<ASTIfStatement>();

            const Variant if_condition_value =
                    InterpretExpression(if_statement->m_IfCase.condition).CastToBoolean();
            PHI_ASSERT(if_condition_value.IsBoolean());

            if (if_condition_value.AsBoolean())
            {
                vm().PushBlockScope(if_statement->m_IfCase.body);
                return StatementFinished::Yes;
            }

            // Handle all ElseIf cases
            for (auto&& else_if_case : if_statement->m_ElseIfCases)
            {
                const Variant condition_value =
                        InterpretExpression(else_if_case.condition).CastToBoolean();
                PHI_ASSERT(condition_value.IsBoolean());

                if (condition_value.AsBoolean())
                {
                    vm().PushBlockScope(else_if_case.body);
                    return StatementFinished::Yes;
                }
            }

            // Handle Else case
            vm().PushBlockScope(if_statement->m_ElseCase);
            return StatementFinished::Yes;
        }

        case ASTNodeType::VariableAssignment: {
            auto variable_assignment = statement->as<ASTVariableAssignment>();

            const phi::string_view variable_name = variable_assignment->m_VariableName;
            PHI_ASSERT(!variable_name.is_empty());

            // TODO: Const?
            phi::observer_ptr<ASTExpression> initial_expression =
                    variable_assignment->m_InitialValueExpression;
            if (initial_expression)
            {
                const Variant expression_value =
                        InterpretExpression(initial_expression.release_not_null());

                vm().PushOrAssignVariable(variable_name, expression_value);
                return StatementFinished::Yes;
            }

            // Insert a default initialized variable
            vm().PushVariable(variable_name, {});
            return StatementFinished::Yes;
        }

        case ASTNodeType::WhileStatement: {
            auto while_statement = statement->as<ASTWhileStatement>();

            // Evaluate condition
            const Variant condition =
                    InterpretExpression(while_statement->m_ConditionExpression).CastToBoolean();
            PHI_ASSERT(condition.IsBoolean());

            if (!condition.AsBoolean())
            {
                return StatementFinished::Yes;
            }

            // Interpret while statements
            vm().PushBlockScope(while_statement->m_Statements);
            return StatementFinished::No;
        }

        case ASTNodeType::ExitStatement: {
            auto exit_statement = statement->as<ASTExitStatement>();

            if (exit_statement->m_Expression)
            {
                const Variant exit_code =
                        InterpretExpression(exit_statement->m_Expression.not_null_observer())
                                .CastToInt64();

                if (exit_code.IsInt64())
                {
                    vm().Exit(phi::unsafe_cast<phi::u32>(exit_code.AsInt64()));
                    return StatementFinished::Yes;
                }
            }

            vm().Exit(0u);
            return StatementFinished::Yes;
        }

        default:
            PHI_ASSERT_NOT_REACHED();
            return StatementFinished::No;
    }
}

Variant Interpreter::InterpretExpression(phi::not_null_observer_ptr<ASTExpression> expression)
{
    switch (expression->NodeType())
    {
        case ASTNodeType::ArraySubscriptExpression:
            // TODO: ArraySubscriptExpression
            return {};

        case ASTNodeType::BinaryExpression: {
            auto binary_expression = expression->as<ASTBinaryExpression>();

            const Variant lhs_value = InterpretExpression(binary_expression->m_LHS);
            const Variant rhs_value = InterpretExpression(binary_expression->m_RHS);

            return EvaluateBinaryExpression(lhs_value, rhs_value, binary_expression->m_Operator);
        }

        case ASTNodeType::BooleanLiteral: {
            auto boolean_literal = expression->as<ASTBooleanLiteral>();

            return Variant::MakeBoolean(boolean_literal->m_Value);
        }

        case ASTNodeType::FunctionCallExpression: {
            // TODO: What happens when you assign variable to the return of a void function?
            auto function_call_expression = expression->as<ASTFunctionCallExpression>();

            // Evaluate all arguments
            const std::vector<Variant> arguments =
                    InterpretExpressions(function_call_expression->m_Arguments);

            // Handle builtin functions seperately
            if (function_call_expression->m_IsBuiltIn)
            {
                return InterpretBuiltInFunctionCall(function_call_expression->m_BuiltInFunction,
                                                    arguments);
            }

            return InterpretFunctionCall(function_call_expression->m_FunctionName, arguments);
        }

        case ASTNodeType::IntegerLiteral: {
            auto integer_literal = expression->as<ASTIntegerLiteral>();

            return Variant::MakeInt(integer_literal->m_Value);
        }

        case ASTNodeType::KeywordLiteral: {
            auto keyword_literal = expression->as<ASTKeywordLiteral>();

            return Variant::MakeKeyword(keyword_literal->m_Keyword);
        }

        case ASTNodeType::FloatLiteral: {
            auto float_literal = expression->as<ASTFloatLiteral>();

            return Variant::MakeDouble(float_literal->m_Value);
        }

        case ASTNodeType::StringLiteral: {
            auto string_literal = expression->as<ASTStringLiteral>();

            return Variant::MakeString(string_literal->m_Value);
        }

        case ASTNodeType::TernaryIfExpression: {
            auto ternary_expression = expression->as<ASTTernaryIfExpression>();

            const Variant condition_value =
                    InterpretExpression(ternary_expression->m_ConditionExpression);

            if (condition_value.CastToBoolean().AsBoolean())
            {
                return InterpretExpression(ternary_expression->m_TrueExpression);
            }

            return InterpretExpression(ternary_expression->m_FalseExpression);
        }

        case ASTNodeType::MacroExpression: {
            auto macro_expression = expression->as<ASTMacroExpression>();

            const TokenKind macro = macro_expression->m_Macro;

            return EvaluateMacroExpression(macro);
        }

        case ASTNodeType::UnaryExpression: {
            // TODO: add const
            auto unary_expression = expression->as<ASTUnaryExpression>();

            Variant expression_value = InterpretExpression(unary_expression->m_Expression);

            return EvaluateUnaryExpression(expression_value, unary_expression->m_Operator);
        }

        case ASTNodeType::VariableExpression: {
            const auto variable_expression = expression->as<ASTVariableExpression>();

            const phi::string_view variable_name = variable_expression->m_VariableName;

            auto value = vm().LookupVariableByName(variable_name);
            if (!value)
            {
                vm().RuntimeError("No variable named '{}'", std::string_view(variable_name));
                return {};
            }

            return value.value();
        }

        default:
            PHI_ASSERT_NOT_REACHED();
    }

    PHI_ASSERT_NOT_REACHED();
    return {};
}

std::vector<Variant> Interpreter::InterpretExpressions(
        std::vector<phi::not_null_scope_ptr<ASTExpression>>& expressions)
{
    std::vector<Variant> ret;
    ret.reserve(expressions.size());

    for (auto& expression : expressions)
    {
        ret.emplace_back(InterpretExpression(expression));
    }

    return ret;
}

Variant Interpreter::InterpretBuiltInFunctionCall(const TokenKind             function,
                                                  const std::vector<Variant>& arguments)
{
    // TODO: Is pretty incovinient that every function has to check for itself that it hast the right amount of arguments etc.

    switch (function)
    {
        // https://www.autoitscript.com/autoit3/docs/functions/Abs.htm
        case TokenKind::BI_Abs: {
            if (arguments.size() != 1u)
            {
                // TODO: Error
                return {};
            }

            return BuiltIn_Abs(m_VirtualMachine, arguments.at(0u));
        }

        // https://www.autoitscript.com/autoit3/docs/functions/ConsoleWrite.htm
        case TokenKind::BI_ConsoleWrite: {
            if (arguments.size() != 1u)
            {
                // TODO: Error
                return {};
            }

            return BuiltIn_ConsoleWrite(m_VirtualMachine, arguments.at(0u));
        }

        // https://www.autoitscript.com/autoit3/docs/functions/ConsoleWriteError.htm
        case TokenKind::BI_ConsoleWriteError: {
            if (arguments.size() != 1u)
            {
                // TODO: Error
                return {};
            }

            return BuiltIn_ConsoleWriteError(m_VirtualMachine, arguments.at(0u));
        }

        // https://www.autoitscript.com/autoit3/docs/functions/VarGetType.htm
        case TokenKind::BI_VarGetType: {
            if (arguments.size() != 1u)
            {
                // TODO: Error
                return {};
            }

            return BuiltIn_VarGetType(m_VirtualMachine, arguments.at(0u));
        }

        case TokenKind::BI_ConsoleWriteLine: {
            if (arguments.size() != 1u)
            {
                // TODO: Error:
                return {};
            }

            return BuiltIn_ConsoleWriteLine(vm(), arguments.at(0u));
        }

        case TokenKind::BI_ConsoleWriteErrorLine: {
            if (arguments.size() != 1u)
            {
                // TODO: Error
                return {};
            }

            return BuiltIn_ConsoleWriteErrorLine(vm(), arguments.at(0u));
        }

        default:
            vm().RuntimeError("Builtin function '{:s}' not implemented", enum_name(function));
            return {};
    }

    PHI_ASSERT_NOT_REACHED();
    return {};
}

Variant Interpreter::InterpretFunctionCall(const phi::string_view      function,
                                           const std::vector<Variant>& arguments)
{
    phi::observer_ptr<ASTFunctionDefinition> function_definition =
            m_Document->LookupFunctionDefinitionByName(function);

    if (!function_definition)
    {
        vm().RuntimeError("Function '{:s}' not found'", std::string_view(function));
        return {};
    }

    // Push new function scope
    vm().PushFunctionScope(function, function_definition->m_FunctionBody);

    // Push arguments into the new scope
    for (phi::usize index{0u}; index < function_definition->m_Parameters.size(); ++index)
    {
        // TODO: This should be const but theres currently a bug in Phi which prevents us more doing so
        FunctionParameter& parameter = function_definition->m_Parameters.at(index.unsafe());

        // Check if the argument was explicitly provided
        if (index < arguments.size())
        {
            // Simply set the parameter to be the given argument
            vm().PushVariable(parameter.name, arguments.at(index.unsafe()));
        }
        else
        {
            // Otherwise the parameter MUST be defaultet
            if (parameter.default_value_init.empty())
            {
                // TODO: Better error message
                vm().RuntimeError("Missing argument");
                break;
            }

            // Push the parameter with an empty value
            vm().PushVariable(parameter.name, {});

            // Push a virtual block scope which handles the initialization of the default value
            // We do this since function default values can themselves be function calls etc.
            vm().PushBlockScope(parameter.default_value_init);
        }
    }

    return {};
}

Variant Interpreter::EvaluateUnaryExpression(const Variant& value, const TokenKind operator_kind)
{
    switch (operator_kind)
    {
        case TokenKind::OP_Plus:
            return value;

        case TokenKind::OP_Minus:
            return value.UnaryMinus();

        case TokenKind::KW_Not:
            return value.UnaryNot();

        default:
            PHI_ASSERT_NOT_REACHED();
    }

    PHI_ASSERT_NOT_REACHED();
}

Variant Interpreter::EvaluateMacroExpression(const TokenKind macro)
{
    PHI_ASSERT(static_cast<phi::size_t>(macro) >= MacroFirst &&
               static_cast<phi::size_t>(macro) <= MacroLast);

    switch (macro)
    {
        case TokenKind::MK_CR:
            return Variant::MakeString("\r");
        case TokenKind::MK_CRLF:
            return Variant::MakeString("\r\n");
        case TokenKind::MK_LF:
            return Variant::MakeString("\n");

        default:
            vm().RuntimeError("Unimplemented macro '{:s}'", enum_name(macro));
            return {};
    }

    PHI_ASSERT_NOT_REACHED();
}

Variant Interpreter::EvaluateBinaryExpression(const Variant& lhs, const Variant& rhs, TokenKind op)
{
    // TODO: Lots of operators missing here
    switch (op)
    {
        case TokenKind::OP_Plus:
            return EvaluateBinaryPlusExpression(lhs, rhs);

        case TokenKind::OP_Minus:
            return EvaluateBinaryMinusExpression(lhs, rhs);

        case TokenKind::OP_Multiply:
            return EvaluateBinaryMultiplyExpression(lhs, rhs);

        case TokenKind::OP_Divide:
            return EvaluateBinaryDivideExpression(lhs, rhs);

        case TokenKind::OP_Concatenate:
            return lhs.Concatenate(rhs);

        default:
            return {};
    }
}

Variant Interpreter::EvaluateBinaryPlusExpression(const Variant& lhs, const Variant& rhs)
{
    // TODO: We currently only support adding integer which is not correct
    if (!lhs.IsInt64() || !rhs.IsInt64())
    {
        return {};
    }

    return Variant::MakeInt(UnsafeAdd(lhs.AsInt64(), rhs.AsInt64()));
}

Variant Interpreter::EvaluateBinaryMinusExpression(const Variant& lhs, const Variant& rhs)
{
    if (!lhs.IsInt64() || !rhs.IsInt64())
    {
        return {};
    }

    return Variant::MakeInt(UnsafeMinus(lhs.AsInt64(), rhs.AsInt64()));
}

Variant Interpreter::EvaluateBinaryMultiplyExpression(const Variant& lhs, const Variant& rhs)
{
    if (!lhs.IsInt64() || !rhs.IsInt64())
    {
        return {};
    }

    return Variant::MakeInt(UnsafeMultiply(lhs.AsInt64(), rhs.AsInt64()));
}

Variant Interpreter::EvaluateBinaryDivideExpression(const Variant& lhs, const Variant& rhs)
{
    if (!lhs.IsInt64() || !rhs.IsInt64())
    {
        return {};
    }

    // Return infinity when dividing by 0
    if (rhs.AsInt64() == 0)
    {
        if (lhs.AsInt64() == 0)
        {
            return Variant::MakeDouble(phi::f64::limits_type::quiet_NaN());
        }
        if (lhs.AsInt64() < 0)
        {
            return Variant::MakeDouble(-phi::f64::limits_type::infinity());
        }
        if (lhs.AsInt64() > 0)
        {
            return Variant::MakeDouble(phi::f64::limits_type::infinity());
        }

        PHI_ASSERT_NOT_REACHED();
    }

    return Variant::MakeInt(UnsafeDivide(lhs.AsInt64(), rhs.AsInt64()));
}

} // namespace OpenAutoIt
