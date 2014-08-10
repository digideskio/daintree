#include "arch.h"

uint8_t in8(uint16_t port) {
  uint8_t result;
  asm volatile("inb %1, %0" : "=a" (result) : "dN" (port));
  return result;
}

void out8(uint16_t port, uint8_t data) {
  asm volatile("outb %1, %0" : : "dN" (port), "a" (data));
}

// vim: set sw=2 cc=80 et:
