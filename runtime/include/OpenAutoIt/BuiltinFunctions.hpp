#pragma once

namespace OpenAutoIt
{
    class VirtualMachine;
    class Variant;

    Variant BuiltIn_Abs(const VirtualMachine& vm, const Variant& input) noexcept;

    Variant BuiltIn_ConsoleWrite(const VirtualMachine& vm, const Variant& input) noexcept;

    Variant BuiltIn_ConsoleWriteWrite(const VirtualMachine& vm, const Variant& input) noexcept;

    Variant BuiltIn_VarGetType(const VirtualMachine& vm, const Variant& input) noexcept;
} // namespace OpenAutoIt
