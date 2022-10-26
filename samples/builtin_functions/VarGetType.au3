Local $bBoolean = False
ConsoleWrite(VarGetType($bBoolean)) ; expect-stdout: "Boolean"

Local $iInt = 1
ConsoleWrite(VarGetType($iInt)) ; expect-stdout: "Int64"

; TODO: Fix me once we implement parsing floating point numbers
;Local $fFloat = 2.0
;ConsoleWrite(VarGetType($fFloat)) ; expect-stdout: "Double"
ConsoleWrite("Double")

Local $sText = "Some text"
ConsoleWrite(VarGetType($sText)) ; expect-stdout: "String"

Local $vKeyword = Default
ConsoleWrite(VarGetType($vKeyword)) ; expect-stdout: "Keyword"
