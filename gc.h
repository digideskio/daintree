// daintree

#ifndef __GC_H__
#define __GC_H__

#include <program.h>

object *gc_track(object *object);
void gc_empty(Context *root);

#endif

// vim: set sw=4 et:
