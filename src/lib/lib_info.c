#include "lib/lib_info.h"

#include <lauxlib.h>

#include "container.h"
#include "monitor.h"
#include "seat.h"
#include "server.h"
#include "tile/tileUtils.h"
#include "tag.h"
#include "layout.h"
#include "tagset.h"
#include "root.h"
#include "translationLayer.h"
#include "lib/lib_layout.h"
#include "lib/lib_tag.h"
#include "lib/lib_container.h"

static const struct luaL_Reg info_meta[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg info_static_methods[] =
{
    {"get_active_layout", lib_layout_get_layout_name},
    {"get_container_under_cursor", lib_get_container_under_cursor},
    {"get_next_empty_tag", lib_tag_get_next_empty},
    {"get_this_container_count", lib_get_this_container_count},
    {"get_tag_count", lib_get_tag_count},
    {"is_container_not_in_limit", lib_is_container_not_in_limit},
    {"is_container_not_in_master_limit", lib_is_container_not_in_master_limit},
    {"is_keycombo", lib_is_keycombo},
    {NULL, NULL},
};

static const struct luaL_Reg info_methods[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg info_setter[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg info_getter[] =
{
    {NULL, NULL},
};

void lua_load_info(lua_State *L)
{
    create_class(L,
            info_meta,
            info_static_methods,
            info_methods,
            info_setter,
            info_getter,
            CONFIG_INFO);

    luaL_newlib(L, info_static_methods);
    lua_setglobal(L, "Info");
}

int lib_get_this_container_count(lua_State *L)
{
    struct monitor *m = server_get_selected_monitor();
    struct tag *tag = monitor_get_active_tag(m);

    int i = get_slave_container_count(tag) + 1;
    lua_pushinteger(L, i);
    return 1;
}

int lib_get_tag_count(lua_State *L)
{
    lua_pushinteger(L, server_get_tag_count());
    return 1;
}

int lib_get_container_under_cursor(lua_State *L)
{
    struct seat *seat = input_manager_get_default_seat();
    struct wlr_cursor *wlr_cursor = seat->cursor->wlr_cursor;

    struct container *con = xy_to_container(wlr_cursor->x, wlr_cursor->y);
    create_lua_container(L, con);
    return 1;
}

int lib_get_root_area(lua_State *L)
{
    struct monitor *m = server_get_selected_monitor();
    struct root *root = m->root;
    lua_createtable(L, 1, 0);
    lua_pushinteger(L, root->geom.x);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, root->geom.y);
    lua_rawseti(L, -2, 2);
    lua_pushinteger(L, root->geom.width);
    lua_rawseti(L, -2, 3);
    lua_pushinteger(L, root->geom.height);
    lua_rawseti(L, -2, 4);
    return 1;
}

int lib_is_container_not_in_limit(lua_State *L)
{
    struct monitor *m = server_get_selected_monitor();
    struct layout *lt = get_layout_in_monitor(m);

    struct wlr_fbox geom = lua_togeometry(L);
    lua_pop(L, 1);

    bool not_in_limit = is_resize_not_in_limit(&geom, &lt->options->layout_constraints);
    return not_in_limit;
}

int lib_is_container_not_in_master_limit(lua_State *L)
{
    struct monitor *m = server_get_selected_monitor();
    struct layout *lt = get_layout_in_monitor(m);

    struct wlr_fbox geom = lua_togeometry(L);
    lua_pop(L, 1);

    bool not_in_limit = is_resize_not_in_limit(&geom, &lt->options->master_constraints);
    return not_in_limit;
}

int lib_is_keycombo(lua_State *L)
{
    const char *key_combo_name = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    bool found = server_is_keycombo(key_combo_name);
    lua_pushboolean(L, found);
    return 1;
}
