#pragma once

#include "OpenAutoIt/Interpreter.hpp"
#include <phi/core/boolean.hpp>

namespace OpenAutoIt
{
    class REPLInterpreter
    {
    public:
        int Run();

    private:
        void Loop();

        Interpreter  m_Interpreter;
        phi::boolean m_Running{true};
    };
} // namespace OpenAutoIt
