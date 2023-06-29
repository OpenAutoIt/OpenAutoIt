ConsoleWrite(1 / 0) ; expect-stdout: "inf"
ConsoleWrite(-1 / 0) ; expect-stdout: "-inf"
ConsoleWrite(0 / 0) ; expect-stdout: "nan"
