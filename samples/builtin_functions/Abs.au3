; Integers
ConsoleWrite(Abs(3)) ; expect-stdout: "3"
ConsoleWrite(Abs(-3)) ; expect-stdout: "3"

; Double
; TODO: AutoIt formats doubles differently
ConsoleWrite(Abs(3.14)) ; expect-stdout: "3.140000"
ConsoleWrite(Abs(-3.14)) ; expect-stdout: "3.140000"

; Strings
ConsoleWrite(Abs(""))       ; expect-stdout: "0"
ConsoleWrite(Abs("String")) ; expect-stdout: "0"