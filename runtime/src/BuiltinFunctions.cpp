#include "OpenAutoIt/BuiltinFunctions.hpp"

#include "OpenAutoIt/Variant.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <phi/math/abs.hpp>
#include <ostream>

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
    Variant BuiltIn_ConsoleWrite(VirtualMachine& vm, const Variant& input) noexcept
    {
        const Variant value = input.CastToString();
        PHI_ASSERT(value.IsString());

        const std::string& output = value.AsString();

        // Output to VM
        auto std_out = vm.GetStdout();
        if (std_out != nullptr)
        {
            *std_out << output;
        }

        return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
    }

    // https://www.autoitscript.com/autoit3/docs/functions/ConsoleWriteError.htm
    Variant BuiltIn_ConsoleWriteError(VirtualMachine& vm, const Variant& input) noexcept
    {
        const Variant value = input.CastToString();
        PHI_ASSERT(value.IsString());

        const std::string& output = value.AsString();

        // Output to VM
        auto std_err = vm.GetStderr();
        if (std_err != nullptr)
        {
            *std_err << output;
        }

        return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
    }

    // https://www.autoitscript.com/autoit3/docs/functions/VarGetType.htm
    Variant BuiltIn_VarGetType(const VirtualMachine& /*vm*/, const Variant& input) noexcept
    {
        return Variant::MakeString(input.GetTypeName());
    }

    // OpenAutoIt extension
    Variant BuiltIn_ConsoleWriteLine(VirtualMachine& vm, const Variant& input) noexcept
    {
        const Variant value = input.CastToString();
        PHI_ASSERT(value.IsString());

        const std::string& output = value.AsString();

        // Output to VM
        auto std_out = vm.GetStdout();
        if (std_out != nullptr)
        {
            *std_out << output << 'n';
        }

        return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
    }

    // OpenAutoIt extension
    Variant BuiltIn_ConsoleWriteErrorLine(VirtualMachine& vm, const Variant& input) noexcept
    {
        const Variant value = input.CastToString();
        PHI_ASSERT(value.IsString());

        const std::string& output = value.AsString();

        // Output to VM
        auto std_err = vm.GetStderr();
        if (std_err != nullptr)
        {
            *std_err << output << '\n';
        }

        return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
    }
} // namespace OpenAutoIt
