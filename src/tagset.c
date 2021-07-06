#include "tagset.h"

#include <assert.h>

#include "bitset/bitset.h"
#include "list_set.h"
#include "server.h"
#include "workspace.h"
#include "utils/coreUtils.h"
#include "utils/parseConfigUtils.h"
#include "tile/tileUtils.h"
#include "ipc-server.h"
#include "monitor.h"

static void tagset_unload_workspaces(struct tagset *tagset);

struct tagset *create_tagset(struct monitor *m, int selected_ws_id, BitSet workspaces)
{
    struct tagset *tagset = calloc(1, sizeof(struct tagset));
    tagset->m = m;

    tagset->selected_ws_id = selected_ws_id;
    struct workspace *selected_workspace = get_workspace(tagset->selected_ws_id);
    selected_workspace->tagset = tagset;

    setup_list_set(&tagset->list_set);

    bitset_setup(&tagset->workspaces, server.workspaces.length);
    tagset_set_tags(tagset, workspaces);

    wlr_list_push(&server.tagsets, tagset);
    return tagset;
}

void destroy_tagset(struct tagset *tagset)
{
    if (!tagset)
        return;
    printf("destroy tagset: %p\n", tagset);
    tagset_unload_workspaces(server.previous_tagset);
    wlr_list_remove(&server.tagsets, cmp_ptr, tagset);
    free(tagset);
}

void focus_most_recent_container(struct tagset *tagset, enum focus_actions a)
{
    struct container *con = get_in_composed_list(&tagset->list_set.focus_stack_lists, 0);

    if (!con) {
        con = get_container(tagset, 0);
        if (!con) {
            ipc_event_window();
            return;
        }
    }

    focus_container(con, a);
}

static void unfocus_tagset(struct tagset *old_tagset)
{
    if (!old_tagset)
        return;

    printf("\n");
    for (int i = 0; i < old_tagset->workspaces.size; i++) {
        bool bit = bitset_test(&old_tagset->workspaces, i);
        /* bool is_selected_workspace = i == old_tagset->selected_ws_id; */

        if (!bit)
            continue;

        struct workspace *ws = get_workspace(i);
        bool is_empty = is_workspace_empty(ws);
        if (is_empty && ws->m == selected_monitor) {
            printf("unfocus workspace: %zu\n", ws->id);
            ws->m = NULL;
        }
    }
}

static void focus_action(struct tagset *tagset)
{
    struct monitor *m = tagset->m;
    for (int i = 0; i < tagset->workspaces.size; i++) {
        bool bit = bitset_test(&tagset->workspaces, i);

        if (!bit)
            continue;

        struct workspace *ws = get_workspace(i);
        ws->tagset = m->tagset;
    }

    struct workspace *ws = get_workspace(tagset->selected_ws_id);
    if (!ws->m) {
        ws->m = tagset->m;
    }
}

void focus_tagset(struct tagset *tagset)
{
    if(!tagset)
        return;

    struct monitor *m = tagset->m;
    focus_monitor(m);

    struct tagset *old_tagset = m->tagset;
    unfocus_tagset(old_tagset);
    printf("old_tagset: %p\n", old_tagset);
    printf("new_tagset: %p\n", tagset);

    m->tagset = tagset;
    focus_action(tagset);

    ipc_event_workspace();

    arrange();
    focus_most_recent_container(m->tagset, FOCUS_NOOP);
    root_damage_whole(m->root);
}

static void tagset_unload_workspaces(struct tagset *tagset)
{
    if (!tagset)
        return;
    if (!tagset->loaded) {
        return;
    }

    clear_list_set(&tagset->list_set);

    for (int i = 0; i < tagset->workspaces.size; i++) {
        bool bit = bitset_test(&tagset->workspaces, i);

        if (!bit)
            continue;

        struct workspace *ws = get_workspace(i);

        wlr_list_remove(&ws->list_set.change_affected_list_sets, cmp_ptr, &tagset->list_set);
    }
    tagset->loaded = false;
}

static void tagset_save_to_workspaces(struct tagset *tagset)
{
    if (!tagset)
        return;
    if (!tagset->loaded) {
        printf("already unloaded\n");
        return;
    }

    for (int i = 0; i < tagset->workspaces.size; i++) {
        bool bit = bitset_test(&tagset->workspaces, i);

        if (!bit)
            continue;

        struct workspace *ws = get_workspace(i);
        clear_list_set(&ws->list_set);

        // TODO convert to a function
        for (int i = 0; i < ws->list_set.all_lists.length; i++) {
            struct wlr_list *dest_list = ws->list_set.all_lists.items[i];
            struct wlr_list *src_list = tagset->list_set.all_lists.items[i];
            for (int j = 0; j < src_list->length; j++) {
                struct container *con = src_list->items[j];
                if (con->client->ws_id != ws->id)
                    continue;
                wlr_list_push(dest_list, con);
            }
        }
    }
}

struct layout *tagset_get_layout(struct tagset *tagset)
{
    struct workspace *ws = get_workspace(tagset->selected_ws_id);
    return ws->layout;
}

void tagset_load_workspaces(struct tagset *tagset)
{
    if (!tagset)
        return;
    if (tagset->loaded) {
        printf("already loaded\n");
        return;
    }

    for(size_t i = 0; i < tagset->workspaces.size; i++) {
        bool bit = bitset_test(&tagset->workspaces, i);

        if (!bit)
            continue;

        struct workspace *ws = get_workspace(i);
        subscribe_list_set(&tagset->list_set, &ws->list_set);
        ws->tagset = tagset;
        ws->m = tagset->m;
    }
    tagset->loaded = true;
}

void tagset_set_tags(struct tagset *tagset, BitSet bitset)
{
    struct monitor *m = tagset->m;

    tagset_save_to_workspaces(m->tagset);
    tagset_unload_workspaces(tagset);

    bitset_move(&tagset->workspaces, &bitset);
    tagset_load_workspaces(tagset);

    ipc_event_workspace();
}

static void tagset_push_queue(struct tagset *prev_tagset, struct tagset *tagset)
{
    if (prev_tagset->selected_ws_id == tagset->selected_ws_id)
        return;

    if (server.previous_tagset != tagset) {
        destroy_tagset(server.previous_tagset);
    }

    server.previous_tagset = prev_tagset;
}

void push_tagset(struct tagset *tagset)
{
    if (!tagset)
        return;

    struct monitor *m = tagset->m;
    printf("m->tagset: %p vs %p\n", m->tagset, tagset);

    tagset_save_to_workspaces(m->tagset);

    tagset_push_queue(m->tagset, tagset);

    focus_tagset(tagset);
}

void tagset_focus_workspace(int ws_id)
{
    BitSet bitset;
    bitset_setup(&bitset, server.workspaces.length);
    bitset_set(&bitset, ws_id);
    tagset_focus_tags(ws_id, bitset);
}

void tagset_toggle_add(struct tagset *tagset, BitSet bitset)
{
    if (!tagset)
        return;

    BitSet new_bitset;
    bitset_copy(&new_bitset, &bitset);
    bitset_xor(&new_bitset, &tagset->workspaces);

    unfocus_tagset(tagset);
    tagset_set_tags(tagset, new_bitset);
    focus_action(tagset);
    ipc_event_workspace();
}

void tagset_focus_tags(int ws_id, struct BitSet bitset)
{
    struct workspace *ws = get_workspace(ws_id);
    struct monitor *m = ws->m ? ws->m : selected_monitor;

    struct tagset *tagset = get_tagset_from_active_workspace_id(ws_id);
    if (tagset) {
        tagset_set_tags(tagset, bitset);
    } else {
        tagset = create_tagset(m, ws_id, bitset);
    }

    push_tagset(tagset);
}


void tagset_toggle_add_workspace_id(struct tagset *tagset, int ws_id)
{
    // TODO implement
}

struct tagset *get_tagset_from_active_workspace_id(int ws_id)
{
    for (int i = 0; i < server.mons.length; i++) {
        struct monitor *m = server.mons.items[i];
        struct tagset *tagset = m->tagset;

        if (!tagset)
            continue;

        if (tagset->selected_ws_id == ws_id) {
            return tagset;
        }
    }
    return NULL;
}

struct tagset *get_tagset_from_workspace_id(int ws_id)
{
    for (int i = 0; i < server.mons.length; i++) {
        struct monitor *m = server.mons.items[i];
        struct tagset *tagset = m->tagset;

        if (!tagset)
            continue;

        BitSet bitset;
        bitset_setup(&bitset, server.workspaces.length);
        bitset_set(&bitset, ws_id);
        bitset_and(&bitset, &tagset->workspaces);
        if (bitset_any(&bitset)) {
            bitset_destroy(&bitset);
            return tagset;
        }
    }
    return NULL;
}

struct container *get_container(struct tagset *ts, int i)
{
    struct list_set *list_set = &ts->list_set;
    if (!list_set)
        return NULL;

    return get_in_composed_list(&list_set->container_lists, i);
}

static bool container_intersects_with_monitor(struct container *con, struct monitor *m)
{
    if (!con)
        return false;
    if (!m)
        return false;

    struct wlr_box tmp_geom;
    return wlr_box_intersection(&tmp_geom, &con->geom, &m->geom);
}

struct wlr_list *tagset_get_visible_lists(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);

    if (lt->options.arrange_by_focus)
        return &tagset->list_set.focus_stack_visible_lists;
    else
        return &tagset->list_set.visible_container_lists;
}

struct wlr_list *tagset_get_tiled_list(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);

    if (lt->options.arrange_by_focus)
        return &tagset->list_set.focus_stack_normal;
    else
        return &tagset->list_set.tiled_containers;
}

struct wlr_list *tagset_get_floating_list(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);

    if (lt->options.arrange_by_focus)
        return &tagset->list_set.focus_stack_normal;
    else
        return &tagset->list_set.floating_containers;
}

struct wlr_list *tagset_get_hidden_list(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);

    if (lt->options.arrange_by_focus)
        return &tagset->list_set.focus_stack_hidden;
    else
        return &tagset->list_set.hidden_containers;
}

void workspace_id_to_tag(BitSet *dest, int ws_id)
{
    bitset_set(dest, ws_id);
}

bool tagset_contains_client(struct tagset *tagset, struct client *c)
{

    BitSet bitset;
    bitset_setup(&bitset, server.workspaces.length);
    workspace_id_to_tag(&bitset, c->ws_id);
    bitset_and(&bitset, &tagset->workspaces);
    return bitset_any(&bitset);
}

bool tagset_has_clients(struct tagset *tagset)
{
    if (!tagset)
        return false;

    for (int i = 0; i < server.normal_clients.length; i++) {
        struct client *c = server.normal_clients.items[i];

        if (tagset_contains_client(tagset, c))
            return true;
    }

    return false;
}

bool workspace_has_clients(struct tagset *tagset)
{
    if (!tagset)
        return false;

    for (int i = 0; i < server.normal_clients.length; i++) {
        struct client *c = server.normal_clients.items[i];

        if (tagset_contains_client(tagset, c))
            return true;
    }

    return false;
}

bool hidden_on(struct container *con, struct tagset *ts)
{
    return !visible_on(con, ts) && exist_on(con, ts);
}

bool visible_on(struct container *con, struct tagset *ts)
{
    if (!con)
        return false;
    if (con->hidden)
        return false;

    return exist_on(con, ts);
}

bool exist_on(struct container *con, struct tagset *tagset)
{
    if (!con || !tagset)
        return false;
    if (con->m != tagset->m) {
        if (con->floating)
            return container_intersects_with_monitor(con, tagset->m)
                && tagset_contains_client(con->m->tagset, con->client);
        else
            return false;
    }

    struct client *c = con->client;

    if (!c)
        return false;

    if (c->type == LAYER_SHELL) {
        return true;
    }
    if (c->sticky) {
        return true;
    }

    return tagset_contains_client(tagset, c);
}

int tagset_get_container_count(struct tagset *ts)
{
    if (!ts)
        return -1;

    int i = 0;
    for (int i = 0; i < ts->list_set.tiled_containers.length; i++) {
        struct container *con = get_container(ts, i);

        if (visible_on(con, ts))
            i++;
    }
    return i;
}
