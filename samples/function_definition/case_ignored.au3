func a()
    ConsoleWrite("PASS")
EndFunc

func hello()
    consolewrite("Hello")
EndFunc

; expect-stdout: "PASS"
A()

; expect-stdout: "Hello"
HeLlo()
