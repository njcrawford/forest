#ifndef _APTGET_H
#define _APTGET_H

#include "lineList.h"

int getAvailableUpdatesAptGet(lineList* outList);
int applyUpdatesAptGet(lineList* list);
#endif
