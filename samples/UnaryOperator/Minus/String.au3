ConsoleWrite(-"") ; expect-stdout: "0"
ConsoleWrite(-"String") ; expect-stdout: "0"

ConsoleWrite(-"1") ; expect-stdout: "-1"
ConsoleWrite(-"-1") ; expect-stdout: "1"

; TODO: Formatting is wrong here
ConsoleWrite(-"1.1") ; expect-stdout: "-1.100000"
ConsoleWrite(-"-1.1") ; expect-stdout: "1.100000"

ConsoleWrite(-"1Love") ; expect-stdout: "-1"
ConsoleWrite(-"-3Beer") ; expect-stdout: "3"
