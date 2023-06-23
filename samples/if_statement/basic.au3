; Test If
If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
Endif

If False Then
    ConsoleWrite("FAIL")
EndIf

; Test If/Else
If False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
Else
    ConsoleWrite("FAIL")
EndIf

; Test If/ElseIf
If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
EndIf

; Test If/ElseIf/Else
If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
Else
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
EndIf

; Test If/ElseIf/ElseIf
If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
EndIf

; Test If/ElseIf/ElseIf/Else
If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf True Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
ElseIf False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf True Then
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
Else
    ConsoleWrite("FAIL")
EndIf

If False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
ElseIf False Then
    ConsoleWrite("FAIL")
Else
    ConsoleWrite("PASS") ; expect-stdout: "PASS"
EndIf
