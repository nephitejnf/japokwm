#include "utils/gapUtils.h"
#include <math.h>

static void container_add_gap_left(struct wlr_box *con, float gap)
{
    con->x += gap;
}

static void container_add_gap_right(struct wlr_box *con, float gap)
{
    con->width = MAX(1, con->width - gap);
}

static void container_add_gap_top(struct wlr_box *con, float gap)
{
    con->y += gap;
}

static void container_add_gap_bottom(struct wlr_box *con, float gap)
{
    con->height = MAX(1, con->height - gap);
}

void container_add_gaps(struct wlr_box *con, double gap, enum wlr_edges edges) {
    if (edges & WLR_EDGE_LEFT)
        container_add_gap_left(con, gap/2);
    if (edges & WLR_EDGE_RIGHT)
        container_add_gap_right(con, gap);
    if (edges & WLR_EDGE_TOP)
        container_add_gap_top(con, gap/2);
    if (edges & WLR_EDGE_BOTTOM)
        container_add_gap_bottom(con, gap);
}

void container_surround_gaps(struct wlr_box *con, double gap)
{
    /* *
     * left = x and top = y
     * right = width and bottom = height
     * +------------+
     * |            |
     * |   +----+   |
     * |-->|    |<--|
     * | x +----+ x |
     * |            |
     * +------------+
     * therefore x and y need to be 1/2ed and the width has to be decreased
     * by the whole amound
     * */
        container_add_gaps(con, gap, WLR_EDGE_TOP | WLR_EDGE_LEFT);
        container_add_gaps(con, gap, WLR_EDGE_RIGHT | WLR_EDGE_BOTTOM);
}

void configure_gaps(int *inner_gap, int *outer_gap)
{
    /* innerGaps are applied twice because gaps don't overlap from two
     containers therefore it has to be divided by 2*/
    *inner_gap /= 2;
    /* outerGap + innerGap = resultGap but we only want the outerGap */
    *outer_gap -= *inner_gap;
}
