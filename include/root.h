#ifndef ROOT_H
#define ROOT_H

#include <wlr/types/wlr_box.h>
#include <wlr/util/edges.h>
#include "color.h"

struct tag;

struct root {
    /* window area(area where windows can tile) */
    struct wlr_box geom;
    struct output *m;
    struct color color;
};

struct root *create_root(struct output *m, struct wlr_box geom);
void destroy_root(struct root *root);
/* set the are where windows can be placed in respect to layershell based
 * programs which occupie space
 * The resulting root area is relative to outputs*/
void set_root_color(struct root *root, struct color color);
void set_root_geom(struct root *root, struct wlr_box geom);
void root_damage_whole(struct root *root);
void bars_update_visiblitiy(struct tag *tag);
enum wlr_edges get_bars_visible(struct tag *tag);
void toggle_bars_visible(struct tag *tag, enum wlr_edges direction);
void set_bars_visible(struct tag *tag, enum wlr_edges direction);

#endif /* ROOT_H */
