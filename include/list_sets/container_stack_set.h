#ifndef CONTAINER_STACK_H
#define CONTAINER_STACK_H

#include <glib.h>

#include "utils/coreUtils.h"

struct tagset;
struct output;

/*
 * list_set is used by tagsets and tags to hold all containers on them
 * */
struct container_set {
    GPtrArray *tiled_containers;
};

struct container_set *create_container_set();
void destroy_container_set(struct container_set *container_set);

void container_set_write_to_parent(
        struct container_set *parent,
        struct container_set *child);
void container_set_append(
        struct output *m,
        struct container_set *dest,
        struct container_set *src);
void container_set_clear(struct container_set *list_set);

#endif /* CONTAINER_STACK_H */
