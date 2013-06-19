#include "arch.h"

void out8(uint16_t port, uint8_t data) {
  asm volatile("outb %1, %0" : : "dN" (port), "a" (data));
}

// vim: set sw=2 cc=80 et:
