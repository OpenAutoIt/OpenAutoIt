; Boolean
ConsoleWrite(Abs(False)) ; expect-stdout: "0"
ConsoleWrite(Abs(True))  ; expect-stdout: "1"

; Integers
ConsoleWrite(Abs(3))  ; expect-stdout: "3"
ConsoleWrite(Abs(-3)) ; expect-stdout: "3"

; Double
; TODO: AutoIt formats doubles differently
ConsoleWrite(Abs(3.14)) ; expect-stdout: "3.140000"
ConsoleWrite(Abs(-3.14)) ; expect-stdout: "3.140000"

; Strings
ConsoleWrite(Abs(""))       ; expect-stdout: "0"
ConsoleWrite(Abs("String")) ; expect-stdout: "0"

ConsoleWrite(Abs("0"))      ; expect-stdout: "0"
ConsoleWrite(Abs("1"))      ; expect-stdout: "1"
ConsoleWrite(Abs("2"))      ; expect-stdout: "2"
ConsoleWrite(Abs("314"))    ; expect-stdout: "314"
ConsoleWrite(Abs("-1"))     ; expect-stdout: "1"
ConsoleWrite(Abs("-2"))     ; expect-stdout: "2"
ConsoleWrite(Abs("-314"))   ; expect-stdout: "314"

ConsoleWrite(Abs("1Love"))  ; expect-stdout: "1"

; TODO: Same formatting problem here
ConsoleWrite(Abs("3.14"))   ; expect-stdout: "3.140000"
ConsoleWrite(Abs("-3.14"))   ; expect-stdout: "3.140000"

ConsoleWrite(Abs("3.14Hey"))   ; expect-stdout: "3.140000"

; TODO: Test behavior of different types like arrays, etc.
