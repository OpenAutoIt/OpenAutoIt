#include "IncludeOnceAtTheEnd.au3" ; expect-stdout: "1"
#include <IncludeOnceAtTheEnd.au3>

ConsoleWrite("PASS") ; expect-stdout: "PASS"
