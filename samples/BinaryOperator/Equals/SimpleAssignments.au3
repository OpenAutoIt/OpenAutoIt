Local $var

$var = 123
ConsoleWrite($var) ; expect-stdout: "123"

$var = "String"
ConsoleWrite($var) ; expect-stdout: "String"
