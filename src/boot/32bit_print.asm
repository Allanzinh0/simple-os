[bits 32] ; using 32-bit protected mode

; this is how constants are defined
VIDEO_MEMORY equ 0xb8000

print_string_pm:
  pusha
  mov edx, VIDEO_MEMORY

print_string_pm_loop:
  mov al, [ebx] ; [ebx] is the address of our character
  mov ah, 0x0A ; Color for printing 

  cmp al, 0 ; Check if end of string
  je print_string_pm_done

  mov [edx], ax ; store character + attribute in video memory
  add ebx, 1 ; next char
  add edx, 2 ; next video memory position

  jmp print_string_pm_loop

print_string_pm_done:
  popa
  ret
