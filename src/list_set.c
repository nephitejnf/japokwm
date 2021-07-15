#include "list_set.h"

#include <assert.h>

#include "ipc-server.h"
#include "server.h"
#include "utils/coreUtils.h"

struct list_set *create_list_set()
{
    struct list_set *list_set = calloc(1, sizeof(struct list_set));
    wlr_list_init(&list_set->change_affected_list_sets);
    wlr_list_push(&list_set->change_affected_list_sets, list_set);

    wlr_list_init(&list_set->container_lists);
    wlr_list_init(&list_set->visible_container_lists);

    wlr_list_init(&list_set->independent_containers);
    wlr_list_init(&list_set->tiled_containers);
    wlr_list_init(&list_set->hidden_containers);
    wlr_list_init(&list_set->floating_containers);

    wlr_list_push(&list_set->container_lists, &list_set->tiled_containers);
    wlr_list_push(&list_set->container_lists, &list_set->floating_containers);
    wlr_list_push(&list_set->container_lists, &list_set->hidden_containers);

    wlr_list_push(&list_set->visible_container_lists, &list_set->tiled_containers);
    wlr_list_push(&list_set->visible_container_lists, &list_set->floating_containers);

    wlr_list_init(&list_set->focus_stack_lists);
    wlr_list_init(&list_set->focus_stack_visible_lists);
    wlr_list_init(&list_set->focus_stack_lists_with_layer_shell);

    wlr_list_init(&list_set->focus_stack_layer_background);
    wlr_list_init(&list_set->focus_stack_layer_bottom);
    wlr_list_init(&list_set->focus_stack_layer_top);
    wlr_list_init(&list_set->focus_stack_layer_overlay);
    wlr_list_init(&list_set->focus_stack_layer_bottom);
    wlr_list_init(&list_set->focus_stack_on_top);
    wlr_list_init(&list_set->focus_stack_normal);
    wlr_list_init(&list_set->focus_stack_hidden);
    wlr_list_init(&list_set->focus_stack_not_focusable);

    wlr_list_push(&list_set->focus_stack_lists, &list_set->focus_stack_layer_top);
    wlr_list_push(&list_set->focus_stack_lists, &list_set->focus_stack_on_top);
    wlr_list_push(&list_set->focus_stack_lists, &list_set->focus_stack_normal);
    wlr_list_push(&list_set->focus_stack_lists, &list_set->focus_stack_not_focusable);
    wlr_list_push(&list_set->focus_stack_lists, &list_set->focus_stack_hidden);

    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_layer_overlay);
    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_layer_top);
    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_on_top);
    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_normal);
    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_not_focusable);
    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_layer_bottom);
    wlr_list_push(&list_set->focus_stack_lists_with_layer_shell, &list_set->focus_stack_layer_background);

    wlr_list_push(&list_set->focus_stack_visible_lists, &list_set->focus_stack_on_top);
    wlr_list_push(&list_set->focus_stack_visible_lists, &list_set->focus_stack_normal);
    wlr_list_push(&list_set->focus_stack_visible_lists, &list_set->focus_stack_not_focusable);

    wlr_list_init(&list_set->all_lists);
    wlr_list_push(&list_set->all_lists, &list_set->floating_containers);
    wlr_list_push(&list_set->all_lists, &list_set->tiled_containers);
    wlr_list_push(&list_set->all_lists, &list_set->hidden_containers);
    wlr_list_push(&list_set->all_lists, &list_set->independent_containers);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_layer_background);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_layer_bottom);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_layer_top);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_layer_overlay);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_on_top);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_normal);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_hidden);
    wlr_list_push(&list_set->all_lists, &list_set->focus_stack_not_focusable);
    return list_set;
}

void destroy_list_set(struct list_set *list_set)
{
    wlr_list_clear(&list_set->change_affected_list_sets, NULL);
    wlr_list_finish(&list_set->change_affected_list_sets);
    wlr_list_clear(&list_set->container_lists, NULL);
    wlr_list_finish(&list_set->container_lists);
    wlr_list_clear(&list_set->visible_container_lists, NULL);
    wlr_list_finish(&list_set->visible_container_lists);
    wlr_list_clear(&list_set->independent_containers, NULL);
    wlr_list_finish(&list_set->independent_containers);
    wlr_list_clear(&list_set->tiled_containers, NULL);
    wlr_list_finish(&list_set->tiled_containers);
    wlr_list_clear(&list_set->hidden_containers, NULL);
    wlr_list_finish(&list_set->hidden_containers);
    wlr_list_clear(&list_set->floating_containers, NULL);
    wlr_list_finish(&list_set->floating_containers);
    wlr_list_clear(&list_set->focus_stack_lists, NULL);
    wlr_list_finish(&list_set->focus_stack_lists);
    wlr_list_clear(&list_set->focus_stack_visible_lists, NULL);
    wlr_list_finish(&list_set->focus_stack_visible_lists);
    wlr_list_clear(&list_set->focus_stack_lists_with_layer_shell, NULL);
    wlr_list_finish(&list_set->focus_stack_lists_with_layer_shell);
    wlr_list_clear(&list_set->focus_stack_layer_background, NULL);
    wlr_list_finish(&list_set->focus_stack_layer_background);
    wlr_list_clear(&list_set->focus_stack_layer_bottom, NULL);
    wlr_list_finish(&list_set->focus_stack_layer_bottom);
    wlr_list_clear(&list_set->focus_stack_layer_top, NULL);
    wlr_list_finish(&list_set->focus_stack_layer_top);
    wlr_list_clear(&list_set->focus_stack_layer_overlay, NULL);
    wlr_list_finish(&list_set->focus_stack_layer_overlay);
    wlr_list_clear(&list_set->focus_stack_layer_bottom, NULL);
    wlr_list_finish(&list_set->focus_stack_layer_bottom);
    wlr_list_clear(&list_set->focus_stack_on_top, NULL);
    wlr_list_finish(&list_set->focus_stack_on_top);
    wlr_list_clear(&list_set->focus_stack_normal, NULL);
    wlr_list_finish(&list_set->focus_stack_normal);
    wlr_list_clear(&list_set->focus_stack_hidden, NULL);
    wlr_list_finish(&list_set->focus_stack_hidden);
    wlr_list_clear(&list_set->focus_stack_not_focusable, NULL);
    wlr_list_finish(&list_set->focus_stack_not_focusable);
    wlr_list_clear(&list_set->all_lists, NULL);
    wlr_list_finish(&list_set->all_lists);
    free(list_set);
}

void append_list_set(struct list_set *dest, struct list_set *src)
{
    for (int i = 0; i < dest->all_lists.length; i++) {
        struct wlr_list *dest_list = dest->all_lists.items[i];
        struct wlr_list *src_list = src->all_lists.items[i];
        wlr_list_cat(dest_list, src_list);
    }
}

void clear_list_set(struct list_set *list_set)
{
    for (int i = 0; i < list_set->all_lists.length; i++) {
        struct wlr_list *dest_list = list_set->all_lists.items[i];
        wlr_list_clear(dest_list, NULL);
    }
}

void subscribe_list_set(struct list_set *dest, struct list_set *src)
{
    wlr_list_push(&src->change_affected_list_sets, dest);
    append_list_set(dest, src);
}

void unsubscribe_list_set(struct list_set *dest, struct list_set *src)
{
    wlr_list_remove(&src->change_affected_list_sets, cmp_ptr, dest);
}

void write_list_set(struct list_set *dest, struct list_set *src)
{
    clear_list_set(dest);
    append_list_set(dest, src);
}
