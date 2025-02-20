#ifndef INFO_H
#define INFO_H

#include <lua.h>
#include <lauxlib.h>

void lua_load_info(lua_State *L);

int lib_get_container_under_cursor(lua_State *L);
int lib_is_keycombo(lua_State *L);
int lib_stack_position_to_position(lua_State *L);
// decided
// ws
int lib_get_this_container_count(lua_State *L);
// server
int lib_get_tag_count(lua_State *L);
// layout
int lib_is_container_not_in_limit(lua_State *L);
int lib_is_container_not_in_master_limit(lua_State *L);

#endif /* INFO_H */
