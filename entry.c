// daintree

#include "arch.h"
#include "cons.h"
#include "multiboot.h"

void entry(multiboot_info_t *multiboot) {
  if (multiboot == 0) {
    puts("no multiboot info. halting.");
    return;
  }

  clear();
  puts(
      "here we are\n2 hi\n3 ok\n4he\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n"
      "16\n17\n18\n19\n20\n21\n22\n23\n24\n25");

  asm volatile("hlt");
}

// vim: set sw=2 cc=80 et:
