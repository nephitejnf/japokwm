#ifndef ROOT_H
#define ROOT_H

#include <wlr/types/wlr_box.h>

struct workspace;

struct root {
    /* window area(area where windows can tile) */
    struct wlr_box geom;
    struct monitor *m;
    float color[4];
};

struct root *create_root(struct monitor *m, struct wlr_box geom);
void destroy_root(struct root *root);
/* set the are where windows can be placed in respect to layershell based
 * programs which occupie space
 * The resulting root area is relative to outputs*/
void set_root_color(struct root *root, float color[static 4]);
void set_root_geom(struct root *root, struct wlr_box geom);
void root_damage_whole(struct root *root);
void set_bars_visible(struct workspace *ws, bool visible);
bool get_bars_visible(struct workspace *ws);
void toggle_bars_visible(struct workspace *ws);

#endif /* ROOT_H */
