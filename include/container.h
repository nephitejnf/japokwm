#ifndef CONTAINER_H
#define CONTAINER_H

#include <lua.h>
#include <wlr/types/wlr_box.h>
#include <wlr/types/wlr_cursor.h>
#include <glib.h>

struct monitor;
struct resize_constraints;
struct tag;

struct container_property {
    // geometry on each layout
    struct wlr_box geom;
    struct wlr_box floating_geom;
    /* layout-relative, includes border */
    int border_width;
    bool floating;
    bool hidden;
    struct container *con;
};

struct container {
    GPtrArray *properties;

    // if this is set it will overwrite the other geometries
    struct wlr_box global_geom;
    struct wlr_box prev_geom;
    struct client *client;

    int tag_id;

    bool is_exclusive;
    bool is_on_tile;
    bool is_xwayland_popup;
    bool focusable;
    bool has_border;
    bool on_scratchpad;
    bool on_top;
    bool is_unmanaged;

    // height = ratio * width
    float ratio;
    float alpha;
};

struct container *create_container(struct client *c, struct monitor *m, bool has_border);

void destroy_container(struct container *con);

bool container_property_is_floating(struct container_property *property);
struct wlr_box *container_property_get_floating_geom(struct container_property *property);
void container_property_set_floating_geom(struct container_property *property, struct wlr_box *geom);
void container_property_set_floating(struct container_property *property, bool floating);

void add_container_to_tile(struct container *con);
void remove_container_from_tile(struct container *con);

struct container *monitor_get_focused_container(struct monitor *m);
struct container *xy_to_container(double x, double y);
struct container *get_container_on_focus_stack(int tag_id, int position);

struct wlr_box get_center_box(struct wlr_box ref);
struct wlr_box get_centered_box(struct wlr_box box, struct wlr_box ref);
struct wlr_box get_absolute_box(struct wlr_fbox ref, struct wlr_box box);
struct wlr_fbox get_relative_box(struct wlr_box box, struct wlr_box ref);
struct wlr_box get_monitor_local_box(struct wlr_box box, struct monitor *m);
struct wlr_fbox lua_togeometry(lua_State *L);

void ack_configure(struct wl_listener *listener, void *data);
void apply_bounds(struct container *con, struct wlr_box bbox);
void commit_notify(struct wl_listener *listener, void *data);
void configure_notify(struct wl_listener *listener, void *data);
void container_damage_borders(struct container *con, struct monitor *m, struct wlr_box *geom);
void container_damage_part(struct container *con);
void container_damage_whole(struct container *con);
void container_fix_position_to_begin(struct container *con);
void container_fix_position(struct container *con);
void focus_on_hidden_stack(struct monitor *m, int i);
void swap_on_hidden_stack(struct monitor *m, int i);
void focus_on_stack(struct monitor *m, int i);
/* Find the topmost visible client (if any) at point (x, y), including
 * borders. This relies on stack being ordered from top to bottom. */
void lift_container(struct container *con);
void repush(int pos, int pos2);
void container_set_floating(struct container *con, void (*fix_position)(struct container *con),
        bool floating);
void container_set_hidden(struct container *con, bool b);
void container_set_hidden_at_tag(struct container *con, bool b, struct tag *tag);
void set_container_monitor(struct container *con, struct monitor *m);
void resize_container(struct container *con, struct wlr_cursor *cursor, int dx, int dy);
void move_container(struct container *con, struct wlr_cursor *cursor, int offsetx, int offsety);

struct container_property *container_get_property(struct container *con);
struct container_property *container_get_property_at_tag(
        struct container *con,
        struct tag *tag);

void container_set_floating_geom_at_tag(struct container *con,
        struct wlr_box *geom, struct tag *tag);
void container_set_current_geom(struct container *con, struct wlr_box *geom);
void container_set_tiled_geom(struct container *con, struct wlr_box *geom);
void container_set_floating_geom(struct container *con, struct wlr_box *geom);

struct wlr_box *container_get_tiled_geom_at_tag(struct container *con, struct tag *tag);
struct wlr_box *container_get_floating_geom_at_tag(struct container *con, struct tag *tag);
struct wlr_box *container_get_current_geom_at_tag(struct container *con, struct tag *tag);
struct wlr_box *container_get_tiled_geom(struct container *con);
struct wlr_box *container_get_floating_geom(struct container *con);
struct wlr_box *container_get_current_geom(struct container *con);

bool container_get_hidden(struct container *con);
bool container_get_hidden_at_tag(struct container *con, struct tag *tag);

void container_set_border_width(struct container *con, int border_width);
int container_get_border_width(struct container *con);

void container_set_just_tag_id(struct container *con, int tag_id);
void container_set_tag_id(struct container *con, int tag_id);
void container_set_tag(struct container *con, struct tag *tag);
void move_container_to_tag(struct container *con, struct tag *tag);

struct monitor *container_get_monitor(struct container *con);

int absolute_x_to_container_relative(struct wlr_box *geom, int x);
int absolute_y_to_container_relative(struct wlr_box *geom, int y);
int get_position_in_container_focus_stack(struct container *con);
int get_position_in_container_stack(struct container *con);

struct tag *container_get_current_tag(struct container *con);

struct container *get_container_from_container_stack_position(int i);

bool is_resize_not_in_limit(struct wlr_fbox *geom, struct resize_constraints *resize_constraints);
bool container_is_bar(struct container *con);

// this function may return NULL when a container is hidden
struct tag *container_get_tag(struct container *con);
bool container_is_floating(struct container *con);
bool container_is_floating_and_visible(struct container *con);
bool container_is_floating_on_tag(struct container *con, struct tag *tag);
bool container_is_tiled(struct container *con);
bool container_is_tiled_and_visible(struct container *con);
bool container_is_hidden(struct container *con);
bool container_is_visible(struct container *con);
bool container_potentially_visible(struct container *con);
bool container_exists(struct container *con);
bool container_is_unmanaged(struct container *con);
bool container_is_managed(struct container *con);
bool container_is_tiled_and_managed(struct container *con);
bool container_is_on_scratchpad(struct container *con);

const char *container_get_app_id(struct container *con);
#endif /* CONTAINER_H */
