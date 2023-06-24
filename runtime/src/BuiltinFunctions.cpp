#include "OpenAutoIt/BuiltinFunctions.hpp"

#include "OpenAutoIt/Variant.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <phi/math/abs.hpp>
#include <ostream>

namespace OpenAutoIt
{

// https://www.autoitscript.com/autoit3/docs/functions/Abs.htm
Variant BuiltIn_Abs(const VirtualMachine& /*vm*/, const Variant& input)
{
    return input.Abs();
}

// https://www.autoitscript.com/autoit3/docs/functions/ConsoleWrite.htm
Variant BuiltIn_ConsoleWrite(VirtualMachine& vm, const Variant& input)
{
    const Variant value = input.CastToString();
    PHI_ASSERT(value.IsString());

    const std::string& output = value.AsString();

    // Output to VM
    vm.Print(output);

    return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
}

// https://www.autoitscript.com/autoit3/docs/functions/ConsoleWriteError.htm
Variant BuiltIn_ConsoleWriteError(VirtualMachine& vm, const Variant& input)
{
    const Variant value = input.CastToString();
    PHI_ASSERT(value.IsString());

    const std::string& output = value.AsString();

    // Output to VM
    vm.PrintError(output);

    return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
}

// https://www.autoitscript.com/autoit3/docs/functions/VarGetType.htm
Variant BuiltIn_VarGetType(const VirtualMachine& /*vm*/, const Variant& input)
{
    return Variant::MakeString(input.GetTypeName());
}

// OpenAutoIt extension
Variant BuiltIn_ConsoleWriteLine(VirtualMachine& vm, const Variant& input)
{
    const Variant value = input.CastToString();
    PHI_ASSERT(value.IsString());

    const std::string& output = value.AsString();

    // Output to VM
    vm.Print(output);

    return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
}

// OpenAutoIt extension
Variant BuiltIn_ConsoleWriteErrorLine(VirtualMachine& vm, const Variant& input)
{
    const Variant value = input.CastToString();
    PHI_ASSERT(value.IsString());

    const std::string& output = value.AsString();

    // Output to VM
    vm.PrintError(output);

    return Variant::MakeInt(static_cast<phi::int64_t>(output.size()));
}

} // namespace OpenAutoIt
