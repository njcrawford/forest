#ifndef _WUAAPI_H
#define _WUAAPI_H

#include "lineList.h"

int getAvailableUpdatesWuaApi(lineList* outList);
int applyUpdatesWuaApi(lineList* list);
#endif
