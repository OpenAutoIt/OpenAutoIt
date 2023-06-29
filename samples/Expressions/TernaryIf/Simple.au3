ConsoleWrite(True ? "PASS" : "FAIL") ; expect-stdout: "PASS"
ConsoleWrite(False ? "FAIL" : "PASS") ; expect-stdout: "PASS"
