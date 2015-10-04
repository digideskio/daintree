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

static int mark(void *data) {
    val v = (val) (object *) data;
    if (VAL_IS_OBJECT(v)) {
        VAL_OBJECT(v)->mark = 1;
    }
    return 0;
}

void gc_empty(Context *root) {
    for (struct track_entry *te = tracked; te; te = te->next) {
        te->object->mark = 0;
    }

    dict_foreach(root->env, mark);

    struct track_entry **w = &tracked;
    while (*w) {
        if (!(*w)->object->mark) {
            object_free((*w)->object);
            struct track_entry *next = (*w)->next;
            free(*w);
            *w = next;
        } else {
            w = &(*w)->next;
        }
    }
}

// vim: set sw=4 et:
