// daintree

#include <gc.h>
#include <mem.h>

// The Worst Garbage Collector.

struct track_entry {
    object *object;
    struct track_entry *next;
};

struct track_entry *tracked;

object *gc_track(object *object) {
    struct track_entry *te = malloc(sizeof(*te));
    te->object = object;
    te->next = tracked;
    tracked = te;
    return object;
}

void gc_empty(void) {
    struct track_entry *te = tracked;
    while (te) {
        object_free(te->object);
        struct track_entry *next = te->next;
        free(te);
        te = next;
    }
    tracked = NULL;
}

// vim: set sw=4 et:
