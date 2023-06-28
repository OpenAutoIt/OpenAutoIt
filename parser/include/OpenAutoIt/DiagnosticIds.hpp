#pragma once

#include <phi/algorithm/string_length.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

namespace OpenAutoIt
{

// OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage)
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()                                                           \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(None, "", "")                                              \
    /* Lexer warnings */                                                                           \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(NullCharacter, "-Wnull-character",                         \
                                        "null character ignored")                                  \
    /* Parser error */                                                                             \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(UnexpectedEndOfFile, "",                                   \
                                        "unexpected end of file while parsing {:s}")               \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(Expected, "", "expected {:s}")                             \
    /* Parser fatal error */                                                                       \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(FileNotFound, "", "'{:s}' file not found")                 \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EmptyFilename, "", "empty filename")                       \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(IncludeNestingTooDeeply, "", "#include nested too deeply") \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(FileNameTooLong, "",                                       \
                                        "cannot open file '{:s}': File name too long")

enum class DiagnosticId
{
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage) EnumName,

    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()

#undef OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL
};

PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wcovered-switch-default")
PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wreturn-type")

[[nodiscard]] constexpr phi::boolean DiagnosticIdHasFlagName(const DiagnosticId id)
{
    PHI_ASSERT(id != DiagnosticId::None);

    switch (id)
    {
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage)                 \
    case DiagnosticId::EnumName:                                                                   \
        return phi::string_length(FlagName) != 0u;

        OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()

#undef OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL

        default:
            PHI_ASSERT_NOT_REACHED();
    }
}

[[nodiscard]] constexpr const char* DiagnosticIdFlagName(const DiagnosticId id)
{
    PHI_ASSERT(id != DiagnosticId::None);
    PHI_ASSERT(DiagnosticIdHasFlagName(id));

    switch (id)
    {
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage)                 \
    case DiagnosticId::EnumName:                                                                   \
        return FlagName;

        OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()

#undef OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL

        default:
            PHI_ASSERT_NOT_REACHED();
    }
}

[[nodiscard]] constexpr const char* DiagnosticIdMessage(const DiagnosticId id)
{
    PHI_ASSERT(id != DiagnosticId::None);

    switch (id)
    {
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage)                 \
    case DiagnosticId::EnumName: {                                                                 \
        PHI_ASSERT(phi::string_length(DiagnosticMessage) != 0u);                                   \
        return DiagnosticMessage;                                                                  \
    }

        OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()

#undef OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL

        default:
            PHI_ASSERT_NOT_REACHED();
    }
}

PHI_GCC_SUPPRESS_WARNING_POP()
PHI_CLANG_SUPPRESS_WARNING_POP()

} // namespace OpenAutoIt
