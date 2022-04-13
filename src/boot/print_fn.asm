print:
  pusha


; the comparison for string end (null byte)
start:
  mov al, [bx]
  cmp al, 0
  je done

  ; The part where we print with the BIOS help
  mov ah, 0x0e
  int 0x10

  ; increment pointer and do next loop
  add bx, 1
  jmp start

done:
  popa
  ret

print_nl:
  pusha
  mov ah, 0x0e
  mov al, 0x0a ; New line char
  int 0x10
  mov al, 0x0d ; Carriage return
  int 0x10

  popa
  ret
