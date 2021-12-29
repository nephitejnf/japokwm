#include "lib/lib_list2D.h"

#include "translationLayer.h"
#include "server.h"
#include "lib/lib_container.h"
#include "utils/coreUtils.h"
#include "tile/tileUtils.h"
#include "tagset.h"
#include "list_sets/list_set.h"
#include "tag.h"

#include <stdlib.h>
#include <ctype.h>

static const struct luaL_Reg list2D_meta[] =
{
    {"__index", lib_list2D_get},
    {NULL, NULL},
};

static const struct luaL_Reg list2D_static_methods[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg list2D_methods[] = {
    {"find", lib_list2D_find},
    {"get", lib_list2D_get},
    {"repush", lib_list2D_repush},
    {"swap", lib_list2D_swap},
    {NULL, NULL},
};

static const struct luaL_Reg list2D_getter[] = {
    {"len", lib_list2D_length},
    {NULL, NULL},
};

static const struct luaL_Reg list2D_setter[] = {
    {NULL, NULL},
};

void create_lua_list2D(lua_State *L, GPtrArray2D *arr)
{
    if (!arr) {
        lua_pushnil(L);
        return;
    }
    GPtrArray **user_con = lua_newuserdata(L, sizeof(GPtrArray *));
    *user_con = arr;

    luaL_setmetatable(L, CONFIG_LIST2D);
}

void lua_load_list2D(lua_State *L)
{
    create_class(L,
            list2D_meta,
            list2D_static_methods,
            list2D_methods,
            list2D_setter,
            list2D_getter,
            CONFIG_LIST2D);

    luaL_newlib(L, list2D_static_methods);
    lua_setglobal(L, "List2D");
}

GPtrArray *check_list2D(lua_State *L, int argn)
{
    void **ud = luaL_checkudata(L, argn, CONFIG_LIST2D);
    luaL_argcheck(L, ud != NULL, argn, "`list' expected");
    return (GPtrArray *)*ud;
}

// static methods
int lib_list2D_to_array(lua_State *L)
{
    GPtrArray *array = check_list2D(L, 1);
    lua_pop(L, 1);
    lua_createtable(L, array->len, 0);
    for (int i = 0; i < array->len; i++) {
        struct container *con = get_in_composed_list(array, i);
        create_lua_container(L, con);
        lua_rawseti (L, -2, i+1); /* In lua indices start at 1 */
    }
    return 0;
}

// methods
int lib_list2D_find(lua_State *L)
{
    struct container *con = check_container(L, 2);
    lua_pop(L, 1);
    GPtrArray *array = check_list2D(L, 1);
    lua_pop(L, 1);

    guint pos = find_in_composed_list(array, NULL, con);

    lua_pushinteger(L, c_idx_to_lua_idx(pos));
    return 1;
}

int lib_list2D_get(lua_State *L)
{
    const char *key = luaL_checkstring(L, -1);
    GPtrArray *array = check_list2D(L, 1);
    debug_print("key: %s\n", key);

    bool is_number = lua_isnumber(L, -1);
    if (!is_number) {
        get_lua_value(L);
        return 1;
    }
    int i = lua_tonumber(L, -1)-1;

    if (i < 0) {
        lua_pushnil(L);
        return 1;
    }
    if (i >= array->len) {
        lua_pushnil(L);
        return 1;
    }

    struct container *con = get_in_composed_list(array, i);
    create_lua_container(L, con);
    return 1;
}

int lib_list2D_swap(lua_State *L)
{
    int j = lua_idx_to_c_idx(luaL_checkinteger(L, -1));
    lua_pop(L, 1);
    int i = lua_idx_to_c_idx(luaL_checkinteger(L, -1));
    lua_pop(L, 1);
    GPtrArray *array = check_list2D(L, 1);
    lua_pop(L, 1);

    tag_swap_containers(array, i, j);

    struct tag *tag = server_get_selected_tag();
    tagset_reload(tag);
    arrange();
    return 0;
}

int lib_list2D_repush(lua_State *L)
{
    int abs_index = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    int i = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    GPtrArray2D *array2D = check_list2D(L, 1);
    lua_pop(L, 1);

    GPtrArray *array = list2D_flatten(array2D);
    array = NULL;

    tag_repush_containers(array, i, abs_index);

    sub_list_write_to_parent_list(array2D, array);
    g_ptr_array_unref(array);

    struct tag *tag = server_get_selected_tag();
    tagset_reload(tag);
    arrange();
    return 0;
}

// getter
int lib_list2D_length(lua_State *L)
{
    GPtrArray2D* list = check_list2D(L, 1);
    lua_pop(L, 1);
    int composed_list_length = length_of_composed_list(list);
    lua_pushinteger(L, composed_list_length);
    return 1;
}
// setter
