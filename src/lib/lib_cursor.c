#include "lib/lib_cursor.h"

#include "translationLayer.h"
#include "cursor.h"

static const struct luaL_Reg cursor_meta[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg cursor_static_methods[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg cursor_methods[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg cursor_setter[] =
{
    {NULL, NULL},
};

static const struct luaL_Reg cursor_getter[] =
{
    {NULL, NULL},
};

void lua_load_cursor(lua_State *L)
{
    create_class(L, cursor_meta,
            cursor_static_methods,
            cursor_methods,
            cursor_setter,
            cursor_getter,
            CONFIG_CURSOR);

    luaL_newlib(L, cursor_static_methods);
    lua_setglobal(L, "Cursor");
}

struct cursor *check_cursor(lua_State *L, int narg) {
    void **ud = luaL_checkudata(L, narg, CONFIG_CURSOR);
    luaL_argcheck(L, ud != NULL, narg, "`cursor' expected");
    return *(struct cursor **)ud;
}

static void create_lua_cursor(lua_State *L, struct cursor cursor) {
    struct cursor *user_cursor = lua_newuserdata(L, sizeof(struct cursor));
    *user_cursor = cursor;

    luaL_setmetatable(L, CONFIG_CURSOR);
}

