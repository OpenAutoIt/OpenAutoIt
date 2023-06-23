#pragma once

namespace OpenAutoIt
{
class VirtualMachine;
class Variant;

// https://www.autoitscript.com/autoit3/docs/functions/

Variant BuiltIn_Abs(const VirtualMachine& vm, const Variant& input);

Variant BuiltIn_ConsoleWrite(VirtualMachine& vm, const Variant& input);

Variant BuiltIn_ConsoleWriteError(VirtualMachine& vm, const Variant& input);

Variant BuiltIn_VarGetType(const VirtualMachine& vm, const Variant& input);

// OpenAutoIt Extensions

Variant BuiltIn_ConsoleWriteLine(VirtualMachine& vm, const Variant& input);

Variant BuiltIn_ConsoleWriteErrorLine(VirtualMachine& vm, const Variant& input);
} // namespace OpenAutoIt
