; header
.segment "NES_FILE_HEADER"
.byte "NES", $1A, 2

; code, implement "4+2" operation
.segment "CODE"
main:
  LDA #$4     ; Immediate addressing, put `4` in r_acc
  ADC #$2

; interrupt vector
.segment "INTERRUPT_VECTOR"
  .word main, main, main
