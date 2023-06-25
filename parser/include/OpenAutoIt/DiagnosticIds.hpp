#pragma once

#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

namespace OpenAutoIt
{

// OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage)
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()                                                           \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(None, "", "")                                              \
    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(NullCharacter, "-Wnull-character", "null character ignored")

enum class DiagnosticId
{
#define OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL(EnumName, FlagName, DiagnosticMessage) EnumName,

    OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()

#undef OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL
};

PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wcovered-switch-default")

[[nodiscard]] constexpr const char* DiagnosticIdFlagName(const DiagnosticId id)
{
    PHI_ASSERT(id != DiagnosticId::None);

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
    case DiagnosticId::EnumName:                                                                   \
        return DiagnosticMessage;

        OPENAUTOIT_ENUM_DIAGNOSTIC_IDS()

#undef OPENAUTOIT_ENUM_DIAGNOSTIC_IDS_IMPL

        default:
            PHI_ASSERT_NOT_REACHED();
    }
}

PHI_CLANG_SUPPRESS_WARNING_POP()

} // namespace OpenAutoIt
