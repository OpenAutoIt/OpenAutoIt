#include "OpenAutoIt/Interpreter.hpp"

#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTFunctionCallExpression.hpp"
#include "OpenAutoIt/AST/ASTKeywordLiteral.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTWhileStatement.hpp"
#include "OpenAutoIt/BuiltinFunctions.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Variant.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/sized_types.hpp>

namespace OpenAutoIt
{
    Interpreter::Interpreter(phi::not_null_observer_ptr<ASTDocument> document) noexcept
        : m_Document{phi::move(document)}
    {}

    void Interpreter::SetDocument(phi::not_null_observer_ptr<ASTDocument> document) noexcept
    {
        m_Document = phi::move(document);
    }

    void Interpreter::Run()
    {
        InterpretStatements(m_Document->m_Statements);
    }

    void Interpreter::InterpretStatements(
            std::vector<phi::not_null_scope_ptr<ASTStatement>>& statements)
    {
        for (const phi::not_null_observer_ptr<ASTStatement> statement : statements)
        {
            // Run pre-action
            m_VirtualMachine.PreStatementAction(statement);

            if (m_VirtualMachine.m_GracefullyHalt)
            {
                return;
            }

            // Run actual statement
            InterpretStatement(statement);

            // Run post-action
            m_VirtualMachine.PostStatementAction(statement);
        }
    }

    void Interpreter::InterpretStatement(phi::not_null_observer_ptr<ASTStatement> statement)
    {
        switch (statement->NodeType())
        {
            case ASTNodeType::ExpressionStatement: {
                auto expression_statement = statement->as<ASTExpressionStatement>();

                InterpretExpression(expression_statement->m_Expression);
                return;
            }

            case ASTNodeType::IfStatement: {
                auto if_statement = statement->as<ASTIfStatement>();

                const Variant if_condition_value =
                        InterpretExpression(if_statement->m_IfCase.condition).CastToBoolean();
                PHI_ASSERT(if_condition_value.IsBoolean());

                if (if_condition_value.AsBoolean())
                {
                    InterpretStatements(if_statement->m_IfCase.body);
                    return;
                }

                // Handle all ElseIf cases
                for (auto&& else_if_case : if_statement->m_ElseIfCases)
                {
                    const Variant condition_value =
                            InterpretExpression(else_if_case.condition).CastToBoolean();
                    PHI_ASSERT(condition_value.IsBoolean());

                    if (condition_value.AsBoolean())
                    {
                        InterpretStatements(else_if_case.body);
                        return;
                    }
                }

                // Handle Else case
                InterpretStatements(if_statement->m_ElseCase);
                return;
            }

            case ASTNodeType::VariableAssignment: {
                auto variable_assignment = statement->as<ASTVariableAssignment>();

                const phi::string_view variable_name = variable_assignment->m_VariableName;
                PHI_ASSERT(!variable_name.is_empty());

                phi::observer_ptr<ASTExpression> initial_expression =
                        variable_assignment->m_InitialValueExpression;
                if (initial_expression)
                {
                    const Variant expression_value =
                            InterpretExpression(initial_expression.release_not_null());

                    // TODO: Wrap in function
                    m_VirtualMachine.m_Globals.insert_or_assign(variable_name, expression_value);
                    return;
                }

                // TODO: Wrap in function
                // Insert a default initialized variable
                m_VirtualMachine.m_Globals[variable_name];
                return;
            }

            case ASTNodeType::WhileStatement: {
                auto while_statement = statement->as<ASTWhileStatement>();

                while (true)
                {
                    // TODO: This is actually not totally correct
                    m_VirtualMachine.PreStatementAction(statement);

                    // Evalaute condition
                    const Variant condition =
                            InterpretExpression(while_statement->m_ConditionExpression)
                                    .CastToBoolean();
                    PHI_ASSERT(condition.IsBoolean());

                    // Stop while loop if the condition is false
                    if (!condition.AsBoolean() || m_VirtualMachine.m_GracefullyHalt)
                    {
                        return;
                    }

                    // Interpret while statements
                    InterpretStatements(while_statement->m_Statements);
                }
            }

            default:
                PHI_ASSERT_NOT_REACHED();
                return;
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

                return EvaluateBinaryExpression(lhs_value, rhs_value,
                                                binary_expression->m_Operator);
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

            case ASTNodeType::StringLiteral: {
                auto string_literal = expression->as<ASTStringLiteral>();

                return Variant::MakeString(string_literal->m_Value);
            }

            case ASTNodeType::UnaryExpression:
                // TODO: UnaryExpression
                return {};

            case ASTNodeType::VariableExpression: {
                auto variable_expression = expression->as<ASTVariableExpression>();

                const phi::string_view variable_name = variable_expression->m_VariableName;

                // TODO: Wrap in function
                if (!m_VirtualMachine.m_Globals.contains(variable_name))
                {
                    // TODO: Give error
                    return {};
                }

                // TODO: Wrap in function
                return m_VirtualMachine.m_Globals.at(variable_name);
            }

            default:
                PHI_ASSERT_NOT_REACHED();
        }

        PHI_ASSERT_NOT_REACHED();
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

                return BuiltIn_ConsoleWriteWrite(m_VirtualMachine, arguments.at(0u));
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

            default:
                // TODO: Temporary hack to allow running code which uses not implemented builtins
                std::cout << "RUNTIME ERROR: Usage of unimplemented builtin function \""
                          << enum_name(function) << "\"\n";
                return {};
        }

        PHI_ASSERT_NOT_REACHED();
    }

    Variant Interpreter::InterpretFunctionCall(const phi::string_view      function,
                                               const std::vector<Variant>& arguments)
    {
        // TODO: Implement men

        (void)function;
        (void)arguments;

        return {};
    }

    Variant Interpreter::EvaluateBinaryExpression(const Variant& lhs, const Variant& rhs,
                                                  TokenKind op)
    {
        // TODO: Lots of operators missing here
        switch (op)
        {
            case TokenKind::OP_Plus:
                return EvalutateBinaryPlusExpression(lhs, rhs);

            default:
                return {};
        }
    }

    Variant Interpreter::EvalutateBinaryPlusExpression(const Variant& lhs,
                                                       const Variant& rhs) noexcept
    {
        // TODO: We currently only support adding integer which is not correct
        if (!lhs.IsInt64() || !rhs.IsInt64())
        {
            return {};
        }

        return Variant::MakeInt(lhs.AsInt64() + rhs.AsInt64());
    }
} // namespace OpenAutoIt
