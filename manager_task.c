// daintree

#include <console.h>
#include <mem.h>
#include <task.h>

void manager_task(void) {
    while (1) {
        uint8_t vx = 60, vy = 0;
        puts_at(&vx, &vy, "+-----------------+");

        int i = 0;
        for (struct task_list *tl = tasks; tl; tl = tl->next) {
            vx = 60; vy = i + 1;

            puts_at(&vx, &vy, "| ");
            puts_at(&vx, &vy, tl->task->name);

            if (tl->task->waiting_irq) {
                putf_at(&vx, &vy, " W%d H%d", tl->task->waiting_irq_no, tl->task->waiting_irq_hits);
            }
            
            while (vx < 78) {
                putc_at(&vx, &vy, ' ');
            }
            putc_at(&vx, &vy, '|');
            ++i;
        }

        vx = 60; vy = i + 1;
        putf_at(&vx, &vy, "| heap: %x (%d%%)", heap_in_use, heap_in_use * 100 / heap_size);
        while (vx < 78) {
            putc_at(&vx, &vy, ' ');
        }
        putc_at(&vx, &vy, '|');
        vx = 60; vy = i + 2;
        puts_at(&vx, &vy, "+-----------------+");

        __asm__ __volatile__("int $0x80" : : "a" (5), "b" (100));
    }
}

// vim: set sw=4 et:
