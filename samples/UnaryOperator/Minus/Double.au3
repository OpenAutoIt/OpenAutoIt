; TODO: AutoIt actually prints "-1" here
ConsoleWrite(-1.0) ; expect-stdout: "-1.000000"
; TODO: AutoIt actually prints "-1.1" here
ConsoleWrite(-1.1) ; expect-stdout: "-1.100000"
; TODO: AutoIt actually prints "-3.14" here
ConsoleWrite(-3.14) ; expect-stdout: "-3.140000"

; TODO: Same here with these 3
ConsoleWrite(--1.0) ; expect-stdout: "1.000000"
ConsoleWrite(--1.1) ; expect-stdout: "1.100000"
ConsoleWrite(--3.14) ; expect-stdout: "3.140000"
