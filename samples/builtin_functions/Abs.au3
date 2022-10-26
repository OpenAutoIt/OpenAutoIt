; Integers
ConsoleWrite(Abs(3)) ; expect-stdout: "3"
; TODO: Actual tests with negative values once we support parsing them ;-)

; Strings
ConsoleWrite(Abs(""))       ; expect-stdout: "0"
ConsoleWrite(Abs("String")) ; expect-stdout: "0"