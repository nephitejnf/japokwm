#ifndef TILEUTILS
#define TILEUTILS
#include "utils/coreUtils.h"
#include "client.h"
#include "tile/tileTexture.h"

struct client *focustop(struct monitor *m);
void arrange(struct monitor *m, bool reset);
void arrangeThis(bool reset);
void resize(struct client *c, int x, int y, int w, int h, bool interact);
void updateHiddenStatus();
void updateLayout();
int thisTiledClientCount();
int tiledClientCount(struct monitor *m);
int clientPos();

extern struct containersInfo containersInfo;
#endif
