#ifndef _YUM_H
#define _YUM_H

#include "lineList.h"

int applyUpdatesYum(lineList* list);
int getAvailableUpdatesYum(lineList* outList);
#endif
