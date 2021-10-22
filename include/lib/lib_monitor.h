#ifndef LIB_MONITOR_H
#define LIB_MONITOR_H

#include <lua.h>

void lua_load_monitor();

// functions
int lib_monitor_get_focused(lua_State *L);
// methods

// getter
int lib_monitor_get_workspace(lua_State *L);
// setter
int lib_monitor_set_scale(lua_State *L);
int lib_monitor_set_transform(lua_State *L);

#endif /* LIB_MONITOR_H */
