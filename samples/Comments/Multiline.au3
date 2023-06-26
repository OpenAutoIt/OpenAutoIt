#comments-start
ConsoleWrite("FAIL")
#comments-end

#cs
ConsoleWrite("FAIL")
#ce

ConsoleWrite("PASS") ; expect-stdout: "PASS"
