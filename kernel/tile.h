#ifndef TILE
#define TILE
#include "coreUtils.h"
#include "client.h"
#include <wayland-util.h>

void tile(Monitor *m);
void monocle(Monitor *m);

void resize(Client *c, int x, int y, int w, int h, int interact);

#endif
