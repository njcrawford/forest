#ifndef _YUM_H
#define _YUM_H

#include "PackageManager.h"

class Yum : public PackageManager
{
public:
    int applyUpdates(vector<string> * list);
    int getAvailableUpdates(vector<string> * outList);
}
#endif
