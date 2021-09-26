#include "tile/tileUtils.h"
#include <client.h>
#include <assert.h>
#include <execinfo.h>
#include <string.h>
#include <sys/param.h>
#include <wayland-util.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>
#include <wlr/types/wlr_box.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <stdlib.h>

#include "container.h"
#include "list_sets/container_stack_set.h"
#include "monitor.h"
#include "root.h"
#include "server.h"
#include "utils/coreUtils.h"
#include "utils/gapUtils.h"
#include "utils/parseConfigUtils.h"
#include "event_handler.h"
#include "layer_shell.h"
#include "workspace.h"
#include "list_sets/focus_stack_set.h"
#include "tagset.h"

static void arrange_container(struct container *con, struct monitor *m,
        int arrange_position, struct wlr_box root_geom, int inner_gap);

static void move_floating_containers_back()
{
    server_update_floating_containers();
    for (int i = 0; i < server.floating_containers->len; i++) {
        struct container *con = g_ptr_array_index(server.floating_containers, i);
        container_update_size(con);
    }
}

static void update_container_arranged_by_focus_state()
{
    // TODO: refactor this
    // manage the con->was_arranged_by_focus and con->arranged_by_focus variables
    struct tagset *tagset = monitor_get_active_tagset(selected_monitor);
    struct layout *mon_lt = tagset_get_layout(tagset);
    for (int i = 0; i < length_of_composed_list(tagset->visible_focus_set->focus_stack_lists); i++) {
        struct container *con = get_in_composed_list(tagset->visible_focus_set->focus_stack_lists, i);
        con->was_arranged_by_focus = con->arranged_by_focus;
        con->arranged_by_focus = mon_lt->options.arrange_by_focus;
    }

}

void arrange()
{
    for (int i = 0; i < server.floating_containers->len; i++) {
        struct container *con = g_ptr_array_index(server.floating_containers, i);
        container_set_hidden(con, false);
    }

    for (int i = 0; i < server.mons->len; i++) {
        struct monitor *m = g_ptr_array_index(server.mons, i);
        arrange_monitor(m);
    }

    update_container_arranged_by_focus_state();
    move_floating_containers_back();
}

static void set_layout_ref(struct layout *lt, int n_area)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, lt->lua_layout_copy_data_ref);

    lua_rawgeti(L, -1, n_area);
    lua_ref_safe(L, LUA_REGISTRYINDEX, &lt->lua_layout_ref);

    lua_pop(L, 1);
}

static int get_layout_container_area_count(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);
    lua_rawgeti(L, LUA_REGISTRYINDEX, lt->lua_layout_copy_data_ref);

    int len = luaL_len(L, -1);
    int container_area_count = get_container_area_count(tagset);
    int n_area = MAX(MIN(len, container_area_count), 1);

    lua_pop(L, 1);
    return n_area;
}

static int get_layout_container_max_area_count(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);
    lua_rawgeti(L, LUA_REGISTRYINDEX, lt->lua_layout_copy_data_ref);

    int len = luaL_len(L, -1);

    lua_rawgeti(L, -1, len);

    // TODO refactor
    int max_n_area = luaL_len(L, -1);

    lua_pop(L, 2);
    return max_n_area;
}

static void update_layout_counters(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);

    lt->n_all = get_container_count(tagset);
    lt->n_area = get_layout_container_area_count(tagset);
    set_layout_ref(lt, lt->n_area);
    lt->n_area_max = get_layout_container_max_area_count(tagset);
    lt->n_master_abs = get_master_container_count(tagset);
    lt->n_floating = get_floating_container_count(tagset);
    lt->n_tiled = lt->n_area-1 + lt->n_master_abs;
    lt->n_tiled_max = lt->n_area_max + lt->n_master_abs-1;
    lt->n_visible = lt->n_tiled + lt->n_floating;
    lt->n_hidden = lt->n_all - lt->n_visible;
}

static struct wlr_fbox lua_unbox_layout_geom(lua_State *L, int i) {
    struct wlr_fbox geom;

    if (luaL_len(L, -1) < i) {
        printf("ERROR: index to high: index %i len %lli", i, luaL_len(L, -1));
    }

    lua_rawgeti(L, -1, i);

    lua_rawgeti(L, -1, 1);
    geom.x = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, 2);
    geom.y = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, 3);
    geom.width = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, -1, 4);
    geom.height = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1);
    return geom;
}

/* update layout and was set in the arrange function */
static void apply_nmaster_layout(struct wlr_box *box, struct layout *lt, int position)
{
    if (position > lt->nmaster)
        return;

    // get layout
    lua_rawgeti(L, LUA_REGISTRYINDEX, lt->lua_master_layout_data_ref);
    int len = luaL_len(L, -1);
    int g = MIN(lt->n_master_abs, lt->nmaster);
    g = MAX(MIN(len, g), 1);
    lua_rawgeti(L, -1, g);
    int k = MIN(position, g);
    struct wlr_fbox geom = lua_unbox_layout_geom(L, k);
    lua_pop(L, 1);
    lua_pop(L, 1);

    struct wlr_box obox = get_absolute_box(geom, *box);
    memcpy(box, &obox, sizeof(struct wlr_box));
}

static struct wlr_box get_nth_geom_in_layout(lua_State *L, struct layout *lt, 
        struct wlr_box root_geom, int arrange_position)
{
    // relative position
    int n = MAX(0, arrange_position+1 - lt->nmaster) + 1;

    lua_rawgeti(L, LUA_REGISTRYINDEX, lt->lua_layout_ref);
    struct wlr_fbox rel_geom = lua_unbox_layout_geom(L, n);
    lua_pop(L, 1);

    struct wlr_box box = get_absolute_box(rel_geom, root_geom);

    // TODO fix this function, hard to read
    apply_nmaster_layout(&box, lt, arrange_position+1);
    return box;
}

int get_slave_container_count(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);
    int abs_count = get_tiled_container_count(tagset);
    return MAX(abs_count - lt->nmaster, 0);
}

int get_floating_container_count(struct tagset *tagset)
{
    struct layout *lt = tagset_get_layout(tagset);

    // there are no floating windows when using arrange by focus
    if (lt->options.arrange_by_focus)
        return 0;

    int n = 0;

    GPtrArray *floating_containers = tagset_get_floating_list_copy(tagset);
    for (int i = 0; i < floating_containers->len; i++) {
        struct container *con = g_ptr_array_index(floating_containers, i);
        if (con->client->type == LAYER_SHELL)
            continue;
        n++;
    }
    g_ptr_array_free(floating_containers, FALSE);
    return n;
}

int get_master_container_count(struct tagset *ts)
{
    int abs_count = get_tiled_container_count(ts);
    int slave_container_count = get_slave_container_count(ts);
    return MAX(abs_count - slave_container_count, 0);
}

// amount of slave containers plus the one master area
int get_container_area_count(struct tagset *ts)
{
    return get_slave_container_count(ts) + 1;
}

void arrange_monitor(struct monitor *m)
{
    m->geom = *wlr_output_layout_get_box(server.output_layout, m->wlr_output);
    set_root_geom(m->root, m->geom);
    struct wlr_box active_geom = monitor_get_active_geom(m);

    struct tagset *tagset = monitor_get_active_tagset(m);

    struct layout *lt = tagset_get_layout(tagset);
    container_surround_gaps(&active_geom, lt->options.outer_gap);

    update_layout_counters(tagset);
    call_update_function(lt->options.event_handler, lt->n_area);

    GPtrArray *tiled_containers = tagset_get_tiled_list_copy(tagset);

    update_hidden_status_of_containers(m, tiled_containers);

    arrange_containers(tagset, active_geom, tiled_containers);
    g_ptr_array_free(tiled_containers, FALSE);

    wlr_output_damage_whole(m->wlr_output);
    update_sub_focus_stack(tagset);
    struct workspace *ws = tagset_get_workspace(tagset);
    focus_most_recent_container(ws);
    update_visual_visible_stack(tagset);
}

void arrange_containers(struct tagset *tagset, struct wlr_box root_geom,
        GPtrArray *tiled_containers)
{
    struct layout *lt = tagset_get_layout(tagset);

    /* each container will get an inner_gap. If two containers are adjacent the
     * inner_gap is applied twice. To counter this effect we divide the
     * inner_gap by 2 */
    int actual_inner_gap = (int)lt->options.inner_gap/2;

    /* the root_geom must be reduced by the inner_gap to ensure that the
     * outer_gap stays unchanged when each container is surrounded by the
     * inner_gap. */
    container_surround_gaps(&root_geom, -actual_inner_gap);

    if (lt->options.smart_hidden_edges) {
        if (tiled_containers->len <= 1) {
            container_add_gaps(&root_geom, -lt->options.tile_border_px,
                    lt->options.hidden_edges);
        }
    } else {
        container_add_gaps(&root_geom, -lt->options.tile_border_px,
                lt->options.hidden_edges);
    }

    for (int i = 0; i < tiled_containers->len; i++) {
        struct container *con = g_ptr_array_index(tiled_containers, i);

        /* // the monitor must be on the same monitor as it is tiled on else it is */
        /* // a bug */
        /* printf("con: %i ws: %i monitor: %p\n", i, con->client->ws_id, container_get_monitor(con)); */
        /* printf("tagset: %p ->ws: %i ->m: %p\n", tagset, tagset->selected_ws_id, tagset->m); */
        /* assert(container_get_monitor(con) == tagset->m); */

        arrange_container(con, tagset->m, i, root_geom, actual_inner_gap);
    }
}

static void arrange_container(struct container *con, struct monitor *m,
        int arrange_position, struct wlr_box root_geom, int inner_gap)
{
    if (container_get_hidden(con))
        return;

    struct layout *lt = get_layout_in_monitor(m);
    struct wlr_box geom = get_nth_geom_in_layout(L, lt, root_geom, arrange_position);
    container_surround_gaps(&geom, inner_gap);

    if (container_is_floating(con)) {
        con->floating_container_geom_was_changed = true;
        container_set_border_width(con, lt->options.float_border_px);
    } else {
        container_set_border_width(con, lt->options.tile_border_px);
    }

    // since gaps are halfed we need to multiply it by 2
    int border_width = container_get_border_width(con);
    container_surround_gaps(&geom, 2*border_width);

    container_set_tiled_geom(con, &geom);
}

void container_update_size(struct container *con)
{
    /*
     * Note that I took some shortcuts here. In a more fleshed-out
     * compositor, you'd wait for the client to prepare a buffer at
     * the new size, then commit any movement that was prepared.
     */

    con->client->resized = true;


    struct wlr_box *con_geom = container_get_current_geom(con);
    apply_bounds(con, *wlr_output_layout_get_box(server.output_layout, NULL));

    /* wlroots makes this a no-op if size hasn't changed */
    switch (con->client->type) {
        case XDG_SHELL:
            if (con->client->surface.xdg->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
                wlr_xdg_toplevel_set_size(con->client->surface.xdg,
                        con_geom->width, con_geom->height);
            }
            break;
        case LAYER_SHELL:
            {
                struct monitor *m = container_get_monitor(con);
                int width = m->geom.width;
                int height = m->geom.height;
                if (con->client->surface.layer->current.desired_width > 0)
                    width = con->client->surface.layer->current.desired_width;
                if (con->client->surface.layer->current.desired_height > 0)
                    height = con->client->surface.layer->current.desired_width;
                wlr_layer_surface_v1_configure(con->client->surface.layer,
                        width,
                        height);
            }
            break;
        case X11_UNMANAGED:
        case X11_MANAGED:
            wlr_xwayland_surface_configure(con->client->surface.xwayland,
                    con_geom->x, con_geom->y, con_geom->width,
                    con_geom->height);
    }
}

void update_hidden_status_of_containers(struct monitor *m, GPtrArray *tiled_containers)
{
    struct layout *lt = get_layout_in_monitor(m);

    for (int i = 0; i < tiled_containers->len; i++) {
        struct container *con = g_ptr_array_index(tiled_containers, i);

        bool is_hidden = i >= lt->n_tiled;
        container_set_hidden(con, is_hidden);
    }
}

int get_container_count(struct tagset *tagset)
{
    return tagset->con_set->tiled_containers->len;
}

int get_tiled_container_count(struct tagset *tagset)
{
    int n = 0;
    GPtrArray *tiled_containers = tagset_get_tiled_list_copy(tagset);

    n = tiled_containers->len;
    g_ptr_array_free(tiled_containers, FALSE);
    return n;
}
