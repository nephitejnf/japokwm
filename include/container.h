#ifndef CONTAINER_H
#define CONTAINER_H

#include <lua.h>
#include <wlr/types/wlr_box.h>
#include <wlr/types/wlr_cursor.h>

#include "client.h"
#include "options.h"
#include "monitor.h"

struct container {
    /* sticky containers */
    struct wl_list stlink;

    struct wl_listener commit;

    /* layout-relative, includes border */
    struct wlr_box geom;
    struct wlr_box prev_geom;
    struct wlr_box prev_floating_geom;
    struct client *client;

    /* struct monitor *m; */
    bool floating;
    bool focusable;
    bool has_border;
    bool hidden;
    bool on_scratchpad;
    bool on_top;
    // if position -1 it is floating
    bool geom_was_changed;
    // height = ratio * width
    float ratio;
    float alpha;
};

struct container *create_container(struct client *c, struct monitor *m, bool has_border);

void destroy_container(struct container *con);

struct container *get_focused_container(struct monitor *m);
struct container *xy_to_container(double x, double y);
struct container *get_container_on_focus_stack(int ws_id, int position);

struct wlr_box get_center_box(struct wlr_box ref);
struct wlr_box get_centered_box(struct wlr_box box, struct wlr_box ref);
struct wlr_box get_absolute_box(struct wlr_fbox ref, struct wlr_box box);
struct wlr_fbox get_relative_box(struct wlr_box box, struct wlr_box ref);
struct wlr_box get_monitor_local_box(struct wlr_box box, struct monitor *m);
struct wlr_fbox lua_togeometry(lua_State *L);

void apply_bounds(struct container *con, struct wlr_box bbox);
void apply_rules(struct container *con);
void commit_notify(struct wl_listener *listener, void *data);
void container_damage_borders(struct container *con, struct wlr_box *geom);
void container_damage_part(struct container *con);
void container_damage_whole(struct container *con);
void fix_position(struct container *con);
void focus_container(struct container *con, enum focus_actions a);
void focus_on_hidden_stack(struct monitor *m, int i);
void focus_on_stack(struct monitor *m, int i);
/* Find the topmost visible client (if any) at point (x, y), including
 * borders. This relies on stack being ordered from top to bottom. */
void lift_container(struct container *con);
void repush(int pos, int pos2);
void set_container_floating(struct container *con, void (*fix_position)(struct container *con), bool floating);
void set_container_hidden_status(struct container *con, bool b);
void set_container_monitor(struct container *con, struct monitor *m);
void resize_container(struct container *con, struct wlr_cursor *cursor, int dx, int dy);
void move_container(struct container *con, struct wlr_cursor *cursor, int offsetx, int offsety);

void add_container_to_containers(struct list_set *list_set, struct container *con, int i);
void list_set_add_container_to_focus_stack(struct list_set *list_set, struct container *con);
void add_container_to_stack(struct container *con);

void list_set_remove_container(struct list_set *list_set, struct container *con);
void list_set_remove_container_from_focus_stack(struct list_set *list_set, struct container *con);
void list_set_remove_independent_container(struct list_set *list_set, struct container *con);

void set_container_workspace(struct container *con, struct workspace *ws);
void move_container_to_workspace(struct container *con, struct workspace *ws);

struct monitor *container_get_monitor(struct container *con);

int absolute_x_to_container_relative(struct container *con, int x);
int absolute_y_to_container_relative(struct container *con, int y);
int get_position_in_container_stack(struct container *con);

struct container *get_container_from_container_stack_position(int i);

bool is_resize_not_in_limit(struct wlr_fbox *geom, struct resize_constraints *resize_constraints);
#endif /* CONTAINER_H */
