#pragma once

namespace OpenAutoIt
{
    enum class VariableScope
    {
        Auto,   // Default when not specified. Local inside a function scope otherwise Global
        Global, // Explicitly global
        Local,  // Explicitly local
    };
}
