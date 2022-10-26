#include "OpenAutoIt/BuiltinFunctions.hpp"

#include "OpenAutoIt/Variant.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <phi/math/abs.hpp>

namespace OpenAutoIt
{
    // https://www.autoitscript.com/autoit3/docs/functions/Abs.htm
    Variant BuiltIn_Abs(const VirtualMachine& /*vm*/, const Variant& input) noexcept
    {
        switch (input.GetType())
        {
            case Variant::Type::Int64:
                return Variant::MakeInt(
                        static_cast<phi::int64_t>(phi::abs(input.AsInt64()).unsafe()));

            case Variant::Type::Double:
                return Variant::MakeDouble(phi::abs(input.AsDouble()));

            default:
                return Variant::MakeInt(0);
        }
    }

    // https://www.autoitscript.com/autoit3/docs/functions/ConsoleWrite.htm
    Variant BuiltIn_ConsoleWrite(const VirtualMachine& vm, const Variant& input) noexcept
    {
        const Variant value = input.CastToString();
        PHI_ASSERT(value.IsString());

        const std::string& output = value.AsString();

        // Output to VM
        vm.m_ConsoleWrite(output);

        return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
    }

    // https://www.autoitscript.com/autoit3/docs/functions/ConsoleWriteError.htm
    Variant BuiltIn_ConsoleWriteWrite(const VirtualMachine& vm, const Variant& input) noexcept
    {
        const Variant value = input.CastToString();
        PHI_ASSERT(value.IsString());
        const std::string& output = value.AsString();

        // Output to VM
        vm.m_ConsoleWriteError(output);

        return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
    }

    // https://www.autoitscript.com/autoit3/docs/functions/VarGetType.htm
    Variant BuiltIn_VarGetType(const VirtualMachine& /*vm*/, const Variant& input) noexcept
    {
        return Variant::MakeString(input.GetTypeName());
    }
} // namespace OpenAutoIt
