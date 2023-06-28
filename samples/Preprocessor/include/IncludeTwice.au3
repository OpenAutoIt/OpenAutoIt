#include "TestIncludeSimple.au3" ; expect-stdout: "1"
#include <TestIncludeSimple.au3> ; expect-stdout: "1"

ConsoleWrite("PASS") ; expect-stdout: "PASS"
