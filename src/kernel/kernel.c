#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

void main() {
  isr_install();
  irq_install();

  kprint("\n> ");
}

void user_input(char *input) {
  if (strcmp(input, "END") == 0) {
    kprint("Stopping the CPU. Bye\n");
    asm volatile("hlt");
  } else if (strcmp(input, "CLEAR") == 0) {
    clear_screen();
  } else {
    kprint("You said: ");
    kprint(input);
    kprint("\n");
  }

  kprint("> ");
}
