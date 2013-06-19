# daintree

.code32
.section .text

.globl entry
entry:
    jmp _entry
    hlt

.code16
.section .multiboot
.align 4
    .long 0x1BADB002
    .long 0b11
    .long -(0x1BADB002 + 0b11)

# vim: set sw=4 cc=80 et:
